#include "core/element.h"
#include "core/snake.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 默认元素效果配置
static ElementEffect default_effects[] = {
    // 风元素效果
    {"疾风加速", 300, 5, ELEMENT_WIND},
    {"风之护盾", 200, 3, ELEMENT_WIND},
    {"旋风攻击", 150, 8, ELEMENT_WIND},
    
    // 火元素效果
    {"火焰强化", 250, 7, ELEMENT_FIRE},
    {"灼烧伤害", 180, 6, ELEMENT_FIRE},
    {"爆裂冲击", 120, 10, ELEMENT_FIRE},
    
    // 电元素效果
    {"闪电速度", 280, 6, ELEMENT_ELECTRIC},
    {"电磁护盾", 220, 4, ELEMENT_ELECTRIC},
    {"雷击麻痹", 160, 9, ELEMENT_ELECTRIC},
    
    // 冰元素效果
    {"冰冻减速", 320, 4, ELEMENT_ICE},
    {"冰霜护甲", 240, 5, ELEMENT_ICE},
    {"寒冰爆发", 140, 7, ELEMENT_ICE},
    
    // 水元素效果
    {"水流治愈", 350, 3, ELEMENT_WATER},
    {"水之屏障", 260, 4, ELEMENT_WATER},
    {"潮汐冲击", 130, 6, ELEMENT_WATER}
};

// 默认元素反应配置
static ElementReaction default_reactions[] = {
    {ELEMENT_FIRE, ELEMENT_WATER, REACTION_EVAPORATE, 20, 60},
    {ELEMENT_WATER, ELEMENT_FIRE, REACTION_EVAPORATE, 20, 60},
    
    {ELEMENT_FIRE, ELEMENT_ICE, REACTION_MELT, 15, 50},
    {ELEMENT_ICE, ELEMENT_FIRE, REACTION_MELT, 15, 50},
    
    {ELEMENT_ICE, ELEMENT_WATER, REACTION_FREEZE, 10, 80},
    {ELEMENT_WATER, ELEMENT_ICE, REACTION_FREEZE, 10, 80},
    
    {ELEMENT_FIRE, ELEMENT_ELECTRIC, REACTION_OVERLOAD, 25, 40},
    {ELEMENT_ELECTRIC, ELEMENT_FIRE, REACTION_OVERLOAD, 25, 40},
    
    {ELEMENT_ICE, ELEMENT_ELECTRIC, REACTION_SUPERCONDUCT, 18, 55},
    {ELEMENT_ELECTRIC, ELEMENT_ICE, REACTION_SUPERCONDUCT, 18, 55},
    
    {ELEMENT_WATER, ELEMENT_ELECTRIC, REACTION_ELECTRO_CHARGED, 12, 70},
    {ELEMENT_ELECTRIC, ELEMENT_WATER, REACTION_ELECTRO_CHARGED, 12, 70},
    
    {ELEMENT_WIND, ELEMENT_FIRE, REACTION_SWIRL, 8, 45},
    {ELEMENT_WIND, ELEMENT_WATER, REACTION_SWIRL, 8, 45},
    {ELEMENT_WIND, ELEMENT_ICE, REACTION_SWIRL, 8, 45},
    {ELEMENT_WIND, ELEMENT_ELECTRIC, REACTION_SWIRL, 8, 45}
};

void init_element_manager(ElementManager* manager, int maxCount) {
    if (manager == NULL) {
        return;
    }

    // 初始化链表头
    knode_init(&manager->head);
    manager->count = 0;
    manager->maxCount = maxCount;

    // 加载默认效果
    memcpy(manager->effects, default_effects, sizeof(default_effects));
    manager->effectCount = sizeof(default_effects) / sizeof(ElementEffect);

    // 加载默认反应
    memcpy(manager->reactions, default_reactions, sizeof(default_reactions));
    manager->reactionCount = sizeof(default_reactions) / sizeof(ElementReaction);

    // 初始化随机数种子
    srand((unsigned int)time(NULL));
}

void cleanup_element_manager(ElementManager* manager) {
    if (manager == NULL) {
        return;
    }

    // 遍历链表并释放所有元素食物
    KNode *node, *tmp;
    knode_for_each_safe(node, tmp, &manager->head) {
        ElementFood* food = container_of(node, ElementFood, node);
        knode_del(node);
        FREE(food);
    }

    manager->count = 0;
}

bool load_element_effects_from_config(ElementManager* manager, const char* configPath) {
    if (manager == NULL || configPath == NULL) {
        return false;
    }

    // 这里实现配置文件读取逻辑
    // 暂时使用默认配置
    printf("加载元素效果配置: %s\n", configPath);
    return true;
}

bool load_element_reactions_from_config(ElementManager* manager, const char* configPath) {
    if (manager == NULL || configPath == NULL) {
        return false;
    }

    // 这里实现配置文件读取逻辑
    // 暂时使用默认配置
    printf("加载元素反应配置: %s\n", configPath);
    return true;
}

bool generate_element_food(ElementManager* manager, int gridWidth, int gridHeight, const void* snake) {
    if (manager == NULL || manager->count >= manager->maxCount) {
        return false;
    }

    const Snake* snakePtr = (const Snake*)snake;

    // 尝试生成元素食物的最大尝试次数
    const int maxAttempts = 100;
    int attempts = 0;

    while (attempts < maxAttempts) {
        // 生成随机位置
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;

        // 检查位置是否有效（不在蛇身上，且没有其他元素食物）
        bool positionValid = true;

        // 检查是否在蛇身上
        if (snakePtr != NULL && check_snake_collision(snakePtr, x, y)) {
            positionValid = false;
        }

        // 检查是否已有元素食物
        if (positionValid && check_element_food_at_position(manager, x, y) != NULL) {
            positionValid = false;
        }

        if (positionValid) {
            // 创建新元素食物
            ElementFood* food = (ElementFood*)MALLOC(sizeof(ElementFood));
            if (food == NULL) {
                return false;
            }

            // 随机选择元素类型
            ElementType elementType = (ElementType)((rand() % 5) + 1); // 1-5 对应五种元素
            
            // 获取随机元素效果
            const ElementEffect* effect = get_random_element_effect(manager, elementType);
            if (effect == NULL) {
                FREE(food);
                return false;
            }

            // 初始化元素食物
            food->node.next = &food->node;
            food->node.prev = &food->node;
            food->x = x;
            food->y = y;
            food->element = elementType;
            food->effect = *effect;
            food->value = 2; // 元素食物基础价值为2分

            // 添加到链表
            knode_add(&food->node, &manager->head);
            manager->count++;

            return true;
        }

        attempts++;
    }

    return false;
}

ElementFood* check_element_food_at_position(const ElementManager* manager, int x, int y) {
    if (manager == NULL) {
        return NULL;
    }

    KNode* node;
    knode_for_each(node, &manager->head) {
        ElementFood* food = container_of(node, ElementFood, node);
        if (food->x == x && food->y == y) {
            return food;
        }
    }

    return NULL;
}

int remove_element_food(ElementManager* manager, ElementFood* food) {
    if (manager == NULL || food == NULL) {
        return 0;
    }

    int value = food->value;

    // 从链表中移除
    knode_del(&food->node);
    FREE(food);
    manager->count--;

    return value;
}

const ElementReaction* get_element_reaction(const ElementManager* manager, ElementType element1, ElementType element2) {
    if (manager == NULL || element1 == ELEMENT_NONE || element2 == ELEMENT_NONE) {
        return NULL;
    }

    for (int i = 0; i < manager->reactionCount; i++) {
        ElementReaction* reaction = &manager->reactions[i];
        if ((reaction->element1 == element1 && reaction->element2 == element2) ||
            (reaction->element1 == element2 && reaction->element2 == element1)) {
            return reaction;
        }
    }

    return NULL;
}

const ElementEffect* get_random_element_effect(const ElementManager* manager, ElementType element) {
    if (manager == NULL) {
        return NULL;
    }

    // 收集该元素的所有效果
    ElementEffect* validEffects[50];
    int validCount = 0;

    for (int i = 0; i < manager->effectCount; i++) {
        if (manager->effects[i].element == element) {
            validEffects[validCount++] = &manager->effects[i];
        }
    }

    if (validCount == 0) {
        return NULL;
    }

    // 随机选择一个效果
    int randomIndex = rand() % validCount;
    return validEffects[randomIndex];
}

int get_element_food_count(const ElementManager* manager) {
    if (manager == NULL) {
        return 0;
    }

    return manager->count;
}

bool is_element_food_max_reached(const ElementManager* manager) {
    if (manager == NULL) {
        return true;
    }

    return manager->count >= manager->maxCount;
}

const char* get_element_type_name(ElementType element) {
    switch (element) {
        case ELEMENT_WIND: return "风";
        case ELEMENT_FIRE: return "火";
        case ELEMENT_ELECTRIC: return "电";
        case ELEMENT_ICE: return "冰";
        case ELEMENT_WATER: return "水";
        default: return "无";
    }
}

const char* get_reaction_type_name(ElementReactionType reaction) {
    switch (reaction) {
        case REACTION_EVAPORATE: return "蒸发";
        case REACTION_MELT: return "融化";
        case REACTION_FREEZE: return "冻结";
        case REACTION_OVERLOAD: return "超载";
        case REACTION_SUPERCONDUCT: return "超导";
        case REACTION_ELECTRO_CHARGED: return "感电";
        case REACTION_SWIRL: return "扩散";
        default: return "无反应";
    }
}