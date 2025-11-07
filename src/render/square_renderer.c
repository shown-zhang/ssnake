#include "render/square_renderer.h"
#include "render/shader.h"
#include <SDL3/SDL.h>

int init_square_renderer(SquareRenderer *renderer,
                         const CoordinateSystem *coord,
                         const char *vertexShaderPath,
                         const char *fragmentShaderPath) {
  // 复制坐标系统
  renderer->coord = *coord;

  // 创建着色器程序
  const char *shaderFiles[] = {vertexShaderPath, fragmentShaderPath};
  renderer->shaderProgram = create_shader(shaderFiles, 2);
  if (renderer->shaderProgram == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "创建着色器程序失败");
    return 0;
  }

  // 创建顶点数据（一个单位方格）
  float vertices[] = {
      // 位置坐标     // 纹理坐标
      -0.5f, -0.5f, 0.0f, 0.0f, // 左下角
      0.5f,  -0.5f, 1.0f, 0.0f, // 右下角
      0.5f,  0.5f,  1.0f, 1.0f, // 右上角
      -0.5f, 0.5f,  0.0f, 1.0f  // 左上角
  };

  unsigned int indices[] = {
      0, 1, 2, // 第一个三角形
      2, 3, 0  // 第二个三角形
  };

  // 创建VAO, VBO, EBO
  glGenVertexArrays(1, &renderer->VAO);
  glGenBuffers(1, &renderer->VBO);

  glBindVertexArray(renderer->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 位置属性
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // 纹理坐标属性
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // 创建EBO
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);

  return 1;
}

void render_square(SquareRenderer *renderer, float worldX, float worldY,
                   float worldSize, float color[4]) {
  render_rectangle(renderer, worldX, worldY, worldSize, worldSize, color);
}

void render_rectangle(SquareRenderer *renderer, float worldX, float worldY,
                      float worldWidth, float worldHeight, float color[4]) {
  // 使用着色器程序
  use_shader(renderer->shaderProgram);

  // 计算模型矩阵（缩放和平移）
  float glX, glY;
  world_to_gl_coords(&renderer->coord, worldX, worldY, &glX, &glY);

  float glWidth, glHeight;
  world_to_gl_size(&renderer->coord, worldWidth, worldHeight, &glWidth,
                   &glHeight);

  // 设置颜色uniform
  GLint colorLocation = glGetUniformLocation(renderer->shaderProgram, "color");
  glUniform4f(colorLocation, color[0], color[1], color[2], color[3]);

  // 设置变换uniform
  GLint transformLocation =
      glGetUniformLocation(renderer->shaderProgram, "transform");

  // 创建变换矩阵（先缩放后平移）
  float transform[16] = {glWidth, 0.0f, 0.0f, 0.0f, 0.0f, glHeight, 0.0f, 0.0f,
                         0.0f,    0.0f, 1.0f, 0.0f, glX,  glY,      0.0f, 1.0f};

  glUniformMatrix4fv(transformLocation, 1, GL_FALSE, transform);

  // 渲染矩形
  glBindVertexArray(renderer->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void cleanup_square_renderer(SquareRenderer *renderer) {
  if (renderer->shaderProgram) {
    delete_shader_program(renderer->shaderProgram);
  }
  if (renderer->VAO) {
    glDeleteVertexArrays(1, &renderer->VAO);
  }
  if (renderer->VBO) {
    glDeleteBuffers(1, &renderer->VBO);
  }
}