#include <boost/asio/error.hpp>
#include <chrono>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <rapidjson/stringbuffer.h>
#include <stdexcept>
#include <thread>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/allocators.h>
#include <mosquitto.h>

#include <boost/asio.hpp>
#include <boost/array.hpp>


// #include <bitset>
class IPCConnection {
public:
    IPCConnection() : m_socket(m_io_service) {
        m_socket.open(boost::asio::ip::tcp::v4());
        m_socket.non_blocking(true);
    }

    bool ShouldRun() {
        CheckSocket();
        return m_run;
    }

    void Connect(const std::string & raw_address, const uint32_t port) {
        const boost::asio::ip::tcp::resolver resolver(m_io_service);
        const boost::asio::ip::address address = boost::asio::ip::address::from_string(raw_address);
        const boost::asio::ip::tcp::endpoint endpoint(address, port);
        m_socket.connect(endpoint);
    }

    void Disconnect() {
        m_socket.close();
    }

    void CheckSocket() {
        boost::array<std::byte, 1> buf;
        boost::system::error_code error;

        const size_t read = m_socket.read_some(boost::asio::buffer(buf), error);
        
#if 0
        if (error == boost::asio::error::eof) {
             throw std::runtime_error("Connection closed by remote server");
        } else if (error == boost::asio::error::would_block || error == boost::asio::error::try_again) {
            // std::cout << "nothing to read" << std::endl;
        } else {
            // throw std::runtime_error(error.message());
        }
#endif

        if (read != 0) {
            m_run = static_cast<bool>(buf[0]);
            std::cout << "new m_run: " << m_run << std::endl;
        }
    }

private:
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::socket m_socket;
    // std::atomic_bool m_run = false;
    bool m_run = false;
};


auto main() -> int {
    // mosquittoo
    mosquitto_lib_init();
    struct mosquitto* client = mosquitto_new("publisher", true, NULL);
    mosquitto_connect(client, "localhost", 1883, 300);
    
    cv::KalmanFilter KF(4, 2, 0);
    KF.transitionMatrix = (cv::Mat_<float>(4, 4) <<
        1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    cv::Mat_<float> measurement(2, 1);
    measurement.setTo(0);
    KF.statePre.at<float>(0) = 0;
    KF.statePre.at<float>(1) = 0;
    KF.statePre.at<float>(2) = 0;
    KF.statePre.at<float>(3) = 0;
    cv::setIdentity(KF.measurementMatrix);
    cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(KF.errorCovPost, cv::Scalar::all(50));
    
    IPCConnection connection;
    connection.Connect("127.0.0.1", 8000);

    cv::VideoCapture cap("data/video.mp4");
    std::vector<cv::Point> trajectory;
    cv::Mat frame;
    while(cap.read(frame)) {
        if (!connection.ShouldRun()) {  // read socket every socket
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        
        cv::Mat processed(frame);

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // cv::GaussianBlur(gray, gray, { 7, 7 }, 7);

        cv::Mat edged(gray);
        cv::Canny(gray, edged, 60, 180);

        // cv::adaptiveThreshold(gray, gray, 250, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 7, 2);


        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        findContours(edged, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


        for (const auto & contour : contours) {
            const cv::Rect rect = cv::boundingRect(contour);
            cv::rectangle(processed, rect, { 255, 0, 0 }, 1);
        }


        cv::Mat prediction = KF.predict();
        cv::Point predictPt(prediction.at<float>(0), prediction.at<float>(1));
        
        if (!contours.size()) {
            continue;
        }
        const cv::Rect rect = cv::boundingRect(contours[0]);
        const cv::Point center = { (rect.x + rect.width / 2), (rect.y + rect.height / 2) };
        measurement(0) = center.x;
        measurement(1) = center.y;
        
        cv::Mat estimated = KF.correct(measurement);
        cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
        trajectory.emplace_back(statePt);
        for (const auto & point : trajectory) {
            cv::circle(processed, point, 5, { 127, 127, 0 }, 1);
        }
        
        {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("X", center.x, allocator);
            document.AddMember("Y", center.y, allocator);

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
            const std::string str = buffer.GetString();

            std::cout << str << std::endl;
            // mosquittoo
            mosquitto_publish(client, NULL, "coordinates", str.length(), str.c_str(), 0, false);
        }

        imshow("window", processed);
        if (cv::waitKey(30) >= 0) break;
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    // mosquitto (wonderful C API)
    mosquitto_disconnect(client);
    mosquitto_destroy(client);
    mosquitto_lib_cleanup();

    connection.Disconnect();

    return 0;
}
