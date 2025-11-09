#pragma once

#include "utils/ini_parser.h"
#include <stdbool.h>

// 食物元素类型枚举
typedef enum {
    FOOD_ELEMENT_NORMAL = 0,  // 普通食物
    FOOD_ELEMENT_WATER = 1,   // 水元素
    FOOD_ELEMENT_ICE = 2,     // 冰元素
    FOOD_ELEMENT_FIRE = 3,    // 火元素
    FOOD_ELEMENT_ELECTRIC = 4, // 电元素
    FOOD_ELEMENT_WIND = 5     // 风元素
} FoodElementType;

// 食物配置结构体
typedef struct {
    int elementType;          // 元素类型
    int elementLevel;         // 元素级别（1-5）
    int subElementType;       // 子元素类型
    const char* name;         // 元素名称
    const char* effect;       // 效果描述
    float r, g, b;           // 颜色RGB值
    float rarity;             // 稀有度（0.0-1.0）
} FoodConfig;

// 食物配置管理器
typedef struct {
    ini_file_t* iniFile;      // INI文件句柄
    FoodConfig* configs;      // 配置数组
    int configCount;          // 配置数量
    float normalFoodProb;     // 普通食物概率
    float elementFoodProb;    // 元素食物概率
} FoodConfigManager;

/**
 * @brief 初始化食物配置管理器
 * @param manager 配置管理器指针
 * @param iniFilePath INI文件路径
 * @return 是否初始化成功
 */
bool init_food_config_manager(FoodConfigManager* manager, const char* iniFilePath);

/**
 * @brief 清理食物配置管理器资源
 * @param manager 配置管理器指针
 */
void cleanup_food_config_manager(FoodConfigManager* manager);

/**
 * @brief 根据元素类型、级别和子类型获取食物配置
 * @param manager 配置管理器指针
 * @param elementType 元素类型
 * @param elementLevel 元素级别
 * @param subElementType 子元素类型
 * @return 食物配置指针，如果不存在返回NULL
 */
const FoodConfig* get_food_config(const FoodConfigManager* manager, 
                                 int elementType, int elementLevel, int subElementType);

/**
 * @brief 随机生成一个食物配置
 * @param manager 配置管理器指针
 * @param isElementFood 是否生成元素食物（true=元素食物，false=普通食物）
 * @return 食物配置指针
 */
const FoodConfig* generate_random_food_config(FoodConfigManager* manager, bool isElementFood);

/**
 * @brief 获取元素类型名称
 * @param elementType 元素类型
 * @return 元素类型名称
 */
const char* get_element_type_name(int elementType);

/**
 * @brief 获取元素的基础颜色
 * @param manager 配置管理器指针
 * @param elementType 元素类型
 * @param r 红色分量输出
 * @param g 绿色分量输出
 * @param b 蓝色分量输出
 */
void get_element_base_color(const FoodConfigManager* manager, int elementType, float* r, float* g, float* b);