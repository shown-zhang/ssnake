#include "core/food_config.h"
#include "utils/memory.h"
#include "utils/ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 元素类型名称映射（英文，与INI文件中的节名称匹配）
static const char* ELEMENT_TYPE_NAMES[] = {
    "Normal",    // 0
    "Water",     // 1
    "Ice",       // 2
    "Fire",      // 3
    "Electric",  // 4
    "Wind"       // 5
};

// 元素基础颜色映射
static const float ELEMENT_BASE_COLORS[][3] = {
    {1.0f, 1.0f, 1.0f},    // 普通：白色
    {0.0f, 0.4f, 1.0f},     // 水：蓝色
    {0.8f, 0.9f, 1.0f},     // 冰：浅蓝色
    {1.0f, 0.4f, 0.0f},     // 火：橙色
    {1.0f, 1.0f, 0.0f},     // 电：黄色
    {0.8f, 1.0f, 0.8f}      // 风：浅绿色
};

bool init_food_config_manager(FoodConfigManager* manager, const char* iniFilePath) {
    if (manager == NULL || iniFilePath == NULL) {
        return false;
    }
    
    // 初始化管理器
    memset(manager, 0, sizeof(FoodConfigManager));
    
    // 打开INI文件
    manager->iniFile = ini_load(iniFilePath);
    if (manager->iniFile == NULL) {
        printf("无法打开食物配置文件: %s\n", iniFilePath);
        return false;
    }
    
    // 读取基础概率
    manager->normalFoodProb = ini_get_double(manager->iniFile, "General", "normal_food_probability", 0.5);
    manager->elementFoodProb = ini_get_double(manager->iniFile, "General", "element_food_probability", 0.5);
    
    printf("食物配置管理器初始化成功，普通食物概率: %.1f%%，元素食物概率: %.1f%%\n", 
           manager->normalFoodProb * 100, manager->elementFoodProb * 100);
    
    return true;
}

void cleanup_food_config_manager(FoodConfigManager* manager) {
    if (manager == NULL) {
        return;
    }
    
    // 关闭INI文件
    if (manager->iniFile != NULL) {
        ini_free(manager->iniFile);
        manager->iniFile = NULL;
    }
    
    // 释放配置数组
    if (manager->configs != NULL) {
        FREE(manager->configs);
        manager->configs = NULL;
    }
    
    manager->configCount = 0;
}

const FoodConfig* get_food_config(const FoodConfigManager* manager, 
                                 int elementType, int elementLevel, int subElementType) {
    if (manager == NULL || manager->iniFile == NULL) {
        return NULL;
    }
    
    // 如果是普通食物，返回默认配置
    if (elementType == FOOD_ELEMENT_NORMAL) {
        static FoodConfig normalConfig = {
            .elementType = FOOD_ELEMENT_NORMAL,
            .elementLevel = 0,
            .subElementType = 0,
            .name = "普通食物",
            .effect = "获得1分",
            .r = 1.0f, .g = 1.0f, .b = 1.0f,
            .rarity = 1.0f
        };
        return &normalConfig;
    }
    
    // 动态创建配置（简化实现，实际应该预加载所有配置）
    static FoodConfig config;
    memset(&config, 0, sizeof(FoodConfig));
    
    config.elementType = elementType;
    config.elementLevel = elementLevel;
    config.subElementType = subElementType;
    
    // 获取元素名称
    const char* elementName = get_element_type_name(elementType);
    
    // 构建节名称
    char sectionName[64];
    snprintf(sectionName, sizeof(sectionName), "%s_Level%d", elementName, elementLevel);
    
    // 读取配置
    config.rarity = ini_get_double(manager->iniFile, sectionName, "rarity", 0.1);
    
    // 确保子元素类型在有效范围内（1-5）
    int validSubElementType = subElementType;
    if (subElementType < 1) validSubElementType = 1;
    if (subElementType > 5) validSubElementType = 5;
    
    // 读取子元素名称和效果
    char nameKey[64], effectKey[64], colorKey[64];
    snprintf(nameKey, sizeof(nameKey), "sub_type_%d_name", validSubElementType);
    snprintf(effectKey, sizeof(effectKey), "sub_type_%d_effect", validSubElementType);
    snprintf(colorKey, sizeof(colorKey), "sub_type_%d_color", validSubElementType);
    
    const char* name = ini_get_string(manager->iniFile, sectionName, nameKey, "未知效果");
    const char* effect = ini_get_string(manager->iniFile, sectionName, effectKey, "无效果");
    const char* colorStr = ini_get_string(manager->iniFile, sectionName, colorKey, "255,255,255");
    
    // 解析颜色
    int r = 255, g = 255, b = 255;
    sscanf(colorStr, "%d,%d,%d", &r, &g, &b);
    config.r = r / 255.0f;
    config.g = g / 255.0f;
    config.b = b / 255.0f;
    
    // 构建完整名称（使用静态缓冲区，但添加唯一标识避免冲突）
    static char configName[128];
    snprintf(configName, sizeof(configName), "%sLv%d-%s-%d", elementName, elementLevel, name, subElementType);
    config.name = configName;
    config.effect = effect;
    
    return &config;
}

const FoodConfig* generate_random_food_config(FoodConfigManager* manager, bool isElementFood) {
    if (manager == NULL) {
        return NULL;
    }
    
    if (!isElementFood) {
        // 生成普通食物
        return get_food_config(manager, FOOD_ELEMENT_NORMAL, 0, 0);
    }
    
    // 随机选择元素类型（1-5）
    int elementType = (rand() % 5) + 1;
    
    // 根据稀有度随机选择等级
    float randVal = (float)rand() / RAND_MAX;
    int elementLevel;
    if (randVal < 0.4) {
        elementLevel = 1;      // 40% 概率
    } else if (randVal < 0.7) {
        elementLevel = 2;      // 30% 概率
    } else if (randVal < 0.85) {
        elementLevel = 3;      // 15% 概率
    } else if (randVal < 0.95) {
        elementLevel = 4;      // 10% 概率
    } else {
        elementLevel = 5;      // 5% 概率
    }
    
    // 随机选择子元素类型（1-5），与INI文件中的配置匹配
    int subElementType = (rand() % 5) + 1;
    
    return get_food_config(manager, elementType, elementLevel, subElementType);
}

const char* get_element_type_name(int elementType) {
    if (elementType >= 0 && elementType <= 5) {
        return ELEMENT_TYPE_NAMES[elementType];
    }
    return "未知";
}

void get_element_base_color(const FoodConfigManager* manager, int elementType, float* r, float* g, float* b) {
    // 默认颜色（白色）
    *r = 1.0f;
    *g = 1.0f;
    *b = 1.0f;
    
    if (elementType < 0 || elementType > 5) {
        return;
    }
    
    // 如果是普通食物，使用红色
    if (elementType == 0) {
        *r = 1.0f;
        *g = 0.0f;
        *b = 0.0f;
        return;
    }
    
    // 检查配置管理器是否有效
    if (manager == NULL || manager->iniFile == NULL) {
        // 使用硬编码颜色作为后备
        if (elementType >= 0 && elementType <= 5) {
            *r = ELEMENT_BASE_COLORS[elementType][0];
            *g = ELEMENT_BASE_COLORS[elementType][1];
            *b = ELEMENT_BASE_COLORS[elementType][2];
        }
        return;
    }
    
    // 从INI配置读取元素基础颜色
    const char* elementName = get_element_type_name(elementType);
    if (elementName == NULL) {
        return;
    }
    
    // 构建节名称
    char sectionName[64];
    snprintf(sectionName, sizeof(sectionName), "%s", elementName);
    
    // 读取基础颜色配置
    const char* colorStr = ini_get_string(manager->iniFile, sectionName, "base_color", "255,255,255");
    
    // 解析颜色
    int red = 255, green = 255, blue = 255;
    sscanf(colorStr, "%d,%d,%d", &red, &green, &blue);
    
    // 转换为0-1范围的浮点数
    *r = red / 255.0f;
    *g = green / 255.0f;
    *b = blue / 255.0f;
}