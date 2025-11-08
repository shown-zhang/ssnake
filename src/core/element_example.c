#include "core/element.h"
#include "core/snake.h"
#include <stdio.h>
#include <string.h>

// 示例：演示元素系统的使用
void element_system_example() {
    printf("=== 元素系统使用示例 ===\n");
    
    // 1. 初始化元素管理器
    ElementManager elementManager;
    init_element_manager(&elementManager, 10);
    
    // 2. 加载配置文件（可选）
    load_element_effects_from_config(&elementManager, "assets/element_effects.conf");
    load_element_reactions_from_config(&elementManager, "assets/element_reactions.conf");
    
    // 3. 演示元素反应
    printf("\n元素反应演示:\n");
    const ElementReaction* reaction;
    
    // 火 + 水 = 蒸发
    reaction = get_element_reaction(&elementManager, ELEMENT_FIRE, ELEMENT_WATER);
    if (reaction) {
        printf("火 + 水 = %s (伤害: %d, 持续时间: %d帧)\n", 
               get_reaction_type_name(reaction->reaction), 
               reaction->damage, 
               reaction->duration);
    }
    
    // 冰 + 电 = 超导
    reaction = get_element_reaction(&elementManager, ELEMENT_ICE, ELEMENT_ELECTRIC);
    if (reaction) {
        printf("冰 + 电 = %s (伤害: %d, 持续时间: %d帧)\n", 
               get_reaction_type_name(reaction->reaction), 
               reaction->damage, 
               reaction->duration);
    }
    
    // 4. 演示元素效果
    printf("\n元素效果演示:\n");
    const ElementEffect* effect = get_random_element_effect(&elementManager, ELEMENT_FIRE);
    if (effect) {
        printf("随机火元素效果: %s (持续时间: %d帧, 强度: %d)\n", 
               effect->name, effect->duration, effect->power);
    }
    
    // 5. 清理资源
    cleanup_element_manager(&elementManager);
    
    printf("\n=== 示例结束 ===\n");
}

// 贪吃蛇元素收集系统示例
typedef struct {
    ElementFood* collectedFoods[100];  // 收集的元素食物
    int foodCount;                     // 收集的食物数量
    ElementType activeElements[10];    // 当前激活的元素
    int elementCount;                  // 激活元素数量
} SnakeElementSystem;

void init_snake_element_system(SnakeElementSystem* system) {
    if (system == NULL) return;
    system->foodCount = 0;
    system->elementCount = 0;
}

void collect_element_food(SnakeElementSystem* system, ElementFood* food) {
    if (system == NULL || food == NULL || system->foodCount >= 100) return;
    
    system->collectedFoods[system->foodCount] = food;
    system->foodCount++;
    
    // 添加元素到激活列表
    if (system->elementCount < 10) {
        system->activeElements[system->elementCount] = food->element;
        system->elementCount++;
    }
    
    printf("收集到%s元素食物: %s\n", 
           get_element_type_name(food->element), 
           food->effect.name);
}

void trigger_element_reactions(SnakeElementSystem* system, ElementManager* elementManager) {
    if (system == NULL || elementManager == NULL) return;
    
    // 检查最近两个元素是否会产生反应
    if (system->elementCount >= 2) {
        ElementType elem1 = system->activeElements[system->elementCount - 2];
        ElementType elem2 = system->activeElements[system->elementCount - 1];
        
        const ElementReaction* reaction = get_element_reaction(elementManager, elem1, elem2);
        if (reaction && reaction->reaction != REACTION_NONE) {
            printf("触发元素反应: %s + %s = %s!\n", 
                   get_element_type_name(elem1),
                   get_element_type_name(elem2),
                   get_reaction_type_name(reaction->reaction));
            
            // 应用反应效果（这里可以扩展为具体的游戏效果）
            printf("反应效果: 伤害%d, 持续%d帧\n", reaction->damage, reaction->duration);
            
            // 移除已反应的元素
            system->elementCount -= 2;
        }
    }
}

// 主函数示例
int main_example() {
    printf("贪吃蛇元素系统集成示例\n");
    
    // 演示基础功能
    element_system_example();
    
    // 演示贪吃蛇集成
    printf("\n=== 贪吃蛇元素收集系统演示 ===\n");
    
    SnakeElementSystem snakeSystem;
    init_snake_element_system(&snakeSystem);
    
    ElementManager elementManager;
    init_element_manager(&elementManager, 5);
    
    // 模拟收集元素食物
    ElementFood testFood1, testFood2;
    testFood1.element = ELEMENT_FIRE;
    strcpy(testFood1.effect.name, "火焰强化");
    testFood1.effect.duration = 250;
    testFood1.effect.power = 7;
    
    testFood2.element = ELEMENT_WATER;
    strcpy(testFood2.effect.name, "水流治愈");
    testFood2.effect.duration = 350;
    testFood2.effect.power = 3;
    
    collect_element_food(&snakeSystem, &testFood1);
    collect_element_food(&snakeSystem, &testFood2);
    
    // 触发元素反应
    trigger_element_reactions(&snakeSystem, &elementManager);
    
    cleanup_element_manager(&elementManager);
    
    return 0;
}