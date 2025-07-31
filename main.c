#include "cv.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <complex.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
constexpr u32 w = 1600, h = 800;

#define gl_error()                                                             \
  for (u32 gl_err; (gl_err = glGetError());)                                   \
  printf("%x@%d\n", gl_err, __LINE__)
static u32 compile(u32 type, const char *src) {
  u32 shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, 0);
  glCompileShader(shader);

  int success;
  char buf[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    puts("compile error");
    puts(src);
    glGetShaderInfoLog(shader, 1024, 0, buf);
    puts(buf);
  }
  gl_error();
  return shader;
}
static u32 link(...) {
  va_list arg;
  va_start(arg);
  u64 n = va_arg(arg, u32);
  u32 shader = glCreateProgram();
  for (u64 i = 0; i < n; ++i)
    glAttachShader(shader, va_arg(arg, u32));
  va_end(arg);

  int success;
  char buf[1024];
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    puts("link error");
    glGetProgramInfoLog(shader, 1024, 0, buf);
    puts(buf);
  }
  gl_error();
  return shader;
}

static u32 shader;
static u32 vao;
static void create_shader() {
  const char pos_vert_src[] = {
#embed "pos.vert"
      , 0};
  const char light_frag_src[] = {
#embed "light.frag"
      , 0};
  gl_error();
  u32 pos_vert = compile(GL_VERTEX_SHADER, pos_vert_src);
  u32 light_frag = compile(GL_FRAGMENT_SHADER, light_frag_src);
  shader = link(2, pos_vert, light_frag);
  gl_error();

  puts("shader create success");
}
typedef struct {
  vec4 pos, color;
} Vert;
void create_vao() {
  u32 vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  constexpr u64 N = 65536;
  glBufferData(GL_ARRAY_BUFFER, N * sizeof(Vert), 0, GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, N * 4, 0, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, 0, 32, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, 0, 32, (void *)16);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  gl_error();
  puts("vao create success");
}
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return SDL_APP_FAILURE;
#define WFLAGS SDL_WINDOW_OPENGL //| SDL_WINDOW_HIGH_PIXEL_DENSITY
  if (!SDL_CreateWindowAndRenderer("test", w, h, WFLAGS, &window, &renderer))
    return SDL_APP_FAILURE;
  SDL_SetWindowRelativeMouseMode(window, 1);
  SDL_GL_CreateContext(window);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  printf("GL Version: %s\n", glGetString(GL_VERSION));
  printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Core Profile: using OpenGL %s\n", glGetString(GL_VERSION));

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_MULTISAMPLE);
  gl_error();
  create_shader();
  create_vao();
  cv_init();
  return SDL_APP_CONTINUE;
}

static vec3 dir;
static f32 yaw, pit, rol;
#define PI_2 1.57079632679489661922
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_MOUSE_MOTION:
#define rspeed 0 //2e-3
    yaw -= event->motion.xrel * rspeed;
    pit += event->motion.yrel * rspeed;
    break;
  case SDL_EVENT_KEY_DOWN:
    if (event->key.repeat)
      break;
    switch (event->key.key) {
    case 'w':
      --dir.z;
      break;
    case 's':
      ++dir.z;
      break;
    case 'a':
      --dir.x;
      break;
    case 'd':
      ++dir.x;
      break;
    case ' ':
      ++dir.y;
      break;
    case 'c':
      --dir.y;
      break;
    case 'q':
      rol += .1;
      break;
    case 'e':
      rol -= .1;
      break;
    }
    break;
  case SDL_EVENT_KEY_UP:
    if (event->key.repeat)
      break;
    switch (event->key.key) {
    case 'w':
      ++dir.z;
      break;
    case 's':
      --dir.z;
      break;
    case 'a':
      ++dir.x;
      break;
    case 'd':
      --dir.x;
      break;
    case ' ':
      --dir.y;
      break;
    case 'c':
      ++dir.y;
      break;
    }
    break;
  }
  return SDL_APP_CONTINUE;
}

static vec4 view = {0, .25, 0, 1};
static const f32 lw = 2.5e-3, speed = 3e-4;
static const vec2 a4 = {.210, .297};
static Vert obj[] = {
    {{-.25, 0, lw, 1}, {0, 0, 0, 1}},
    {{.25, 0, lw, 1}, {0, 0, 0, 1}},
    {{-.25, 0, -lw, 1}, {0, 0, 0, 1}},
    {{.25, 0, -lw, 1}, {0, 0, 0, 1}},
    {{-lw, 0, 0, 1}, {0, 0, 0, 1}},
    {{lw, 0, 0, 1}, {0, 0, 0, 1}},
    {{-lw, 0, -2.1, 1}, {0, 0, 0, 1}},
    {{lw, 0, -2.1, 1}, {0, 0, 0, 1}},
    {{-a4.x / 2, 0, -1, 1}, {1, 1, 1, 1}},
    {{a4.x / 2, 0, -1, 1}, {1, 1, 1, 1}},
    {{-a4.x / 2, a4.y, -1, 1}, {1, 1, 1, 1}},
    {{a4.x / 2, a4.y, -1, 1}, {1, 1, 1, 1}},
};
static const u32 indx[] = {0, 1, 2, 2, 1, 3,  4,  5, 6,
                           6, 5, 7, 8, 9, 10, 10, 9, 11};
#define clk __builtin_readcyclecounter()
#define sin __builtin_elementwise_sin
#define cos __builtin_elementwise_cos
static u8 pixel[w * h];
SDL_AppResult SDL_AppIterate(void *appstate) {
  glClearColor(.5, .5, .5, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static f32 scale = 2000;
  glUseProgram(shader);
  glUniform2f(0, scale / w, scale / h);
  vec4 roty[3] = {{cos(yaw), 0, -sin(yaw)}, {0, 1, 0}, {sin(yaw), 0, cos(yaw)}};
  view += (roty[0] * dir.x + roty[1] * dir.y + roty[2] * dir.z) * speed;
  vec4 rot[4] = {{sin(yaw) * sin(pit) * sin(rol) + cos(yaw) * cos(rol),
                  -cos(pit) * sin(rol),
                  cos(yaw) * sin(pit) * sin(rol) - sin(yaw) * cos(rol), 0},
                 {cos(yaw) * sin(rol) - sin(yaw) * sin(pit) * cos(rol),
                  cos(pit) * cos(rol),
                  -cos(yaw) * sin(pit) * cos(rol) - sin(yaw) * sin(rol), 0},
                 {sin(yaw) * cos(pit), sin(pit), cos(yaw) * cos(pit), 0},
                 {0, 0, 0, 1}};
  glUniform3f(1, view.x, view.y, view.z);
  glUniformMatrix4fv(2, 1, 0, (f32 *)rot);
  glBindVertexArray(vao);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(obj), obj);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indx), indx);
  glDrawElements(GL_TRIANGLES,12 , GL_UNSIGNED_INT, 0);
  SDL_GL_SwapWindow(window);
  glReadPixels(0, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, pixel);
  gl_error();

  cv_pixel(pixel, w, h);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
