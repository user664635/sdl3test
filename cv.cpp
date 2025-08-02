#include "cv.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;

u32 cam_w = w, cam_h = h;
#define w cam_w
#define h cam_h
VideoCapture cap;
extern "C" void cv_init() {
  cap = VideoCapture(2);
  if (!cap.isOpened())
    return;
}
auto mid(auto a, auto b) { return (a + b) / 2; }
auto dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }
auto length(vec2 a) { return __builtin_elementwise_sqrt(dot(a, a)); }
extern f32 scale;
extern vec4 view;
auto trans(vec4 p) {
  vec4 tmp = p - view;
  return scale * tmp.xy / -tmp.z;
};
auto idx(vec2 p) {
  return ((u64)((h - p.y) * .5) * w + (u64)((w + p.x) * .5)) * 3;
};
u8 *res;
f32 d, x;
void process(u8 *p) {
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
      vec2 tp0 = trans(p0);
      u32 id = idx(tp0);
      grid[i][j] = {p0, tp0, p[id] < thr};
      light(id);
    }

  u32 prec = 0;
  vec2 prim[10][10];
  for (u32 i = 0; i < 17; ++i) {
    int prev = 0;
    u32 c = 0;
    u32 ij[20];
    f32 y[20];
    for (u32 j = 0; j < 26; ++j) {
      if (prev && !grid[i][j].v)
        ij[c] = j, y[c++] = fied(4, grid[i][j - 1].g, grid[i][j].g).y;
      if (!prev && grid[i][j].v)
        ij[c] = j, y[c++] = fied(4, grid[i][j].g, grid[i][j - 1].g).y;
      prev = grid[i][j].v;
    }
    for (u32 j = 0; j < c; j += 2) {
      if (!prec)
        prim[0][0] = grid[i][j].g, prim[0][1] = grid[i][j + 1].g;
    }
    prec = c;
  }

  vec2 p0 = prim[0][0];
  vec2 p1 = prim[0][1];
  printf("%f,%f %f,%f\t", p0.x, p0.y, p1.x, p1.y);
  d = base.z;
}
extern "C" void cv_pixel(u8 *pixel) {
  static Mat img(h, w, CV_8UC3, pixel);
  flip(img, img, 0);
  res = img.data;
  //process(img.data);

  flip(img, img, 0);
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    ;
}
