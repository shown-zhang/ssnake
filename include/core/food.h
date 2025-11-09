#pragma once

#include "utils/knode.h"
#include <stdbool.h>

// 颜色结构体
typedef struct {
  float r; // 红色分量 (0.0 - 1.0)
  float g; // 绿色分量 (0.0 - 1.0)
  float b; // 蓝色分量 (0.0 - 1.0)
  float a; // 透明度分量 (0.0 - 1.0)
} FoodColor;

// 食物结构体
typedef struct {
  KNode node;         // 链表节点
  int x;              // 网格X坐标
  int y;              // 网格Y坐标
                      // 新增字段：元素类型
  int elementType;    // 元素类型
                      // 新增字段：元素级别
  int elementLevel;   // 元素级别
                      // 新增字段：子元素类型
  int subElementType; // 子元素类型
  int value;          // 食物价值（得分）
} Food;

// 食物管理器结构体
typedef struct {
  KNode head;   // 食物链表头节点
  int count;    // 当前食物数量
  int maxCount; // 最大食物数量
} FoodManager;

/**
 * @brief 初始化食物管理器
 * @param manager 食物管理器指针
 * @param maxCount 最大食物数量
 */
void init_food_manager(FoodManager *manager, int maxCount);

/**
 * @brief 清理食物管理器资源
 * @param manager 食物管理器指针
 */
void cleanup_food_manager(FoodManager *manager);

/**
 * @brief 生成新食物
 * @param manager 食物管理器指针
 * @param gridWidth 网格宽度
 * @param gridHeight 网格高度
 * @param snake 蛇指针（用于避免在蛇身上生成食物）
 * @return 是否成功生成食物
 */
bool generate_food(FoodManager *manager, int gridWidth, int gridHeight,
                   const void *snake);

/**
 * @brief 检查位置是否有食物
 * @param manager 食物管理器指针
 * @param x 检查的X坐标
 * @param y 检查的Y坐标
 * @return 是否有食物，如果有则返回食物指针，否则返回NULL
 */
Food *check_food_at_position(const FoodManager *manager, int x, int y);

/**
 * @brief 移除食物
 * @param manager 食物管理器指针
 * @param food 要移除的食物指针
 * @return 移除的食物价值（得分）
 */
int remove_food(FoodManager *manager, Food *food);

/**
 * @brief 获取当前食物数量
 * @param manager 食物管理器指针
 * @return 食物数量
 */
int get_food_count(const FoodManager *manager);

/**
 * @brief 检查是否达到最大食物数量
 * @param manager 食物管理器指针
 * @return 是否达到最大数量
 */
bool is_food_max_reached(const FoodManager *manager);

/**
 * @brief 设置食物的特殊属性
 * @param food 食物指针
 * @param elementType 元素类型（0=普通食物，其他=特殊食物）
 * @param elementLevel 元素级别（数值越大越稀有）
 * @param subElementType 子元素类型（具体增益效果的ID）
 */
void set_food_special_attributes(Food *food, int elementType, int elementLevel,
                                 int subElementType);

/**
 * @brief 获取食物的元素类型
 * @param food 食物指针
 * @return 元素类型
 */
int get_food_element_type(const Food *food);

/**
 * @brief 获取食物的元素级别
 * @param food 食物指针
 * @return 元素级别
 */
int get_food_element_level(const Food *food);

/**
 * @brief 获取食物的子元素类型
 * @param food 食物指针
 * @return 子元素类型
 */
int get_food_sub_element_type(const Food *food);

/**
 * @brief 检查食物是否为特殊食物
 * @param food 食物指针
 * @return 如果是特殊食物返回true，否则返回false
 */
bool is_food_special(const Food *food);

/**
 * @brief 生成特殊食物
 * @param manager 食物管理器指针
 * @param gridWidth 网格宽度
 * @param gridHeight 网格高度
 * @param snake 蛇指针
 * @param elementType 元素类型
 * @param elementLevel 元素级别
 * @param subElementType 子元素类型
 * @return 是否成功生成特殊食物
 */
bool generate_special_food(FoodManager *manager, int gridWidth, int gridHeight,
                           const void *snake, int elementType, int elementLevel,
                           int subElementType);

/**
 * @brief 获取食物的颜色
 * @param food 食物指针
 * @return 食物的颜色
 */
FoodColor get_food_color(const Food *food);

/**
 * @brief 根据元素类型和级别获取基础颜色
 * @param elementType 元素类型
 * @param elementLevel 元素级别
 * @return 基础颜色
 */
FoodColor get_base_element_color(int elementType, int elementLevel);

/**
 * @brief 根据子元素类型获取详细颜色
 * @param elementType 元素类型
 * @param elementLevel 元素级别
 * @param subElementType 子元素类型
 * @return 详细颜色
 */
FoodColor get_detailed_food_color(int elementType, int elementLevel,
                                  int subElementType);

/**
 * @brief 获取食物的效果描述
 * @param food 食物指针
 * @return 效果描述字符串
 */
const char *get_food_effect_description(const Food *food);

/**
 * @brief 应用食物效果到蛇
 * @param food 食物指针
 * @param snake 蛇指针
 * @return 应用是否成功
 */
bool apply_food_effect(const Food *food, void *snake);

/**
 * @brief 获取食物效果的数值（如增益/减益值）
 * @param food 食物指针
 * @return 效果数值
 */
int get_food_effect_value(const Food *food);