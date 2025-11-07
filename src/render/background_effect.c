#include "render/background_effect.h"
#include "utils/memory.h"
#include <math.h>
#include <stdio.h>

// 背景着色器顶点着色器源码
static const char *background_vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec2 fragCoord;\n"
    "void main()\n"
    "{\n"
    "    fragCoord = aPos;\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

// 背景着色器片段着色器源码 - 波浪效果
static const char *background_fragment_shader_wave =
    "#version 330 core\n"
    "in vec2 fragCoord;\n"
    "out vec4 FragColor;\n"
    "uniform float uTime;\n"
    "uniform float uIntensity;\n"
    "uniform vec2 uResolution;\n"
    "void main()\n"
    "{\n"
    "    vec2 uv = fragCoord * 0.5 + 0.5;\n"
    "    uv.x *= uResolution.x / uResolution.y;\n"
    "    \n"
    "    // 创建波浪效果\n"
    "    float wave1 = sin(uv.x * 8.0 + uTime * 2.0) * 0.1;\n"
    "    float wave2 = cos(uv.y * 6.0 + uTime * 1.5) * 0.08;\n"
    "    float wave3 = sin(uv.x * 4.0 + uv.y * 4.0 + uTime * 3.0) * 0.05;\n"
    "    \n"
    "    // 基础颜色 - 深蓝色调\n"
    "    vec3 baseColor = vec3(0.1, 0.2, 0.4);\n"
    "    \n"
    "    // 波浪颜色混合\n"
    "    vec3 color1 = vec3(0.3, 0.5, 0.8); // 浅蓝色\n"
    "    vec3 color2 = vec3(0.1, 0.3, 0.6); // 中蓝色\n"
    "    vec3 color3 = vec3(0.05, 0.15, 0.3); // 深蓝色\n"
    "    \n"
    "    float intensity = uIntensity * 0.5 + 0.5;\n"
    "    \n"
    "    vec3 finalColor = baseColor;\n"
    "    finalColor = mix(finalColor, color1, wave1 * intensity);\n"
    "    finalColor = mix(finalColor, color2, wave2 * intensity);\n"
    "    finalColor = mix(finalColor, color3, wave3 * intensity);\n"
    "    \n"
    "    // 添加一些闪烁效果\n"
    "    float flicker = sin(uTime * 10.0) * 0.02 + 0.98;\n"
    "    finalColor *= flicker;\n"
    "    \n"
    "    FragColor = vec4(finalColor, 1.0);\n"
    "}\n";

// 背景着色器片段着色器源码 - 渐变效果
static const char *background_fragment_shader_gradient =
    "#version 330 core\n"
    "in vec2 fragCoord;\n"
    "out vec4 FragColor;\n"
    "uniform float uTime;\n"
    "uniform float uIntensity;\n"
    "uniform vec2 uResolution;\n"
    "void main()\n"
    "{\n"
    "    vec2 uv = fragCoord * 0.5 + 0.5;\n"
    "    \n"
    "    // 创建动态渐变效果\n"
    "    float angle = uTime * 0.5;\n"
    "    vec2 center = vec2(0.5, 0.5);\n"
    "    vec2 dir = normalize(uv - center);\n"
    "    \n"
    "    // 旋转渐变方向\n"
    "    float cosAngle = cos(angle);\n"
    "    float sinAngle = sin(angle);\n"
    "    vec2 rotatedDir = vec2(dir.x * cosAngle - dir.y * sinAngle, \n"
    "                          dir.x * sinAngle + dir.y * cosAngle);\n"
    "    \n"
    "    // 基础渐变\n"
    "    float gradient = dot(rotatedDir, vec2(1.0, 0.0)) * 0.5 + 0.5;\n"
    "    \n"
    "    // 颜色渐变\n"
    "    vec3 color1 = vec3(0.2, 0.4, 0.6); // 蓝绿色\n"
    "    vec3 color2 = vec3(0.4, 0.2, 0.6); // 紫红色\n"
    "    vec3 color3 = vec3(0.6, 0.4, 0.2); // 橙黄色\n"
    "    \n"
    "    // 动态颜色混合\n"
    "    float colorMix = sin(uTime * 0.3) * 0.5 + 0.5;\n"
    "    vec3 baseColor = mix(color1, color2, colorMix);\n"
    "    \n"
    "    // 最终颜色\n"
    "    float intensity = uIntensity * 0.8 + 0.2;\n"
    "    vec3 finalColor = mix(baseColor, color3, gradient * intensity);\n"
    "    \n"
    "    // 添加脉动效果\n"
    "    float pulse = sin(uTime * 2.0) * 0.1 + 0.9;\n"
    "    finalColor *= pulse;\n"
    "    \n"
    "    FragColor = vec4(finalColor, 1.0);\n"
    "}\n";

// 背景着色器片段着色器源码 - 脉冲效果
static const char *background_fragment_shader_pulse =
    "#version 330 core\n"
    "in vec2 fragCoord;\n"
    "out vec4 FragColor;\n"
    "uniform float uTime;\n"
    "uniform float uIntensity;\n"
    "uniform vec2 uResolution;\n"
    "void main()\n"
    "{\n"
    "    vec2 uv = fragCoord * 0.5 + 0.5;\n"
    "    uv.x *= uResolution.x / uResolution.y;\n"
    "    \n"
    "    // 创建脉冲波效果\n"
    "    vec2 center = vec2(0.5, 0.5);\n"
    "    float dist = distance(uv, center);\n"
    "    \n"
    "    // 多个脉冲波\n"
    "    float pulse1 = sin(dist * 20.0 - uTime * 5.0) * 0.5 + 0.5;\n"
    "    float pulse2 = sin(dist * 15.0 - uTime * 3.0 + 1.0) * 0.5 + 0.5;\n"
    "    float pulse3 = sin(dist * 25.0 - uTime * 7.0 + 2.0) * 0.5 + 0.5;\n"
    "    \n"
    "    // 颜色设置\n"
    "    vec3 color1 = vec3(0.3, 0.1, 0.5); // 深紫色\n"
    "    vec3 color2 = vec3(0.1, 0.3, 0.5); // 深蓝色\n"
    "    vec3 color3 = vec3(0.5, 0.1, 0.3); // 深红色\n"
    "    \n"
    "    // 脉冲颜色混合\n"
    "    float intensity = uIntensity * 0.6 + 0.4;\n"
    "    vec3 finalColor = color1 * pulse1 + color2 * pulse2 + color3 * "
    "pulse3;\n"
    "    finalColor = normalize(finalColor) * intensity;\n"
    "    \n"
    "    // 添加中心亮点\n"
    "    float centerGlow = 1.0 - smoothstep(0.0, 0.3, dist);\n"
    "    finalColor += vec3(0.2, 0.3, 0.5) * centerGlow * 0.3;\n"
    "    \n"
    "    FragColor = vec4(finalColor, 1.0);\n"
    "}\n";

// 编译着色器程序
static GLuint compile_background_shader(const char *fragmentSource) {
  GLuint vertexShader, fragmentShader;
  GLuint shaderProgram;
  int success;
  char infoLog[512];

  // 顶点着色器
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &background_vertex_shader_source, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "顶点着色器编译失败: %s",
                 infoLog);
    return 0;
  }

  // 片段着色器
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "片段着色器编译失败: %s",
                 infoLog);
    glDeleteShader(vertexShader);
    return 0;
  }

  // 着色器程序
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "着色器链接失败: %s", infoLog);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

// 获取当前特效的片段着色器源码
static const char *get_fragment_shader_source(BackgroundEffectType type) {
  switch (type) {
  case BACKGROUND_EFFECT_WAVE:
    return background_fragment_shader_wave;
  case BACKGROUND_EFFECT_GRADIENT:
    return background_fragment_shader_gradient;
  case BACKGROUND_EFFECT_PULSE:
    return background_fragment_shader_pulse;
  default:
    return background_fragment_shader_wave;
  }
}

int init_background_effect(BackgroundEffectManager *manager, int screenWidth,
                           int screenHeight) {
  if (!manager) {
    return 0;
  }

  // 初始化默认值
  manager->currentEffect = BACKGROUND_EFFECT_WAVE;
  manager->time = 0.0f;
  manager->speed = 1.0f;
  manager->intensity = 1.0f;
  manager->screenWidth = screenWidth;
  manager->screenHeight = screenHeight;

  // 编译着色器
  const char *fragmentSource =
      get_fragment_shader_source(manager->currentEffect);
  manager->shaderProgram = compile_background_shader(fragmentSource);
  if (manager->shaderProgram == 0) {
    return 0;
  }

  // 创建全屏四边形顶点数据
  float vertices[] = {
      -1.0f, -1.0f, // 左下
      1.0f,  -1.0f, // 右下
      -1.0f, 1.0f,  // 左上
      1.0f,  1.0f   // 右上
  };

  // 创建VAO和VBO
  glGenVertexArrays(1, &manager->VAO);
  glGenBuffers(1, &manager->VBO);

  glBindVertexArray(manager->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, manager->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 1;
}

void update_background_effect(BackgroundEffectManager *manager,
                              float deltaTime) {
  if (!manager)
    return;

  manager->time += deltaTime * manager->speed;
}

void render_background_effect(BackgroundEffectManager *manager) {
  if (!manager || manager->shaderProgram == 0)
    return;

  glUseProgram(manager->shaderProgram);

  // 设置uniform变量
  glUniform1f(glGetUniformLocation(manager->shaderProgram, "uTime"),
              manager->time);
  glUniform1f(glGetUniformLocation(manager->shaderProgram, "uIntensity"),
              manager->intensity);
  glUniform2f(glGetUniformLocation(manager->shaderProgram, "uResolution"),
              (float)manager->screenWidth, (float)manager->screenHeight);

  glBindVertexArray(manager->VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void cleanup_background_effect(BackgroundEffectManager *manager) {
  if (!manager)
    return;

  if (manager->shaderProgram != 0) {
    glDeleteProgram(manager->shaderProgram);
    manager->shaderProgram = 0;
  }

  if (manager->VAO != 0) {
    glDeleteVertexArrays(1, &manager->VAO);
    manager->VAO = 0;
  }

  if (manager->VBO != 0) {
    glDeleteBuffers(1, &manager->VBO);
    manager->VBO = 0;
  }
}

void set_background_effect_type(BackgroundEffectManager *manager,
                                BackgroundEffectType type) {
  if (!manager || type >= BACKGROUND_EFFECT_COUNT)
    return;

  // 如果特效类型改变，重新编译着色器
  if (manager->currentEffect != type) {
    manager->currentEffect = type;

    // 清理旧的着色器
    if (manager->shaderProgram != 0) {
      glDeleteProgram(manager->shaderProgram);
    }

    // 编译新的着色器
    const char *fragmentSource = get_fragment_shader_source(type);
    manager->shaderProgram = compile_background_shader(fragmentSource);
  }
}

void set_background_effect_speed(BackgroundEffectManager *manager,
                                 float speed) {
  if (!manager)
    return;
  manager->speed = speed;
}

void set_background_effect_intensity(BackgroundEffectManager *manager,
                                     float intensity) {
  if (!manager)
    return;
  manager->intensity = intensity;
}

void resize_background_effect(BackgroundEffectManager *manager, int screenWidth,
                              int screenHeight) {
  if (!manager)
    return;

  manager->screenWidth = screenWidth;
  manager->screenHeight = screenHeight;
}