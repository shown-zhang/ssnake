#pragma once

#include <stdbool.h>
#include "utils/knode.h"

// 蛇身体节点结构体
typedef struct {
    KNode node;     // 链表节点
    int x;          // 网格X坐标
    int y;          // 网格Y坐标
} SnakeSegment;

// 蛇结构体
typedef struct {
    KNode head;         // 链表头节点
    int length;         // 蛇的长度
    bool isAlive;       // 蛇是否存活
} Snake;

/**
 * @brief 初始化蛇
 * @param snake 蛇指针
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param initialLength 初始长度
 */
void init_snake(Snake* snake, int startX, int startY, int initialLength);

/**
 * @brief 清理蛇资源
 * @param snake 蛇指针
 */
void cleanup_snake(Snake* snake);

/**
 * @brief 移动蛇
 * @param snake 蛇指针
 * @param direction 移动方向
 * @param gridWidth 网格宽度
 * @param gridHeight 网格高度
 * @param shouldGrow 是否增长蛇（吃到食物时设为true）
 * @return 是否成功移动（如果撞墙或自身则返回false）
 */
bool move_snake(Snake* snake, int direction, int gridWidth, int gridHeight, bool shouldGrow);

/**
 * @brief 检查蛇是否与位置碰撞
 * @param snake 蛇指针
 * @param x 检查的X坐标
 * @param y 检查的Y坐标
 * @return 是否碰撞
 */
bool check_snake_collision(const Snake* snake, int x, int y);

/**
 * @brief 检查蛇头是否与身体碰撞
 * @param snake 蛇指针
 * @return 是否碰撞
 */
bool check_self_collision(const Snake* snake);

/**
 * @brief 获取蛇头位置
 * @param snake 蛇指针
 * @param x 返回的X坐标
 * @param y 返回的Y坐标
 */
void get_snake_head(const Snake* snake, int* x, int* y);

/**
 * @brief 获取蛇尾位置
 * @param snake 蛇指针
 * @param x 返回的X坐标
 * @param y 返回的Y坐标
 */
void get_snake_tail(const Snake* snake, int* x, int* y);