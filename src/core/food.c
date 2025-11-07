#include "core/food.h"
#include "core/snake.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <time.h>

void init_food_manager(FoodManager* manager, int maxCount) {
    if (manager == NULL) {
        return;
    }
    
    // 初始化链表头
    knode_init(&manager->head);
    manager->count = 0;
    manager->maxCount = maxCount;
    
    // 初始化随机数种子
    srand((unsigned int)time(NULL));
}

void cleanup_food_manager(FoodManager* manager) {
    if (manager == NULL) {
        return;
    }
    
    // 遍历链表并释放所有食物
    KNode *node, *tmp;
    knode_for_each_safe(node, tmp, &manager->head) {
        Food* food = container_of(node, Food, node);
        knode_del(node);
        FREE(food);
    }
    
    manager->count = 0;
}

bool generate_food(FoodManager* manager, int gridWidth, int gridHeight, const void* snake) {
    if (manager == NULL || manager->count >= manager->maxCount) {
        return false;
    }
    
    const Snake* snakePtr = (const Snake*)snake;
    
    // 尝试生成食物的最大尝试次数
    const int maxAttempts = 100;
    int attempts = 0;
    
    while (attempts < maxAttempts) {
        // 生成随机位置
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;
        
        // 检查位置是否有效（不在蛇身上，且没有其他食物）
        bool positionValid = true;
        
        // 检查是否在蛇身上
        if (snakePtr != NULL && check_snake_collision(snakePtr, x, y)) {
            positionValid = false;
        }
        
        // 检查是否已有食物
        if (positionValid && check_food_at_position(manager, x, y) != NULL) {
            positionValid = false;
        }
        
        if (positionValid) {
            // 创建新食物
            Food* food = (Food*)MALLOC(sizeof(Food));
            if (food == NULL) {
                return false;
            }
            
            // 手动初始化节点，避免宏中的return语句
            food->node.next = &food->node;
            food->node.prev = &food->node;
            food->x = x;
            food->y = y;
            food->value = 1; // 默认每个食物得1分
            
            // 添加到链表
            knode_add(&food->node, &manager->head);
            manager->count++;
            
            return true;
        }
        
        attempts++;
    }
    
    return false;
}

Food* check_food_at_position(const FoodManager* manager, int x, int y) {
    if (manager == NULL) {
        return NULL;
    }
    
    KNode* node;
    knode_for_each(node, &manager->head) {
        Food* food = container_of(node, Food, node);
        if (food->x == x && food->y == y) {
            return food;
        }
    }
    
    return NULL;
}

int remove_food(FoodManager* manager, Food* food) {
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

int get_food_count(const FoodManager* manager) {
    if (manager == NULL) {
        return 0;
    }
    
    return manager->count;
}

bool is_food_max_reached(const FoodManager* manager) {
    if (manager == NULL) {
        return true;
    }
    
    return manager->count >= manager->maxCount;
}