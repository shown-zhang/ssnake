#pragma once
#include "render/coordinate.h"
#include <glad/glad.h>

/**
 * @brief 方格渲染器结构体
 */
typedef struct {
  GLuint shaderProgram;   // 着色器程序
  GLuint VAO;             // 顶点数组对象
  GLuint VBO;             // 顶点缓冲区对象
  CoordinateSystem coord; // 坐标系统
} SquareRenderer;

/**
 * @brief 初始化方格渲染器
 *
 * @param renderer 渲染器指针
 * @param coord 坐标系统
 * @param vertexShaderPath 顶点着色器文件路径
 * @param fragmentShaderPath 片段着色器文件路径
 * @return int 成功返回1，失败返回0
 */
int init_square_renderer(SquareRenderer *renderer,
                         const CoordinateSystem *coord,
                         const char *vertexShaderPath,
                         const char *fragmentShaderPath);

/**
 * @brief 渲染一个方格
 *
 * @param renderer 渲染器指针
 * @param worldX 方格中心的世界X坐标
 * @param worldY 方格中心的世界Y坐标
 * @param worldSize 方格的世界尺寸
 * @param color 方格颜色（RGBA，每个分量0.0-1.0）
 */
void render_square(SquareRenderer *renderer, float worldX, float worldY,
                   float worldSize, float color[4]);

/**
 * @brief 渲染一个矩形
 *
 * @param renderer 渲染器指针
 * @param worldX 矩形中心的世界X坐标
 * @param worldY 矩形中心的世界Y坐标
 * @param worldWidth 矩形的世界宽度
 * @param worldHeight 矩形的世界高度
 * @param color 矩形颜色（RGBA，每个分量0.0-1.0）
 */
void render_rectangle(SquareRenderer *renderer, float worldX, float worldY,
                      float worldWidth, float worldHeight, float color[4]);

/**
 * @brief 清理渲染器资源
 *
 * @param renderer 渲染器指针
 */
void cleanup_square_renderer(SquareRenderer *renderer);