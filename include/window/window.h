
#pragma once
#include "core/food.h"
#include "core/snake.h"
#include "core/state.h"
#include "scene/scene.h"
#include <SDL3/SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct {
  SDL_Window *window;
  GameScene *scene;        // 游戏场景
  GameStateData gameState; // 游戏状态
  Snake snake;             // 贪吃蛇
  FoodManager foodManager; // 食物管理器
  Uint64 lastFrameTime;    // 上一帧时间
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
