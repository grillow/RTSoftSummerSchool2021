#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>


auto main() -> int {
    cv::VideoCapture cap("data/video.mkv");
    cv::Mat frame;
    while(cap.read(frame)) {
        cv::Mat processed(frame);

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(gray, gray, { 7, 7 }, 7);

        cv::Mat edged;
        cv::Canny(gray, edged, 60, 180);


        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        findContours(edged, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


        for (const auto & contour : contours) {
            const cv::Rect rect = cv::boundingRect(contour);
            cv::rectangle(processed, rect, { 255, 0, 0 }, 1);
        }

        imshow("window", processed);
        if(cv::waitKey(30) >= 0) break;
    }

    return 0;
}
