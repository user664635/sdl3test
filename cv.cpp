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
  Mat img(h, w, CV_8UC1, pixel);
  flip(img, img, 0);
  vec4 pos = {0, 0, -2.1, 1};
  auto trans = [w, h](vec4 p) {
    vec4 tmp = p - view;
    vec2 t = scale * tmp.xy / -tmp.z * .5;
    return Point(w / 2. + t.x, h / 2. - t.y);
  };
  u8 p0 = img.at<u8>(trans(pos));
  printf("%d\n", p0);

  auto cir = [img](Point p) { circle(img, p, 50, Scalar(200), 10); };
  cir(trans(pos));
  cir(trans(0));
  namedWindow("pixel", WINDOW_KEEPRATIO);
  imshow("pixel", img);
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
