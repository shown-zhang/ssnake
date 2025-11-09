#include "window/window.h"
#include "utils/ini_parser.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

int init_app_meta_data() {
  // 加载INI配置文件
  ini_file_t *ini = ini_load("assets/config/windows.ini");
  if (!ini) {
    // 如果无法加载INI文件，使用默认值
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING,
                               "贪吃蛇 - 3D");
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

  // 从INI文件读取元数据设置
  const char *app_name =
      ini_get_string(ini, "metadata", "app_name", "贪吃蛇 - 3D");
  const char *app_version =
      ini_get_string(ini, "metadata", "app_version", "1.0.0");
  const char *app_identifier =
      ini_get_string(ini, "metadata", "app_identifier", "com.shown.zhang");
  const char *app_creator =
      ini_get_string(ini, "metadata", "app_creator", "zhangfeiqing");
  const char *app_copyright =
      ini_get_string(ini, "metadata", "app_copyright", "zhangfeiqing");
  const char *app_type = ini_get_string(ini, "metadata", "app_type", "game");

  // 设置应用程序元数据
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, app_name);
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, app_version);
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING,
                             app_identifier);
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, app_creator);
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
                             app_copyright);
  SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, app_type);

  // 释放INI配置
  ini_free(ini);
  return 0;
}

int create_window(AppState *state) {
  // 加载INI配置文件
  ini_file_t *ini = ini_load("assets/config/windows.ini");
  if (!ini) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "无法加载窗口配置文件，使用默认设置");
  }

  // 从INI文件读取窗口设置
  int window_width =
      ini ? ini_get_int(ini, "window", "width", SCREEN_WIDTH) : SCREEN_WIDTH;
  int window_height =
      ini ? ini_get_int(ini, "window", "height", SCREEN_HEIGHT) : SCREEN_HEIGHT;
  bool fullscreen =
      ini ? ini_get_bool(ini, "window", "fullscreen", false) : false;
  bool resizable =
      ini ? ini_get_bool(ini, "window", "resizable", false) : false;
  bool vsync = ini ? ini_get_bool(ini, "window", "vsync", true) : true;

  // 从INI文件读取OpenGL设置
  int gl_major = ini ? ini_get_int(ini, "opengl", "major_version", 3) : 3;
  int gl_minor = ini ? ini_get_int(ini, "opengl", "minor_version", 3) : 3;
  const char *gl_profile =
      ini ? ini_get_string(ini, "opengl", "profile", "core") : "core";

  // 从INI文件读取应用程序标题
  const char *window_title =
      ini ? ini_get_string(ini, "metadata", "app_name", "贪吃蛇 - 3D")
          : "贪吃蛇 - 3D";

  // 初始化SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL初始化失败: %s\n",
                 SDL_GetError());
    if (ini)
      ini_free(ini);
    return 0;
  }

  // 设置OpenGL属性
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);

  // 设置OpenGL配置文件
  int profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
  if (strcmp(gl_profile, "compatibility") == 0) {
    profile_mask = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
  } else if (strcmp(gl_profile, "es") == 0) {
    profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile_mask);

  // 设置垂直同步
  SDL_GL_SetSwapInterval(vsync ? 1 : 0);

  // 设置窗口标志
  Uint32 window_flags = SDL_WINDOW_OPENGL;
  if (fullscreen) {
    window_flags |= SDL_WINDOW_FULLSCREEN;
  }
  if (resizable) {
    window_flags |= SDL_WINDOW_RESIZABLE;
  }

  // 创建窗口
  state->window =
      SDL_CreateWindow(window_title, window_width, window_height, window_flags);
  if (!state->window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "窗口创建失败: %s",
                 SDL_GetError());
    if (ini)
      ini_free(ini);
    return 0;
  }

  // 记录配置信息
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "窗口创建成功: %dx%d, 全屏: %s, 可调整大小: %s, VSync: %s",
              window_width, window_height, fullscreen ? "是" : "否",
              resizable ? "是" : "否", vsync ? "是" : "否");

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "OpenGL版本: %d.%d, 配置文件: %s",
              gl_major, gl_minor, gl_profile);

  // 释放INI配置
  if (ini)
    ini_free(ini);
  return 1;
}
