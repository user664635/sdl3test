#include <glad/gl.h>
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
  gl_error();

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
static u32 vao, vbo;
static void create_shader(void) {
  const char pos_vert_src[] = {
#embed "pos.vert"
      , 0};
  const char light_frag_src[] = {
#embed "light.frag"
      , 0};
  const f32 trans[4][4] = {
      {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
  u32 pos_vert = compile(GL_VERTEX_SHADER, pos_vert_src);
  u32 light_frag = compile(GL_FRAGMENT_SHADER, light_frag_src);
  shader = link(2, pos_vert, light_frag);
  //glUniformMatrix4fv(1, 1, 0, (f32 *)trans);
  gl_error();

  puts("shader create success");
}
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return SDL_APP_FAILURE;
  if (!SDL_CreateWindowAndRenderer("test", w, h, SDL_WINDOW_OPENGL, &window,
                                   &renderer))
    return SDL_APP_FAILURE;
  SDL_GL_CreateContext(window);
  gladLoadGL(SDL_GL_GetProcAddress);
  create_shader();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

#define clk __builtin_readcyclecounter()
SDL_AppResult SDL_AppIterate(void *appstate) {
  glClearColor(.5, .5, .5, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
