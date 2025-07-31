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
extern "C" vec2 cv_pixel(u8 *pixel, u8 **result) {
  static Mat img(h, w, CV_8UC3, pixel);
  flip(img, img, 0);
  auto trans = [](vec4 p) {
    vec4 tmp = p - view;
    return scale * tmp.xy / -tmp.z;
  };
  auto idx = [&](vec2 p) {
    return ((u64)((h - p.y) * .5) * w + (u64)((w + p.x) * .5)) * 3;
  };
  u8 *p = img.data;
  *result = p;
  constexpr u8 thr = 50;
  auto fied = [&](u8 n, vec2 p0, vec2 p1) {
    for (u8 i = n; --i;) {
      vec2 m = mid(p0, p1);
      if (p[idx(m)] < thr)
        p0 = m;
      else
        p1 = m;
    }
    p[idx(p0) + 2] = 255;
    return p0;
  };

  auto fiba = [&](f32 x) {
    vec4 axis = {x, 0, -2.1, 1};
    vec4 axis0 = {x, 0, -1, 1};
    vec2 start = trans(axis0);
    vec2 end = trans(axis);
    constexpr u32 n = 32;
    vec2 step = (end - start) / n;
    for (u64 i = 0; i < n; ++i) {
      vec2 p0 = start + step * i;
      if (p[idx(p0)] < thr)
        return fied(4, p0, p0 - step);
    }
    return end;
  };
  vec2 b = (fiba(-.05) + fiba(.05)) / 2;
  p[idx(b) + 1] = 255;
  vec4 base = {0, 0, view.y * scale / b.y, 1};
  vec4 a40 = base + vec4{-a4.x / 2 + .025f, .0235f};
  struct {
    vec4 o;
    vec2 g;
    int v;
  } grid[17][26];
  for (u32 i = 0; i < 17; ++i)
    for (u32 j = 0; j < 26; ++j) {
      vec4 p0 = a40 + vec4{i * .01f, j * .01f};
      grid[i][j] = {p0, trans(p0), -(p[idx(trans(p0)) > thr])};
    }
  for (u32 i = 0; i < 17; ++i)
    for (u32 j = 0; j < 26; ++j) {
      p[idx(grid[i][j].g) + 1] = grid[i][j].v;
    }

  flip(img, img, 0);
  return base.z;
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    puts("empty");
}
