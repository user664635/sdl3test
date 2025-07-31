#include "cv.h"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;
extern "C" void cv_init() {
  cap = VideoCapture(2);
  if (!cap.isOpened())
    return;
}
extern "C" void cv_pixel(u8 *pixel, u32 w, u32 h) {
  static f32 scale = 2000;
  static vec4 view = {0, .25, 0, 1};
  Mat image(h, w, CV_8UC1, pixel);
  flip(image, image, 0);
  vec4 pos = {0, 0, -2.1, 1};
  auto trans = [w, h](vec4 p) {
    vec4 tmp = p - view;
    vec2 t = scale * tmp.xy / -tmp.z * .5;
    return Point(w / 2. + t.x, h / 2. - t.y);
  };
  auto cir = [image](Point p) { circle(image, p, 50, Scalar(50), 10); };
  cir(trans(pos));
  cir(trans(0));
  namedWindow("pixel", WINDOW_KEEPRATIO);
  imshow("pixel", image);
  waitKey(1);
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
  namedWindow("test", WINDOW_AUTOSIZE);
  imshow("test", frame);
  waitKey(1);
}
