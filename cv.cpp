#include "cv.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;

VideoCapture cap;
extern "C" void cv_init() {
  cap = VideoCapture(2);
  if (!cap.isOpened())
    return;
}
auto mid(auto a, auto b) { return (a + b) / 2; }
extern f32 scale;
extern vec4 view;
extern "C" vec2 cv_pixel(u8 *pixel, u32 w, u32 h) {
  Mat img(h, w, CV_8UC1, pixel);
  flip(img, img, 0);
  auto trans = [](vec4 p) {
    vec4 tmp = p - view;
    return scale * tmp.xy / -tmp.z;
  };
  auto idx = [w, h](vec2 p) {
    return (u64)((h - p.y) * .5) * w + (u64)((w + p.x) * .5);
  };
  u8 *p = img.data;
  constexpr u8 thr = 50;
  auto fied = [p, idx](vec2 p0, vec2 p1) {
    for (u8 i = 10; --i;) {
      vec2 m = mid(p0, p1);
      if (p[idx(m)] > thr)
        p0 = m;
      else
        p1 = m;
    }
    return mid(p0, p1);
  };

  vec4 axis = {-.05, 0, -2.1, 1};
  vec4 axis0 = {-.05, 0, -1, 1};
  vec2 start = trans(axis0);
  vec2 end = trans(axis);
  constexpr u32 n = 128;
  vec2 step = (end - start) / n;
  vec2 b0;
  for (u64 i = 0; i < n; ++i) {
    vec2 p0 = start + step * i;
    if (p[idx(p0)] < thr) {
      b0 = fied(p0, p0 - step);
      break;
    }
  }
  return view.y * scale / b0.y;
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
}
