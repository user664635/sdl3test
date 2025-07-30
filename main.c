#define SDL_MAIN_USE_CALLBACKS 1
#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <complex.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
constexpr int w = 1600, h = 800;

typedef float f32;
typedef complex float c32;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return SDL_APP_FAILURE;
  if (!SDL_CreateWindowAndRenderer("test", w, h, SDL_WINDOW_OPENGL, &window,
                                   &renderer))
    return SDL_APP_FAILURE;
  SDL_GL_CreateContext(window);
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
  glClearColor(0, 1, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
