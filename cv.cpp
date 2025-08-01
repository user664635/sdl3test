#include "cv.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;

VideoCapture cap;
extern "C" void cv_init() {
  cap = VideoCapture(2, CAP_V4L2);
  if (!cap.isOpened())
    return;
  cap.set(CAP_PROP_FRAME_WIDTH, 1920);
  cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
  cap.set(CAP_PROP_FPS, 60);
  cap.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
  //  Mat frame;
  //  bool success = cap.read(frame);
  //  if (!success || frame.empty())
  //    ;
}
auto mid(auto a, auto b) { return (a + b) / 2; }
auto dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }
auto length(vec2 a) { return __builtin_elementwise_sqrt(dot(a, a)); }
extern f32 scale;
extern vec4 view;
u32 cam_w, cam_h;
#define w cam_w
#define h cam_h
extern "C" vec2 cv_pixel(u8 *pixel, u8 **result) {
  // static Mat img(h, w, CV_8UC3, pixel);
  // flip(img, img, 0);

  static Mat img;
  bool success = cap.read(img);
  if (!success || img.empty())
    puts("err");
  w = img.cols;
  h = img.rows;
  printf("%d,%d\t", w, h);

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
  auto light = [&](u64 i) {
    p[i - w * 3 + 1] = -1;
    p[i - w * 3 + 2] = 0;
    p[i + 1] = 0;
    p[i + 2] = -1;
    p[i + w * 3 + 1] = -1;
    p[i + w * 3 + 2] = 0;
  };
  auto fied = [&](u8 n, vec2 p0, vec2 p1) {
    while (--n) {
      vec2 m = mid(p0, p1);
      if (p[idx(m)] < thr)
        p0 = m;
      else
        p1 = m;
    }
    light(idx(p0));
    return p0;
  };
  auto fipo = [&](u8 n, vec2 m, vec2 d) {
    vec2 rd = {d.y, -d.x};
    while (--n) {
      vec2 t = fied(4, m, m + d);
      bool dir = p[idx(t + rd / 32)] > thr;
      vec2 t1 = fied(4, t, t + (dir ? rd : -rd));
      m = (t + t1) / 2;
    }
    return m;
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
  light(idx(b));
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
      grid[i][j] = {p0, trans(p0), p[idx(trans(p0))] < thr};
    }

  // u32 prec = 0;
  // vec2 prey0 = 0;
  // vec2 predy0 = 0;
  // vec2 prey1 = 0;
  // vec2 pos[4];
  // for (u32 i = 0; i < 17; ++i) {
  //   int prev = 0;
  //   vec2 y0 = 0, y1 = 0;
  //   u32 c = 0;
  //   for (u32 j = 0; j < 26; ++j) {
  //     if (!prev && grid[i][j].v)
  //       y0 = fied(4, grid[i][j].g, grid[i][j - 1].g), ++c;
  //     if (prev && !grid[i][j].v)
  //       y1 = fied(4, grid[i][j - 1].g, grid[i][j].g), ++c;
  //     prev = grid[i][j].v;
  //   }
  //   if (c) {
  //     if (!prec)
  //       pos[0] = fipo(4, (y0 + y1) / 2, vec2{-32, 0});
  //     vec2 dy0 = y0 - prey0;
  //     if (predy0.y > dy0.y) {
  //       pos[1] = fipo(4, (y0 + prey0) / 2, vec2{0, 16});
  //       break;
  //     }

  //    prey0 = y0;
  //    predy0 = dy0;
  //    prey1 = y1;
  //  }
  //  prec = c;
  //}
  // f32 l = length(pos[1] - pos[0]) * base.z / scale;

  flip(img, img, 0);
  return {base.z, 0};
}
