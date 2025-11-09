#include "core/food.h"
#include "core/snake.h"
#include "core/food_config.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// 全局食物配置管理器
static FoodConfigManager g_foodConfigManager;
static bool g_foodConfigInitialized = false;

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

  // 清理所有食物
  KNode *node, *tmp;
  knode_for_each_safe(node, tmp, &manager->head) {
    Food *food = container_of(node, Food, node);
    knode_del(node);
    FREE(food);
  }

  manager->count = 0;
  
  // 清理食物配置管理器
  if (g_foodConfigInitialized) {
    cleanup_food_config_manager(&g_foodConfigManager);
    g_foodConfigInitialized = false;
  }
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
      
      // 初始化食物配置管理器（如果未初始化）
      if (!g_foodConfigInitialized) {
          g_foodConfigInitialized = init_food_config_manager(&g_foodConfigManager, "assets/config/food.ini");
          if (!g_foodConfigInitialized) {
              printf("警告：无法初始化食物配置管理器，使用默认配置\n");
          }
      }
      
      // 50%概率生成普通食物，50%概率生成元素食物
      bool isElementFood = (rand() % 2) == 1; // 50%概率
      
      if (g_foodConfigInitialized) {
          // 使用配置管理器生成食物配置
          const FoodConfig* config = generate_random_food_config(&g_foodConfigManager, isElementFood);
          if (config != NULL) {
              food->elementType = config->elementType;
              food->elementLevel = config->elementLevel;
              food->subElementType = config->subElementType;
              
              // 根据配置设置食物价值
              if (config->elementType == 0) {
                  food->value = 1; // 普通食物
              } else {
                  food->value = 1 + config->elementLevel * 2; // 特殊食物
              }
              
              printf("生成食物: %s (类型=%d, 级别=%d, 子类型=%d, 价值=%d)\n", 
                     config->name, config->elementType, config->elementLevel, 
                     config->subElementType, food->value);
          } else {
              // 配置获取失败，使用默认值
              food->elementType = isElementFood ? 1 : 0;
              food->elementLevel = isElementFood ? 1 : 0;
              food->subElementType = isElementFood ? 1 : 0;
              food->value = isElementFood ? 3 : 1;
          }
      } else {
          // 配置管理器未初始化，使用简单逻辑
          food->elementType = isElementFood ? 1 : 0;
          food->elementLevel = isElementFood ? 1 : 0;
          food->subElementType = isElementFood ? 1 : 0;
          food->value = isElementFood ? 3 : 1;
      }

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

FoodColor get_food_color(const Food *food) {
    FoodColor color = {1.0f, 1.0f, 1.0f, 1.0f}; // 默认白色
    
    if (food == NULL) {
        return color;
    }
    
    // 如果是普通食物，使用默认颜色
    if (food->elementType == 0) {
        color.r = 1.0f; // 红色
        color.g = 0.0f; // 绿色
        color.b = 0.0f; // 蓝色
        return color;
    }
    
    // 如果是特殊食物，获取元素种类的基础颜色（边框颜色）
    return get_base_element_color(food->elementType, food->elementLevel);
}

FoodColor get_base_element_color(int elementType, int elementLevel) {
    FoodColor color = {1.0f, 1.0f, 1.0f, 1.0f}; // 默认白色
    
    // 从INI配置获取基础颜色
    if (g_foodConfigInitialized) {
        float r, g, b;
        get_element_base_color(&g_foodConfigManager, elementType, &r, &g, &b);
        color.r = r;
        color.g = g;
        color.b = b;
    } else {
        // 如果配置未初始化，使用硬编码颜色作为后备
        switch (elementType) {
            case 1: // 水元素
                color.r = 0.0f;
                color.g = 0.5f;
                color.b = 1.0f;
                break;
            case 2: // 冰元素
                color.r = 0.5f;
                color.g = 0.8f;
                color.b = 1.0f;
                break;
            case 3: // 火元素
                color.r = 1.0f;
                color.g = 0.3f;
                color.b = 0.0f;
                break;
            case 4: // 电元素
                color.r = 1.0f;
                color.g = 1.0f;
                color.b = 0.0f;
                break;
            case 5: // 风元素
                color.r = 0.7f;
                color.g = 1.0f;
                color.b = 0.7f;
                break;
            default: // 普通食物
                color.r = 1.0f;
                color.g = 0.0f;
                color.b = 0.0f;
                break;
        }
    }
    
    // 根据级别调整亮度（级别越高越亮）
    float brightness = 0.5f + (elementLevel * 0.1f);
    if (brightness > 1.0f) brightness = 1.0f;
    
    color.r *= brightness;
    color.g *= brightness;
    color.b *= brightness;
    
    return color;
}

FoodColor get_detailed_food_color(int elementType, int elementLevel, int subElementType) {
    FoodColor color = {1.0f, 1.0f, 1.0f, 1.0f}; // 默认白色
    
    // 从INI配置获取详细颜色
    if (g_foodConfigInitialized) {
        const FoodConfig* config = get_food_config(&g_foodConfigManager, elementType, elementLevel, subElementType);
        if (config != NULL) {
            // 使用配置中的颜色作为填充颜色
            color.r = config->r;
            color.g = config->g;
            color.b = config->b;
            
            // 对填充颜色进行显著调整，确保与边框颜色有明显区别
            // 使用互补色或对比色方案
            if (elementType > 0) {
                // 计算互补色：将RGB值取反
                color.r = 1.0f - color.r;
                color.g = 1.0f - color.g;
                color.b = 1.0f - color.b;
                
                // 根据子元素类型微调色调，增加多样性
                float hueAdjust = (subElementType % 5) * 0.1f;
                if (subElementType % 2 == 0) {
                    color.r = fminf(1.0f, color.r + hueAdjust);
                } else {
                    color.g = fminf(1.0f, color.g + hueAdjust);
                }
            }
            
            return color;
        }
    }
    
    // 如果配置获取失败，使用基础颜色并应用互补色
    color = get_base_element_color(elementType, elementLevel);
    
    // 对基础颜色应用互补色转换，确保与边框颜色有明显区别
    if (elementType > 0) {
        color.r = 1.0f - color.r;
        color.g = 1.0f - color.g;
        color.b = 1.0f - color.b;
        
        // 根据子元素类型微调
        float hueShift = (subElementType % 10) * 0.05f;
        if (subElementType % 3 == 0) {
            color.r = fminf(1.0f, color.r + hueShift);
        } else if (subElementType % 3 == 1) {
            color.g = fminf(1.0f, color.g + hueShift);
        } else {
            color.b = fminf(1.0f, color.b + hueShift);
        }
    }
    
    return color;
}

const char* get_food_effect_description(const Food *food) {
    if (food == NULL) {
        return "无效果";
    }
    
    // 普通食物
    if (food->elementType == 0) {
        return "普通食物：增加1分";
    }
    
    // 特殊食物效果描述
    switch (food->elementType) {
        case 1: // 水元素
            switch (food->subElementType % 5) {
                case 0: return "水元素：增加移动速度";
                case 1: return "水元素：增加生命值";
                case 2: return "水元素：减少移动速度";
                case 3: return "水元素：增加得分倍数";
                case 4: return "水元素：减少蛇长度";
                default: return "水元素：未知效果";
            }
        case 2: // 冰元素
            switch (food->subElementType % 5) {
                case 0: return "冰元素：冻结敌人";
                case 1: return "冰元素：增加防御力";
                case 2: return "冰元素：减速效果";
                case 3: return "冰元素：增加冰冻时间";
                case 4: return "冰元素：减少移动速度";
                default: return "冰元素：未知效果";
            }
        case 3: // 火元素
            switch (food->subElementType % 5) {
                case 0: return "火元素：增加攻击力";
                case 1: return "火元素：燃烧效果";
                case 2: return "火元素：增加移动速度";
                case 3: return "火元素：减少生命值";
                case 4: return "火元素：增加得分";
                default: return "火元素：未知效果";
            }
        case 4: // 电元素
            switch (food->subElementType % 5) {
                case 0: return "电元素：麻痹效果";
                case 1: return "电元素：增加攻击速度";
                case 2: return "电元素：连锁伤害";
                case 3: return "电元素：减少防御力";
                case 4: return "电元素：增加能量";
                default: return "电元素：未知效果";
            }
        case 5: // 风元素
            switch (food->subElementType % 5) {
                case 0: return "风元素：增加移动速度";
                case 1: return "风元素：闪避效果";
                case 2: return "风元素：减少攻击力";
                case 3: return "风元素：增加跳跃能力";
                case 4: return "风元素：减少重量";
                default: return "风元素：未知效果";
            }
        default:
            return "未知元素：未知效果";
    }
}

bool apply_food_effect(const Food *food, void *snake) {
    if (food == NULL || snake == NULL) {
        return false;
    }
    
    // 这里需要根据实际的蛇结构来实现效果应用
    // 由于不知道蛇的具体结构，这里只返回成功状态
    // 在实际应用中，这里应该修改蛇的属性
    
    printf("应用食物效果：%s (类型=%d, 级别=%d, 子类型=%d)\n", 
           get_food_effect_description(food), 
           food->elementType, food->elementLevel, food->subElementType);
    
    return true;
}

int get_food_effect_value(const Food *food) {
    if (food == NULL) {
        return 0;
    }
    
    // 普通食物
    if (food->elementType == 0) {
        return 1; // 基础得分
    }
    
    // 特殊食物的效果值基于元素类型、级别和子类型
    int baseValue = 1;
    
    // 根据元素类型调整基础值
    switch (food->elementType) {
        case 1: baseValue = 2; break; // 水元素
        case 2: baseValue = 3; break; // 冰元素
        case 3: baseValue = 4; break; // 火元素
        case 4: baseValue = 5; break; // 电元素
        case 5: baseValue = 6; break; // 风元素
        default: baseValue = 1; break;
    }
    
    // 根据级别调整（级别越高效果越强）
    baseValue += food->elementLevel;
    
    // 根据子类型微调
    baseValue += (food->subElementType % 3);
    
    return baseValue;
}