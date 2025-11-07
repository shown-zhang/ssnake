#pragma once
#include "render/coordinate.h"
#include "render/square_renderer.h"

/**
 * @brief 游戏场景结构体
 */
typedef struct {
  CoordinateSystem coord;      // 坐标系统
  SquareRenderer gridRenderer; // 网格渲染器
  int gridWidth;               // 网格宽度（格子数）
  int gridHeight;              // 网格高度（格子数）
  float gridSize;              // 每个格子的大小
  float gridColor[4];          // 网格颜色
} GameScene;

/**
 * @brief 初始化游戏场景
 *
 * @param scene 场景指针
 * @param gridWidth 网格宽度（格子数）
 * @param gridHeight 网格高度（格子数）
 * @param gridSize 每个格子的大小
 * @param gridColor 网格颜色（RGBA）
 * @return int 成功返回1，失败返回0
 */
int init_game_scene(GameScene *scene, int gridWidth, int gridHeight,
                    float gridSize, float gridColor[4]);

/**
 * @brief 渲染游戏场景（包括网格）
 *
 * @param scene 场景指针
 */
void render_game_scene(GameScene *scene);

/**
 * @brief 清理游戏场景资源
 *
 * @param scene 场景指针
 */
void cleanup_game_scene(GameScene *scene);

/**
 * @brief 更新游戏场景以适应新的窗口尺寸
 *
 * @param scene 场景指针
 * @param screenWidth 新的屏幕宽度
 * @param screenHeight 新的屏幕高度
 */
void update_game_scene_size(GameScene *scene, int screenWidth,
                            int screenHeight);