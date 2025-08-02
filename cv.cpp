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
    vec4 axis0 = {x, 0, -.9, 1};
    vec2 start = trans(axis0);
    vec2 end = trans(axis);
    constexpr u32 n = 32;
    vec2 step = (end - start) / n;
    for (u64 i = 0; i <= n; ++i) {
      vec2 p0 = start + step * i;
      if (p[idx(p0)] < thr)
        return fied(4, p0, p0 - step);
    }
    return end;
  };
  vec2 b0 = fiba(-.05), b1 = fiba(.05);
  f32 z0 = view.y * scale / b0.y;
  f32 z1 = view.y * scale / b1.y;
  f32 trot = (z1 - z0) * 10;
  f32 t = sqrt(trot * trot + 1);
  f32 crot = 1 / t;
  f32 srot = trot / t;
  vec4 base = {0, a4.y / 2, (z0 + z1) / 2, 1};
  auto page = [&](vec2 p) {
    vec4 pos = {p.x * crot, p.y, p.x * srot};
    return trans(base + pos);
  };

  struct {
    vec2 p;
    vec2 g;
    int v;
  } grid[17][27];
  for (int i = 0; i < 17; ++i)
    for (int j = 0; j < 27; ++j) {
      vec2 p0 = {(i - 8) * .01f, (j - 13) * .01f};
      vec2 tp0 = page(p0);
      u32 id = idx(tp0);
      grid[i][j] = {p0, tp0, p[id] < thr};
    }

  auto fipo = [&](u32 n, vec2 o, vec2 d) {
    iter:
      if (--n) {
        vec2 pos[4] = {o + d, o + vec2{d.x, 0}, o + vec2{0, d.y}, o};
        for (u32 i = 0; i < 4; ++i)
          if (p[idx(page(pos[i]))] < thr) {
            o = pos[i], d /= 2;
            goto iter;
          }
      }
      return o;
  };

  u32 prec = 0, primc = 0;
  vec2 prim[10][64]{};
  f32 py[10], pdy[10];
  for (u32 i = 0; i < 17; ++i) {
    int prev = 0;
    u32 c = 0;
    u32 ij[20];
    f32 y[20];
    for (u32 j = 1; j < 27; ++j) {
      if (prev && !grid[i][j].v)
        ij[c] = j - 1, y[c++] = fied(4, grid[i][j - 1].g, grid[i][j].g).y;
      if (!prev && grid[i][j].v)
        ij[c] = j, y[c++] = fied(4, grid[i][j].g, grid[i][j - 1].g).y;
      prev = grid[i][j].v;
    }
    for (u32 j = 0; j < c; j += 2) {
      f32 dy0 = py[j] - y[j], dy1 = py[j + 1] - y[j + 1];
      if (!prec) {
        prim[0][primc++] = fipo(4, grid[i][ij[j]].p, -.01),
        prim[0][primc++] = fipo(4, grid[i][ij[j + 1]].p, vec2{-.01, 01});
        goto cont;
      }
      if (abs(pdy[j] - dy0) > 20)
        prim[0][primc++] = fipo(4, grid[i][ij[j]].p, vec2{0, -.01});
      if (abs(pdy[j + 1] - dy1) > 20)
        prim[0][primc++] = fipo(4, grid[i][ij[j + 1]].p, vec2{0, .01});
    cont:
      py[j] = y[j];
      py[j + 1] = y[j + 1];
      pdy[j] = dy0;
      pdy[j + 1] = dy1;
    }
    prec = c;
  }

  for (u32 i = 0; i < 1; ++i) {
    puts("");
    for (u32 j = 0; j < primc; ++j) {
      vec2 p0 = prim[i][j];
      printf("%.3f,%.3f\t", p0.x, p0.y);
    }
  }

  d = base.z;
}
extern "C" void cv_pixel(u8 *pixel) {
  static Mat img(h, w, CV_8UC3, pixel);
  flip(img, img, 0);
  res = img.data;
  process(img.data);

  flip(img, img, 0);
}

extern "C" void cv_run() {
  Mat frame;
  bool success = cap.read(frame);
  if (!success || frame.empty())
    ;
}
