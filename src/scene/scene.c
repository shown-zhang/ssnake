#include "scene/scene.h"
#include <SDL3/SDL.h>

// 网格线宽度（相对于格子大小的比例）
#define GRID_LINE_WIDTH 0.05f

// 前向声明静态函数
static void render_grid_lines(GameScene *scene);

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
      init_coordinate_system(0, worldWidth, 0, worldHeight, 640, 480);

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
  // 渲染网格线
  render_grid_lines(scene);
}

void cleanup_game_scene(GameScene *scene) {
    cleanup_square_renderer(&scene->gridRenderer);
}

/**
 * @brief 更新游戏场景以适应新的窗口尺寸
 * 
 * @param scene 场景指针
 * @param screenWidth 新的屏幕宽度
 * @param screenHeight 新的屏幕高度
 */
void update_game_scene_size(GameScene *scene, int screenWidth, int screenHeight) {
    // 更新坐标系统的屏幕尺寸
    update_coordinate_screen_size(&scene->coord, screenWidth, screenHeight);
}

/**
 * @brief 渲染网格线
 *
 * @param scene 场景指针
 */
static void render_grid_lines(GameScene *scene) {
  float lineWidth = scene->gridSize * GRID_LINE_WIDTH;

  // 渲染水平网格线
  for (int y = 0; y <= scene->gridHeight; y++) {
    // 计算线的世界坐标
    float lineY = y * scene->gridSize;

    // 渲染一条从左边到右边的水平线
    // 线的中心在网格线上，宽度为lineWidth，长度为整个网格宽度
    float lineLength = scene->gridWidth * scene->gridSize;
    float lineCenterX = lineLength / 2.0f;

    render_rectangle(&scene->gridRenderer, lineCenterX, lineY, lineLength,
                     lineWidth, scene->gridColor);
  }

  // 渲染垂直网格线
  for (int x = 0; x <= scene->gridWidth; x++) {
    // 计算线的世界坐标
    float lineX = x * scene->gridSize;

    // 渲染一条从上到下的垂直线
    // 线的中心在网格线上，宽度为lineWidth，长度为整个网格高度
    float lineLength = scene->gridHeight * scene->gridSize;
    float lineCenterY = lineLength / 2.0f;

    render_rectangle(&scene->gridRenderer, lineX, lineCenterY, lineWidth,
                     lineLength, scene->gridColor);
  }
}