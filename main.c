#include <GLES3/gl32.h>
#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <complex.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
constexpr int w = 1600, h = 800;

typedef float f32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef f32 [[clang::matrix_type(4, 4)]] mat4;
typedef f32 [[clang::ext_vector_type(2)]] vec2;
typedef f32 [[clang::ext_vector_type(3)]] vec3;
typedef f32 [[clang::ext_vector_type(4)]] vec4;

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
  if (!SDL_CreateWindowAndRenderer("test", w, h, SDL_WINDOW_OPENGL, &window,
                                   &renderer))
    return SDL_APP_FAILURE;
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
  return SDL_APP_CONTINUE;
}

static vec4 dir;
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
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

static vec4 trans[4] = {
    {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, -1, 0, 1}};
static const f32 lw = 2.5e-3, speed = 1e-3;
// static const vec2 a4 = {297, 210};
static Vert obj[] = {
    {{-.25, 0, lw, 1}, {0, 0, 0, 1}},  {{.25, 0, lw, 1}, {0, 0, 0, 1}},
    {{-.25, 0, -lw, 1}, {0, 0, 0, 1}}, {{.25, 0, -lw, 1}, {0, 0, 0, 1}},
    {{-lw, 0, 0, 1}, {0, 0, 0, 1}},    {{lw, 0, 0, 1}, {0, 0, 0, 1}},
    {{-lw, 0, -2.1, 1}, {0, 0, 0, 1}}, {{lw, 0, -2.1, 1}, {0, 0, 0, 1}},
    {{-.5, 0, -1, 1}, {1, 1, 1, 1}},   {{.5, 0, -1, 1}, {1, 1, 1, 1}},
    {{-.5, 2, -1, 1}, {1, 1, 1, 1}},   {{.5, 2, -1, 1}, {1, 1, 1, 1}},
};
static const u32 indx[] = {0, 1, 2, 2, 1, 3,  4,  5, 6,
                           6, 5, 7, 8, 9, 10, 10, 9, 11};
#define clk __builtin_readcyclecounter()
SDL_AppResult SDL_AppIterate(void *appstate) {
  glClearColor(.5, .5, .5, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static f32 scale = 1000;
  glUseProgram(shader);
  glUniform2f(0, scale / w, scale / h);
  trans[3] -= dir * speed;
  glUniformMatrix4fv(1, 1, 0, (f32 *)trans);
  glBindVertexArray(vao);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(obj), obj);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indx), indx);
  glDrawElements(GL_TRIANGLES, sizeof(indx) / 4, GL_UNSIGNED_INT, 0);
  gl_error();
  SDL_GL_SwapWindow(window);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
