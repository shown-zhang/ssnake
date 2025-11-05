#include "render/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_shader_from_file(const char *filename, char **shader_source) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "打开着色器文件[%s]失败 ",
                 filename);
    return 0;
  }

  // 文件内部指针指向文件末尾，计算文件大小
  fseek(fp, 0, SEEK_END);
  // 计算文件大小
  long fsize = ftell(fp);
  // 文件内部指针指向文件开头
  fseek(fp, 0, SEEK_SET);
  // 分配内存
  *shader_source = (char *)malloc(fsize + 1);
  if (*shader_source == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "读取着色器文件[%s]时,内存分配失败 ", filename);
    fclose(fp);
    return 0;
  }
  fread(*shader_source, 1, fsize, fp);

  fclose(fp);
  (*shader_source)[fsize] = '\0';
  return 1;
}

GLuint create_shader(const char **filenames, int count) {
  unsigned int shaderProgram;
  unsigned int shaders[count];
  int success;
  char infoLog[512];

  // 创建着色器程序
  shaderProgram = glCreateProgram();

  // 读取着色器源代码
  for (int i = 0; i < count; i++) {
    char *shader_source = NULL; // 着色器源代码
    int shader_type;            // 着色器类型
    unsigned int someShader;    // 着色器对象

    if (!read_shader_from_file(filenames[i], &shader_source)) {
      return SDL_SetError("读取着色器文件[%s]失败 ", filenames[i]);
    }
    // 根据文件名后缀判断着色器类型
    if (strstr(filenames[i], ".vert") != NULL) {
      shader_type = GL_VERTEX_SHADER;
    } else if (strstr(filenames[i], ".frag") != NULL) {
      shader_type = GL_FRAGMENT_SHADER;
    } else {
      return SDL_SetError("不支持的着色器类型: %s\n", filenames[i]);
    }

    someShader = glCreateShader(shader_type);
    const char *shader_source_c = shader_source;
    // glShaderSource函数把要编译的着色器对象作为第一个参数。
    // 第二参数指定了传递的源码字符串数量，这里只有一个。
    // 第三个参数是顶点着色器真正的源码，
    // 第四个参数我们先设置为NULL。
    glShaderSource(someShader, 1, &shader_source_c, NULL);
    glCompileShader(someShader);

    glGetShaderiv(someShader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(someShader, 512, NULL, infoLog);
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "着色器编译失败: %s", infoLog);
      return SDL_SetError("着色器编译失败: %s", infoLog);
    }

    glAttachShader(shaderProgram, someShader);

    // 记录着色器对象，在链接后可以删除
    shaders[i] = someShader;
    free(shader_source);
  }

  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "着色器链接失败: %s", infoLog);
    return SDL_SetError("着色器链接失败: %s", infoLog);
  }

  // 删除着色器对象
  for (int i = 0; i < count; i++) {
    glDeleteShader(shaders[i]);
  }

  return shaderProgram;
}

void use_shader(GLuint shaderProgram) { glUseProgram(shaderProgram); }

void set_uniformf(GLuint shaderProgram, const char *name, float value) {
  glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}

void set_uniform2f(GLuint shaderProgram, const char *name, float x, float y) {
  glUniform2f(glGetUniformLocation(shaderProgram, name), x, y);
}

void set_uniform3f(GLuint shaderProgram, const char *name, float x, float y,
                   float z) {
  glUniform3f(glGetUniformLocation(shaderProgram, name), x, y, z);
}

void delete_shader_program(GLuint shaderProgram) {
  glDeleteProgram(shaderProgram);
}