#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "utils/knode.h"

// 元素类型枚举
typedef enum {
    ELEMENT_NONE = 0,
    ELEMENT_WIND,   // 风
    ELEMENT_FIRE,   // 火
    ELEMENT_ELECTRIC, // 电
    ELEMENT_ICE,    // 冰
    ELEMENT_WATER   // 水
} ElementType;

// 元素反应类型枚举
typedef enum {
    REACTION_NONE = 0,
    REACTION_EVAPORATE,    // 蒸发：火 + 水
    REACTION_MELT,         // 融化：火 + 冰
    REACTION_FREEZE,       // 冻结：冰 + 水
    REACTION_OVERLOAD,     // 超载：火 + 电
    REACTION_SUPERCONDUCT, // 超导：冰 + 电
    REACTION_ELECTRO_CHARGED, // 感电：水 + 电
    REACTION_SWIRL         // 扩散：风 + 其他元素
} ElementReactionType;

// 元素效果加成结构体
typedef struct {
    char name[32];         // 效果名称
    int duration;          // 持续时间（帧数）
    int power;             // 效果强度
    ElementType element;   // 所属元素
} ElementEffect;

// 元素食物结构体
typedef struct {
    KNode node;            // 链表节点
    int x;                 // 网格X坐标
    int y;                 // 网格Y坐标
    ElementType element;   // 元素类型
    ElementEffect effect;  // 元素效果
    int value;             // 基础价值
} ElementFood;

// 元素反应结构体
typedef struct {
    ElementType element1;          // 第一个元素
    ElementType element2;          // 第二个元素
    ElementReactionType reaction;  // 反应类型
    int damage;                    // 反应伤害
    int duration;                  // 反应持续时间
} ElementReaction;

// 元素管理器结构体
typedef struct {
    KNode head;                    // 元素食物链表头节点
    int count;                     // 当前元素食物数量
    int maxCount;                  // 最大元素食物数量
    ElementEffect effects[50];     // 元素效果配置
    int effectCount;               // 效果数量
    ElementReaction reactions[20]; // 元素反应配置
    int reactionCount;             // 反应数量
} ElementManager;

/**
 * @brief 初始化元素管理器
 * @param manager 元素管理器指针
 * @param maxCount 最大元素食物数量
 */
void init_element_manager(ElementManager* manager, int maxCount);

/**
 * @brief 清理元素管理器资源
 * @param manager 元素管理器指针
 */
void cleanup_element_manager(ElementManager* manager);

/**
 * @brief 从配置文件加载元素效果
 * @param manager 元素管理器指针
 * @param configPath 配置文件路径
 * @return 是否成功加载
 */
bool load_element_effects_from_config(ElementManager* manager, const char* configPath);

/**
 * @brief 从配置文件加载元素反应
 * @param manager 元素管理器指针
 * @param configPath 配置文件路径
 * @return 是否成功加载
 */
bool load_element_reactions_from_config(ElementManager* manager, const char* configPath);

/**
 * @brief 生成新的元素食物
 * @param manager 元素管理器指针
 * @param gridWidth 网格宽度
 * @param gridHeight 网格高度
 * @param snake 蛇指针（用于避免在蛇身上生成食物）
 * @return 是否成功生成元素食物
 */
bool generate_element_food(ElementManager* manager, int gridWidth, int gridHeight, const void* snake);

/**
 * @brief 检查位置是否有元素食物
 * @param manager 元素管理器指针
 * @param x 检查的X坐标
 * @param y 检查的Y坐标
 * @return 是否有元素食物，如果有则返回元素食物指针，否则返回NULL
 */
ElementFood* check_element_food_at_position(const ElementManager* manager, int x, int y);

/**
 * @brief 移除元素食物
 * @param manager 元素管理器指针
 * @param food 要移除的元素食物指针
 * @return 移除的元素食物价值
 */
int remove_element_food(ElementManager* manager, ElementFood* food);

/**
 * @brief 获取元素反应
 * @param manager 元素管理器指针
 * @param element1 第一个元素
 * @param element2 第二个元素
 * @return 元素反应信息，如果没有反应则返回NULL
 */
const ElementReaction* get_element_reaction(const ElementManager* manager, ElementType element1, ElementType element2);

/**
 * @brief 获取随机元素效果
 * @param manager 元素管理器指针
 * @param element 元素类型
 * @return 随机元素效果指针，如果没有对应元素的效果则返回NULL
 */
const ElementEffect* get_random_element_effect(const ElementManager* manager, ElementType element);

/**
 * @brief 获取当前元素食物数量
 * @param manager 元素管理器指针
 * @return 元素食物数量
 */
int get_element_food_count(const ElementManager* manager);

/**
 * @brief 检查是否达到最大元素食物数量
 * @param manager 元素管理器指针
 * @return 是否达到最大数量
 */
bool is_element_food_max_reached(const ElementManager* manager);

/**
 * @brief 获取元素类型名称
 * @param element 元素类型
 * @return 元素类型名称字符串
 */
const char* get_element_type_name(ElementType element);

/**
 * @brief 获取元素反应类型名称
 * @param reaction 元素反应类型
 * @return 元素反应类型名称字符串
 */
const char* get_reaction_type_name(ElementReactionType reaction);