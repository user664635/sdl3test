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
extern "C" void cv_pixel(u8 *pixel, u32 w, u32 h) {
  Mat image(h, w, CV_8UC3, pixel);
  flip(image, image, 0);
  imshow("pixel", image);
  waitKey(1);
}

extern "C" void cv_run() {
  Mat frame;
  if (esc)
    return;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
  namedWindow("test", WINDOW_AUTOSIZE);
  imshow("test", frame);
  waitKey(1);
}
