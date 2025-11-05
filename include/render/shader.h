#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>

/**
 * @brief 从文件中读取着色器源代码
 *
 * @param filename 着色器文件路径
 * @param shader_source 着色器源代码
 * @return int 0: 成功 1: 失败
 */
int read_shader_from_file(const char *filename, char **shader_source);

/**
 * @brief 创建着色器程序
 *
 * @param filenames 着色器文件路径
 * @param count 着色器文件数量
 * @return GLuint 着色器程序ID
 */
GLuint create_shader(const char **filenames, int count);

/**
 * @brief 使用着色器程序
 *
 * @param shaderProgram 着色器程序ID
 */
void use_shader(GLuint shaderProgram);
/**
 * @brief 设置着色器程序中的uniform变量 float类型 - 1个变量
 *
 * @param shaderProgram 着色器程序ID
 * @param name uniform变量名
 * @param value uniform变量值
 */
void set_uniformf(GLuint shaderProgram, const char *name, float value);

/**
 * @brief 设置着色器程序中的uniform变量 float类型 - 2个变量
 *
 * @param shaderProgram 着色器程序ID
 * @param name uniform变量名
 * @param x uniform变量值
 * @param y uniform变量值
 */
void set_uniform2f(GLuint sp, const char *name, float x, float y);

/**
 * @brief 设置着色器程序中的uniform变量 float类型 - 3个变量
 *
 * @param sp 着色器程序ID
 * @param name uniform变量名
 * @param x uniform变量值
 * @param y uniform变量值
 * @param z uniform变量值
 */
void set_uniform3f(GLuint sp, const char *name, float x, float y, float z);

/**
 * @brief 删除着色器程序
 *
 * @param shaderProgram 着色器程序ID
 */
void delete_shader_program(GLuint shaderProgram);