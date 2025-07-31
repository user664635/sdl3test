#include "def.h"
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;
bool esc;
extern "C" void cv_init() {
  cap = VideoCapture(2);
  if (!cap.isOpened())
    return;
}
extern "C" void cv_pixel(u8 *pixel) {}
extern "C" void cv_run() {
  Mat frame;
  if (esc)
    return;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
  namedWindow("test", WINDOW_AUTOSIZE);
  imshow("test", frame);
  if (cv::waitKey(1) == '\xb')
    esc = 1;
}
