#include "cv.h"
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
    return vec2{w / 2.f + t.x, h / 2.f - t.y};
  };
  vec2 t = trans(pos);
  u8 p0 = img.data[(u32)t.y * w + (u32)t.x];
  printf("%d\n", p0);
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
}
