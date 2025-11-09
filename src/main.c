#define SDL_MAIN_USE_CALLBACKS
#include "core/food.h"
#include "render/gl_init.h"
#include "scene/scene.h"
#include "utils/ini_parser.h"
#include "utils/memory.h"
#include "window/window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glad/glad.h>
#include <stdio.h>

// 从INI文件加载游戏配置
static GameConfig load_game_config() {
  GameConfig config = {.gridWidth = 25,
                       .gridHeight = 25,
                       .gridSize = 10,
                       .initialSnakeLength = 3,
                       .maxFoodCount = 5,
                       .moveInterval = 0.3f};

  // 加载INI配置文件
  ini_file_t *ini = ini_load("assets/config/game.ini");
  if (ini == NULL) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "无法加载game.ini配置文件，使用默认配置");
    return config;
  }

  // 读取网格设置
  config.gridWidth = ini_get_int(ini, "grid", "width", config.gridWidth);
  config.gridHeight = ini_get_int(ini, "grid", "height", config.gridHeight);
  config.gridSize = ini_get_int(ini, "grid", "size", config.gridSize);

  // 读取蛇的设置
  config.initialSnakeLength =
      ini_get_int(ini, "snake", "initial_length", config.initialSnakeLength);

  // 读取食物的设置
  config.maxFoodCount =
      ini_get_int(ini, "food", "max_count", config.maxFoodCount);

  // 读取游戏逻辑设置
  config.moveInterval =
      ini_get_double(ini, "game", "move_interval", config.moveInterval);

  SDL_Log("从game.ini加载游戏配置：网格=%dx%d，格子大小=%d，蛇长=%d，食物数=%"
          "d，移动间隔=%.2f秒",
          config.gridWidth, config.gridHeight, config.gridSize,
          config.initialSnakeLength, config.maxFoodCount, config.moveInterval);

  ini_free(ini);
  return config;
}

// 游戏配置
static GameConfig gameConfig;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  // 加载游戏配置
  gameConfig = load_game_config();

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

  // 初始化游戏场景
  state->scene = NEW(GameScene);
  float white[] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA白色
  if (!init_game_scene(state->scene, gameConfig.gridWidth,
                       gameConfig.gridHeight, gameConfig.gridSize, white)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "初始化游戏场景失败");
    return SDL_APP_FAILURE;
  }

  // 初始化背景特效管理器
  if (!init_background_effect(&state->bgEffect, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "初始化背景特效失败");
    return SDL_APP_FAILURE;
  }

  // 初始化游戏状态
  init_game_state(&state->gameState, &gameConfig);

  // 初始化贪吃蛇
  int startX = gameConfig.gridWidth / 2;
  int startY = gameConfig.gridHeight / 2;
  init_snake(&state->snake, startX, startY, gameConfig.initialSnakeLength);

  // 初始化食物管理器
  init_food_manager(&state->foodManager, gameConfig.maxFoodCount);

  // 生成初始食物
  generate_food(&state->foodManager, gameConfig.gridWidth,
                gameConfig.gridHeight, &state->snake);

  // 记录初始时间
  state->lastFrameTime = SDL_GetTicks();

  // 开始游戏
  start_game(&state->gameState);

  *appstate = state;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  if (!appstate) {
    return SDL_APP_FAILURE;
  }
  AppState *state = (AppState *)appstate;

  // 计算时间增量
  Uint64 currentTime = SDL_GetTicks();
  float deltaTime = (currentTime - state->lastFrameTime) / 1000.0f; // 转换为秒
  state->lastFrameTime = currentTime;

  // 更新游戏状态，检查是否需要移动蛇
  if (update_game_state(&state->gameState, deltaTime)) {
    // 先检查是否吃到食物（在移动前检查当前位置）
    int headX, headY;
    get_snake_head(&state->snake, &headX, &headY);
    Food *food = check_food_at_position(&state->foodManager, headX, headY);
    bool shouldGrow = (food != NULL);

    // 移动蛇，根据是否吃到食物决定是否增长
    if (!move_snake(&state->snake, state->gameState.currentDirection,
                    state->gameState.config.gridWidth,
                    state->gameState.config.gridHeight, shouldGrow)) {
      // 移动失败，游戏结束
      game_over(&state->gameState);
    }

    // 如果吃到食物，处理食物逻辑
    if (food != NULL) {
      // 吃到食物，增加分数
      int foodValue = remove_food(&state->foodManager, food);
      state->gameState.score += foodValue;

      // 生成新食物
      generate_food(&state->foodManager, state->gameState.config.gridWidth,
                    state->gameState.config.gridHeight, &state->snake);

      SDL_Log("吃到食物！当前得分: %d", state->gameState.score);
    }

    // 检查是否需要生成更多食物
    if (!is_food_max_reached(&state->foodManager)) {
      generate_food(&state->foodManager, state->gameState.config.gridWidth,
                    state->gameState.config.gridHeight, &state->snake);
    }
  }

  // 更新背景特效
  update_background_effect(&state->bgEffect, deltaTime);

  // 清除屏幕并渲染动态背景特效
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 渲染背景特效（禁用深度测试，确保背景在最底层）
  glDisable(GL_DEPTH_TEST);
  render_background_effect(&state->bgEffect);
  glEnable(GL_DEPTH_TEST);

  // 渲染游戏场景（网格）
  render_game_scene(state->scene);

  // 渲染贪吃蛇（白色）
  float snakeColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA白色
  KNode *node;
  knode_for_each(node, &state->snake.head) {
    SnakeSegment *segment = container_of(node, SnakeSegment, node);
    float x = segment->x * state->gameState.config.gridSize +
              state->gameState.config.gridSize / 2.0f;
    float y = segment->y * state->gameState.config.gridSize +
              state->gameState.config.gridSize / 2.0f;
    render_rectangle(&state->scene->gridRenderer, x, y,
                     state->gameState.config.gridSize * 0.8f,
                     state->gameState.config.gridSize * 0.8f, snakeColor);
  }

  // 渲染食物（根据元素类型和子元素类型设置边框和填充颜色）
  knode_for_each(node, &state->foodManager.head) {
    Food *food = container_of(node, Food, node);
    float x = food->x * state->gameState.config.gridSize +
              state->gameState.config.gridSize / 2.0f;
    float y = food->y * state->gameState.config.gridSize +
              state->gameState.config.gridSize / 2.0f;

    // 获取元素颜色（边框颜色）
    FoodColor borderColor = get_food_color(food);
    float borderColorArray[] = {borderColor.r, borderColor.g, borderColor.b,
                                borderColor.a};

    // 获取子元素颜色（填充颜色）
    FoodColor fillColor = get_detailed_food_color(
        food->elementType, food->elementLevel, food->subElementType);
    float fillColorArray[] = {fillColor.r, fillColor.g, fillColor.b,
                              fillColor.a};

    // 先渲染内部填充（较小的矩形）
    render_rectangle(&state->scene->gridRenderer, x, y,
                     state->gameState.config.gridSize * 0.6f,
                     state->gameState.config.gridSize * 0.6f, fillColorArray);

    // 再渲染边框（较大的矩形，但使用线框模式或半透明）
    // 使用线框模式渲染边框，确保边框可见
    float borderWidth = state->gameState.config.gridSize * 0.1f;

    // 渲染上边框
    render_rectangle(&state->scene->gridRenderer, x,
                     y + state->gameState.config.gridSize * 0.3f,
                     state->gameState.config.gridSize * 0.8f, borderWidth,
                     borderColorArray);

    // 渲染下边框
    render_rectangle(&state->scene->gridRenderer, x,
                     y - state->gameState.config.gridSize * 0.3f,
                     state->gameState.config.gridSize * 0.8f, borderWidth,
                     borderColorArray);

    // 渲染左边框
    render_rectangle(&state->scene->gridRenderer,
                     x - state->gameState.config.gridSize * 0.3f, y,
                     borderWidth, state->gameState.config.gridSize * 0.6f,
                     borderColorArray);

    // 渲染右边框
    render_rectangle(&state->scene->gridRenderer,
                     x + state->gameState.config.gridSize * 0.3f, y,
                     borderWidth, state->gameState.config.gridSize * 0.6f,
                     borderColorArray);
  }

  // 交换缓冲区
  SDL_GL_SwapWindow(state->window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (!appstate) {
    return SDL_APP_FAILURE;
  }
  AppState *state = (AppState *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  // 处理键盘输入事件
  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.scancode) {
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_W:
      change_direction(&state->gameState, DIRECTION_UP);
      break;
    case SDL_SCANCODE_DOWN:
    case SDL_SCANCODE_S:
      change_direction(&state->gameState, DIRECTION_DOWN);
      break;
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_A:
      change_direction(&state->gameState, DIRECTION_LEFT);
      break;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_D:
      change_direction(&state->gameState, DIRECTION_RIGHT);
      break;
    case SDL_SCANCODE_SPACE:
      if (state->gameState.currentState == GAME_STATE_PLAYING) {
        pause_game(&state->gameState);
      } else if (state->gameState.currentState == GAME_STATE_PAUSED) {
        resume_game(&state->gameState);
      }
      break;
    case SDL_SCANCODE_R:
      if (state->gameState.currentState == GAME_STATE_GAME_OVER) {
        reset_game(&state->gameState);
        // 重新初始化蛇和食物
        cleanup_snake(&state->snake);
        cleanup_food_manager(&state->foodManager);

        int startX = state->gameState.config.gridWidth / 2;
        int startY = state->gameState.config.gridHeight / 2;
        init_snake(&state->snake, startX, startY,
                   state->gameState.config.initialSnakeLength);
        init_food_manager(&state->foodManager,
                          state->gameState.config.maxFoodCount);
        generate_food(&state->foodManager, state->gameState.config.gridWidth,
                      state->gameState.config.gridHeight, &state->snake);
        start_game(&state->gameState);
      }
      break;
    case SDL_SCANCODE_ESCAPE:
      return SDL_APP_SUCCESS;
    default:
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate) {
    AppState *state = (AppState *)appstate;

    // 清理游戏场景
    if (state->scene) {
      cleanup_game_scene(state->scene);
      FREE(state->scene);
    }

    // 清理背景特效管理器
    cleanup_background_effect(&state->bgEffect);

    // 清理贪吃蛇和食物管理器
    cleanup_snake(&state->snake);
    cleanup_food_manager(&state->foodManager);

    SDL_DestroyWindow(state->window);
    FREE(state);
  }
  SDL_Quit();
}
