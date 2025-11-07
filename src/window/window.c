#include "window/window.h"
#include <SDL3/SDL.h>

int init_app_meta_data() {
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "贪吃蛇 - 3d");
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, "1.0.0");
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING,
                             "com.shown.zhang");
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING,
                             "zhangfeiqing");
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
                             "zhangfeiqing");
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");
  return 0;
}

int create_window(AppState *state) {
  // 初始化SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL初始化失败: %s\n",
                 SDL_GetError());
    return 0;
  }

  // 设置OpenGL属性
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  // 设置为核心配置文件
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // 创建窗口
  state->window =
      SDL_CreateWindow("贪吃蛇 - 3d",                           // 窗口标题
                       SCREEN_WIDTH, SCREEN_HEIGHT,             // 宽度、高度
                       SDL_WINDOW_OPENGL                        // 窗口标志（禁用resize）
      );
  if (!state->window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "窗口创建失败: %s",
                 SDL_GetError());
    return 0;
  }

  return 1;
}
