#if 0
#include <iostream>


auto main() -> int {
    
    return 0;
}
#endif

#include "opencv2/opencv.hpp"
using namespace cv;
int main(int, char**) {
    VideoCapture cap(0); // Открываем камеру или файл
    if(!cap.isOpened())
        // check if we succeeded
        return -1;
    Mat edges;
    namedWindow("edges",1);
    while(1) {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        // Перевод в градации серого
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5); // Размытие
        Canny(edges, edges, 0, 30, 3);
        // Выделение границ
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
// Пример из http://docs.opencv.org/3.0-beta/modules/videoio/doc/reading_and_writing_video.html
