
#pragma once
#include <SDL3/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct {
  SDL_Window *window;
} AppState;

/**
 * @brief 初始化应用元数据
 *
 * @return int
 */
int init_app_meta_data();

/**
 * @brief 初始化OpenGL渲染
 *
 * @param appstate 应用状态指针
 * @return int 初始化成功返回1，失败返回0
 */
int create_window(AppState *state);
