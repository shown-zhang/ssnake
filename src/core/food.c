#include "core/food.h"
#include "core/snake.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void init_food_manager(FoodManager *manager, int maxCount) {
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

void cleanup_food_manager(FoodManager *manager) {
  if (manager == NULL) {
    return;
  }

  // 遍历链表并释放所有食物
  KNode *node, *tmp;
  knode_for_each_safe(node, tmp, &manager->head) {
    Food *food = container_of(node, Food, node);
    knode_del(node);
    FREE(food);
  }

  manager->count = 0;
}

bool generate_food(FoodManager *manager, int gridWidth, int gridHeight,
                   const void *snake) {
  if (manager == NULL || manager->count >= manager->maxCount) {
    return false;
  }

  const Snake *snakePtr = (const Snake *)snake;

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
      Food *food = (Food *)MALLOC(sizeof(Food));
      if (food == NULL) {
        return false;
      }

      // 手动初始化节点，避免宏中的return语句
      food->node.next = &food->node;
      food->node.prev = &food->node;
      food->x = x;
      food->y = y;
      food->value = 1; // 默认每个食物得1分
      
      // 初始化新增字段：默认生成普通食物
      food->elementType = 0;    // 普通食物
      food->elementLevel = 0;   // 默认级别
      food->subElementType = 0;  // 默认子类型

      // 添加到链表
      knode_add(&food->node, &manager->head);
      manager->count++;

      return true;
    }

    attempts++;
  }

  return false;
}

Food *check_food_at_position(const FoodManager *manager, int x, int y) {
  if (manager == NULL) {
    return NULL;
  }

  KNode *node;
  knode_for_each(node, &manager->head) {
    Food *food = container_of(node, Food, node);
    if (food->x == x && food->y == y) {
      return food;
    }
  }

  return NULL;
}

int remove_food(FoodManager *manager, Food *food) {
  if (manager == NULL || food == NULL) {
    return 0;
  }

  int value = food->value;

  // 如果是特殊食物，输出相关信息
  if (is_food_special(food)) {
    printf("移除特殊食物: 类型=%d, 级别=%d, 子类型=%d, 价值=%d\n", 
           food->elementType, food->elementLevel, food->subElementType, value);
  }

  // 从链表中移除
  knode_del(&food->node);
  FREE(food);
  manager->count--;

  return value;
}

int get_food_count(const FoodManager *manager) {
  if (manager == NULL) {
    return 0;
  }

  return manager->count;
}

bool is_food_max_reached(const FoodManager *manager) {
  if (manager == NULL) {
    return true;
  }

  return manager->count >= manager->maxCount;
}

/**
 * @brief 设置食物的特殊属性
 * @param food 食物指针
 * @param elementType 元素类型（0=普通食物，其他=特殊食物）
 * @param elementLevel 元素级别（数值越大越稀有）
 * @param subElementType 子元素类型（具体增益效果的ID）
 */
void set_food_special_attributes(Food *food, int elementType, int elementLevel, int subElementType) {
    if (food == NULL) {
        return;
    }
    
    food->elementType = elementType;
    food->elementLevel = elementLevel;
    food->subElementType = subElementType;
    
    // 根据元素类型和级别调整食物价值
    if (elementType == 0) {
        // 普通食物，价值为1
        food->value = 1;
    } else {
        // 特殊食物，价值 = 基础值 + 级别 * 2
        food->value = 1 + elementLevel * 2;
    }
}

/**
 * @brief 获取食物的元素类型
 * @param food 食物指针
 * @return 元素类型
 */
int get_food_element_type(const Food *food) {
    if (food == NULL) {
        return 0;
    }
    return food->elementType;
}

/**
 * @brief 获取食物的元素级别
 * @param food 食物指针
 * @return 元素级别
 */
int get_food_element_level(const Food *food) {
    if (food == NULL) {
        return 0;
    }
    return food->elementLevel;
}

/**
 * @brief 获取食物的子元素类型
 * @param food 食物指针
 * @return 子元素类型
 */
int get_food_sub_element_type(const Food *food) {
    if (food == NULL) {
        return 0;
    }
    return food->subElementType;
}

/**
 * @brief 检查食物是否为特殊食物
 * @param food 食物指针
 * @return 如果是特殊食物返回true，否则返回false
 */
bool is_food_special(const Food *food) {
    if (food == NULL) {
        return false;
    }
    return food->elementType != 0;
}

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
                           const void *snake, int elementType, int elementLevel, int subElementType) {
    if (manager == NULL || manager->count >= manager->maxCount) {
        return false;
    }

    const Snake *snakePtr = (const Snake *)snake;

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
            Food *food = (Food *)MALLOC(sizeof(Food));
            if (food == NULL) {
                return false;
            }

            // 手动初始化节点，避免宏中的return语句
            food->node.next = &food->node;
            food->node.prev = &food->node;
            food->x = x;
            food->y = y;
            
            // 设置特殊属性
            set_food_special_attributes(food, elementType, elementLevel, subElementType);

            // 添加到链表
            knode_add(&food->node, &manager->head);
            manager->count++;

            return true;
        }

        attempts++;
    }

    return false;
}