#pragma once
#include <SDL3/SDL.h>

/**
 * @brief 坐标转换结构体
 */
typedef struct {
  float minX;       // 游戏世界最小X坐标
  float maxX;       // 游戏世界最大X坐标
  float minY;       // 游戏世界最小Y坐标
  float maxY;       // 游戏世界最大Y坐标
  int screenWidth;  // 屏幕宽度
  int screenHeight; // 屏幕高度
} CoordinateSystem;

/**
 * @brief 初始化坐标系统
 *
 * @param minX 游戏世界最小X坐标
 * @param maxX 游戏世界最大X坐标
 * @param minY 游戏世界最小Y坐标
 * @param maxY 游戏世界最大Y坐标
 * @param screenWidth 屏幕宽度
 * @param screenHeight 屏幕高度
 * @return CoordinateSystem 坐标系统
 */
CoordinateSystem init_coordinate_system(float minX, float maxX, float minY,
                                        float maxY, int screenWidth,
                                        int screenHeight);

/**
 * @brief 将游戏世界坐标转换为OpenGL标准化设备坐标
 *
 * @param coord 坐标系统
 * @param worldX 游戏世界X坐标
 * @param worldY 游戏世界Y坐标
 * @param glX 输出的OpenGL X坐标
 * @param glY 输出的OpenGL Y坐标
 */
void world_to_gl_coords(const CoordinateSystem *coord, float worldX,
                        float worldY, float *glX, float *glY);

/**
 * @brief 将游戏世界尺寸转换为OpenGL标准化设备尺寸
 *
 * @param coord 坐标系统
 * @param worldWidth 游戏世界宽度
 * @param worldHeight 游戏世界高度
 * @param glWidth 输出的OpenGL宽度
 * @param glHeight 输出的OpenGL高度
 */
void world_to_gl_size(const CoordinateSystem *coord, float worldWidth,
                      float worldHeight, float *glWidth, float *glHeight);

/**
 * @brief 创建方格的顶点数据（用于渲染一个方格）
 *
 * @param coord 坐标系统
 * @param worldX 方格中心的世界X坐标
 * @param worldY 方格中心的世界Y坐标
 * @param worldSize 方格的世界尺寸
 * @param vertices 输出的顶点数组（需要至少8个float的空间）
 */
void create_square_vertices(const CoordinateSystem *coord, float worldX,
                            float worldY, float worldSize, float vertices[8]);