#define SDL_MAIN_USE_CALLBACKS
#include "render/gl_init.h"
#include "utils/memory.h"
#include "window/window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glad/glad.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  // 元数据
  init_app_meta_data();
  // 分配应用状态
  AppState *state = NEW(AppState);
  // 创建窗口
  if (!create_window(state)) {
    return SDL_APP_FAILURE;
  }
  // 初始化OpenGL渲染
  if (!init_opengl_render(state)) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  if (!appstate) {
    return SDL_APP_FAILURE;
  }
  AppState *state = (AppState *)appstate;
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 交换缓冲区
  SDL_GL_SwapWindow(state->window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (!appstate) {
    return SDL_APP_FAILURE;
  }
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate) {
    AppState *state = (AppState *)appstate;
    SDL_DestroyWindow(state->window);
    FREE(state);
  }
  SDL_Quit();
}
