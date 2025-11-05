#include "render/gl_init.h"
#include <glad/glad.h>

int init_opengl_render(AppState *state) {
  // opengl 上下文
  SDL_GLContext gl_context = SDL_GL_CreateContext(state->window);
  if (!gl_context) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL上下文创建失败: %s",
                 SDL_GetError());
    return 0;
  }
  // 使当前线程的OpenGL上下文成为当前上下文
  SDL_GL_MakeCurrent(state->window, gl_context);
  // 启用垂直同步
  SDL_GL_SetSwapInterval(1);

  // 初始化GLAD
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GLAD初始化失败: %s",
                 SDL_GetError());
    return 0;
  }

  glEnable(GL_DEPTH_TEST);

  return 1;
}