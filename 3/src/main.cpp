#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <rapidjson/stringbuffer.h>
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
    
    cv::VideoCapture cap("data/video.mkv");
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

        {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("x1", 11337, allocator);
            document.AddMember("y1", 21337, allocator);
            document.AddMember("x2", 31337, allocator);
            document.AddMember("y2", 41337, allocator);

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
            const std::string str = buffer.GetString();

            // mosquittoo
            mosquitto_publish(client, NULL, "coordinates", str.length(), str.c_str(), 0, false);
        }

        imshow("window", processed);
        if(cv::waitKey(30) >= 0) break;
    }
    
    // mosquitto (wonderful C API)
    mosquitto_disconnect(client);
    mosquitto_destroy(client);
    mosquitto_lib_cleanup();

    return 0;
}
