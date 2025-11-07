#ifndef BACKGROUND_EFFECT_H
#define BACKGROUND_EFFECT_H

#include <SDL3/SDL.h>
#include <glad/glad.h>

// 背景特效类型枚举
typedef enum {
  BACKGROUND_EFFECT_WAVE = 0, // 波浪效果
  BACKGROUND_EFFECT_GRADIENT, // 渐变效果
  BACKGROUND_EFFECT_PULSE,    // 脉冲效果
  BACKGROUND_EFFECT_WARM_RANDOM, // 暖色调随机动画效果
  BACKGROUND_EFFECT_COUNT     // 效果数量
} BackgroundEffectType;

// 背景特效管理器结构体
typedef struct {
  GLuint shaderProgram;               // 着色器程序
  GLuint VAO;                         // 顶点数组对象
  GLuint VBO;                         // 顶点缓冲区对象
  BackgroundEffectType currentEffect; // 当前特效类型
  float time;                         // 时间变量，用于动画
  float speed;                        // 动画速度
  float intensity;                    // 特效强度
  int screenWidth;                    // 屏幕宽度
  int screenHeight;                   // 屏幕高度
} BackgroundEffectManager;

// 函数声明
int init_background_effect(BackgroundEffectManager *manager, int screenWidth,
                           int screenHeight);
void update_background_effect(BackgroundEffectManager *manager,
                              float deltaTime);
void render_background_effect(BackgroundEffectManager *manager);
void cleanup_background_effect(BackgroundEffectManager *manager);
void set_background_effect_type(BackgroundEffectManager *manager,
                                BackgroundEffectType type);
void set_background_effect_speed(BackgroundEffectManager *manager, float speed);
void set_background_effect_intensity(BackgroundEffectManager *manager,
                                     float intensity);
void resize_background_effect(BackgroundEffectManager *manager, int screenWidth,
                              int screenHeight);

#endif // BACKGROUND_EFFECT_H