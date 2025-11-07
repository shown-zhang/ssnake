#pragma once

#include <stdbool.h>
#include "utils/knode.h"

// 食物结构体
typedef struct {
    KNode node;     // 链表节点
    int x;          // 网格X坐标
    int y;          // 网格Y坐标
    int value;      // 食物价值（得分）
} Food;

// 食物管理器结构体
typedef struct {
    KNode head;         // 食物链表头节点
    int count;          // 当前食物数量
    int maxCount;       // 最大食物数量
} FoodManager;

/**
 * @brief 初始化食物管理器
 * @param manager 食物管理器指针
 * @param maxCount 最大食物数量
 */
void init_food_manager(FoodManager* manager, int maxCount);

/**
 * @brief 清理食物管理器资源
 * @param manager 食物管理器指针
 */
void cleanup_food_manager(FoodManager* manager);

/**
 * @brief 生成新食物
 * @param manager 食物管理器指针
 * @param gridWidth 网格宽度
 * @param gridHeight 网格高度
 * @param snake 蛇指针（用于避免在蛇身上生成食物）
 * @return 是否成功生成食物
 */
bool generate_food(FoodManager* manager, int gridWidth, int gridHeight, const void* snake);

/**
 * @brief 检查位置是否有食物
 * @param manager 食物管理器指针
 * @param x 检查的X坐标
 * @param y 检查的Y坐标
 * @return 是否有食物，如果有则返回食物指针，否则返回NULL
 */
Food* check_food_at_position(const FoodManager* manager, int x, int y);

/**
 * @brief 移除食物
 * @param manager 食物管理器指针
 * @param food 要移除的食物指针
 * @return 移除的食物价值（得分）
 */
int remove_food(FoodManager* manager, Food* food);

/**
 * @brief 获取当前食物数量
 * @param manager 食物管理器指针
 * @return 食物数量
 */
int get_food_count(const FoodManager* manager);

/**
 * @brief 检查是否达到最大食物数量
 * @param manager 食物管理器指针
 * @return 是否达到最大数量
 */
bool is_food_max_reached(const FoodManager* manager);