#include "scene/scene.h"
#include "window/window.h"
#include <SDL3/SDL.h>

// 网格线宽度（相对于格子大小的比例）
#define GRID_LINE_WIDTH 0.05f

int init_game_scene(GameScene *scene, int gridWidth, int gridHeight,
                    float gridSize, float gridColor[4]) {
  // 设置场景参数
  scene->gridWidth = gridWidth;
  scene->gridHeight = gridHeight;
  scene->gridSize = gridSize;

  // 复制网格颜色
  for (int i = 0; i < 4; i++) {
    scene->gridColor[i] = gridColor[i];
  }

  // 初始化坐标系统
  // 游戏世界坐标范围：从0到网格尺寸
  float worldWidth = gridWidth * gridSize;
  float worldHeight = gridHeight * gridSize;
  scene->coord =
      init_coordinate_system(0, worldWidth, 0, worldHeight, SCREEN_WIDTH, SCREEN_HEIGHT);

  // 初始化网格渲染器
  if (!init_square_renderer(&scene->gridRenderer, &scene->coord,
                            "assets/shaders/square.vert",
                            "assets/shaders/square.frag")) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "初始化网格渲染器失败");
    return 0;
  }

  return 1;
}

void render_game_scene(GameScene *scene) {
  // 渲染游戏区域包围线
  float borderWidth =
      scene->gridSize * GRID_LINE_WIDTH * 2.0f; // 包围线比网格线稍粗
  float worldWidth = scene->gridWidth * scene->gridSize;
  float worldHeight = scene->gridHeight * scene->gridSize;

  // 渲染上边界
  render_rectangle(&scene->gridRenderer, worldWidth / 2.0f, 0.0f, worldWidth,
                   borderWidth, scene->gridColor);

  // 渲染下边界
  render_rectangle(&scene->gridRenderer, worldWidth / 2.0f, worldHeight,
                   worldWidth, borderWidth, scene->gridColor);

  // 渲染左边界
  render_rectangle(&scene->gridRenderer, 0.0f, worldHeight / 2.0f, borderWidth,
                   worldHeight, scene->gridColor);

  // 渲染右边界
  render_rectangle(&scene->gridRenderer, worldWidth, worldHeight / 2.0f,
                   borderWidth, worldHeight, scene->gridColor);
}

void cleanup_game_scene(GameScene *scene) {
  cleanup_square_renderer(&scene->gridRenderer);
}