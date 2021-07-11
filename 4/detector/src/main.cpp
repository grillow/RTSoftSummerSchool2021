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
    
    cv::VideoCapture cap("data/video.mp4");
    std::vector<cv::Point> trajectory;
    cv::Mat frame;
    while(cap.read(frame)) {
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

    return 0;
}
