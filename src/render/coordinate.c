#include "render/coordinate.h"

CoordinateSystem init_coordinate_system(float minX, float maxX, float minY,
                                        float maxY, int screenWidth,
                                        int screenHeight) {
  CoordinateSystem coord;
  coord.minX = minX;
  coord.maxX = maxX;
  coord.minY = minY;
  coord.maxY = maxY;
  coord.screenWidth = screenWidth;
  coord.screenHeight = screenHeight;
  return coord;
}

void world_to_gl_coords(const CoordinateSystem *coord, float worldX,
                        float worldY, float *glX, float *glY) {
  // 将世界坐标归一化到[0, 1]范围
  float normalizedX = (worldX - coord->minX) / (coord->maxX - coord->minX);
  float normalizedY = (worldY - coord->minY) / (coord->maxY - coord->minY);

  // 将归一化坐标转换到OpenGL的[-1, 1]范围
  *glX = normalizedX * 2.0f - 1.0f;
  *glY = normalizedY * 2.0f - 1.0f;

  // 根据窗口宽高比调整坐标，保持网格比例
  float aspectRatio = (float)coord->screenWidth / (float)coord->screenHeight;
  float targetAspectRatio = (coord->maxX - coord->minX) / (coord->maxY - coord->minY);
  
  if (aspectRatio > targetAspectRatio) {
    // 窗口比网格宽，在X轴方向缩放
    *glX *= targetAspectRatio / aspectRatio;
  } else {
    // 窗口比网格高，在Y轴方向缩放
    *glY *= aspectRatio / targetAspectRatio;
  }

  // 注意：OpenGL的Y轴方向与屏幕Y轴方向相反，所以需要翻转Y轴
  *glY = -*glY;
}

void world_to_gl_size(const CoordinateSystem *coord, float worldWidth,
                      float worldHeight, float *glWidth, float *glHeight) {
  // 将世界尺寸归一化到[0, 1]范围
  float normalizedWidth = worldWidth / (coord->maxX - coord->minX);
  float normalizedHeight = worldHeight / (coord->maxY - coord->minY);

  // 将归一化尺寸转换到OpenGL的[-1, 1]范围
  *glWidth = normalizedWidth * 2.0f;
  *glHeight = normalizedHeight * 2.0f;

  // 根据窗口宽高比调整尺寸，保持网格比例
  float aspectRatio = (float)coord->screenWidth / (float)coord->screenHeight;
  float targetAspectRatio = (coord->maxX - coord->minX) / (coord->maxY - coord->minY);
  
  if (aspectRatio > targetAspectRatio) {
    // 窗口比网格宽，在X轴方向缩放
    *glWidth *= targetAspectRatio / aspectRatio;
  } else {
    // 窗口比网格高，在Y轴方向缩放
    *glHeight *= aspectRatio / targetAspectRatio;
  }
}

void create_square_vertices(const CoordinateSystem *coord, float worldX,
                            float worldY, float worldSize, float vertices[8]) {
  // 计算方格的四个角在世界坐标系中的位置
  float halfSize = worldSize / 2.0f;

  float corners[4][2] = {
      {worldX - halfSize, worldY - halfSize}, // 左下角
      {worldX + halfSize, worldY - halfSize}, // 右下角
      {worldX + halfSize, worldY + halfSize}, // 右上角
      {worldX - halfSize, worldY + halfSize}  // 左上角
  };

  // 将每个角转换为OpenGL坐标
  for (int i = 0; i < 4; i++) {
    float glX, glY;
    world_to_gl_coords(coord, corners[i][0], corners[i][1], &glX, &glY);
    vertices[i * 2] = glX;     // X坐标
    vertices[i * 2 + 1] = glY; // Y坐标
  }
}

/**
 * @brief 创建方格渲染的完整顶点数据（包括顶点位置和纹理坐标）
 *
 * @param coord 坐标系统
 * @param worldX 方格中心的世界X坐标
 * @param worldY 方格中心的世界Y坐标
 * @param worldSize 方格的世界尺寸
 * @param vertices 输出的顶点数组（需要至少16个float的空间：4个顶点，每个顶点有位置和纹理坐标）
 */
void create_square_vertices_with_texcoords(const CoordinateSystem *coord,
                                           float worldX, float worldY,
                                           float worldSize,
                                           float vertices[16]) {
  // 先创建位置顶点
  float positionVertices[8];
  create_square_vertices(coord, worldX, worldY, worldSize, positionVertices);

  // 定义纹理坐标（每个顶点对应一个纹理坐标）
  float texCoords[8] = {
      0.0f, 0.0f, // 左下角
      1.0f, 0.0f, // 右下角
      1.0f, 1.0f, // 右上角
      0.0f, 1.0f  // 左上角
  };

  // 组合位置和纹理坐标
  for (int i = 0; i < 4; i++) {
    // 位置坐标
    vertices[i * 4] = positionVertices[i * 2];         // X
    vertices[i * 4 + 1] = positionVertices[i * 2 + 1]; // Y
    // 纹理坐标
    vertices[i * 4 + 2] = texCoords[i * 2];     // U
    vertices[i * 4 + 3] = texCoords[i * 2 + 1]; // V
  }
}

/**
 * @brief 更新坐标系统的屏幕尺寸
 *
 * @param coord 坐标系统指针
 * @param screenWidth 新的屏幕宽度
 * @param screenHeight 新的屏幕高度
 */
void update_coordinate_screen_size(CoordinateSystem *coord, int screenWidth, int screenHeight) {
    coord->screenWidth = screenWidth;
    coord->screenHeight = screenHeight;
}