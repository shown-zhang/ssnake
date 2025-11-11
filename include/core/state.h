#pragma once

#include "utils/knode.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#else
// Unix-like系统的替代实现
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#endif

#define GRID_WIDTH 13
#define GRID_HEIGHT 13

/**
 * @brief 网格类型
 */
typedef enum {
  NOTHING = 0, /**< 空网格 */
  SNAKE = 1,   /**< 蛇网格 */
  ENEMY = 2,   /**< 敌人网格 */
  FOOD = 3,    /**< 食物网格 */
  ELEMENT = 4, /**< 元素网格 */
} GridTypeEnum;

const char grid_type_char[5] = {'*', '#', 'B', 'F', 'E'};
typedef enum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
} DirectionEnum;

typedef struct {
  GridTypeEnum type; /**< 网格类型 */
} Grid;

typedef struct SnakeNode {
  int is_head;
  int index;
  KNode node; /**< 指向下一个节点的指针 */
} SnakeNode;

typedef struct {
  int is_over; /**< 游戏是否结束 */
  int score;   /**< 游戏得分 */
  Grid grid[GRID_WIDTH * GRID_HEIGHT];
  KNode snakeHead;         /**< 蛇头节点指针 */
  DirectionEnum direction; /**< 蛇头方向 */
  KNode foodHead;          /**< 食物节点指针 */
} GameState;

// 初始化游戏状态
void init_game_state(GameState *state) {
  state->is_over = 0;
  state->score = 0;
  state->direction = RIGHT;
  for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
    state->grid[i].type = NOTHING;
  }
}

// 实现一个打印函数，每个字符等距打印
void print_game_state(GameState *state) {
  // 使用Windows API清屏（兼容Windows环境）
#ifdef _WIN32
  system("cls");
#else
  // 对于非Windows系统，使用ANSI转义码
  printf("\033[H");
  printf("\033[2J");
#endif

  // 打印游戏状态网格，使用全角字符保证正方形显示
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      int index = y * GRID_WIDTH + x;
      printf("%c ", grid_type_char[state->grid[index].type]);
    }
    printf("\n");
  }

  // 刷新输出缓冲区，确保立即显示
  fflush(stdout);
}

void create_snake_node(SnakeNode **snake_node, int index, int is_head) {
  *snake_node = (SnakeNode *)malloc(sizeof(SnakeNode));
  if (*snake_node == NULL) {
    printf("Error: Failed to allocate memory for snake node.\n");
    FREE(*snake_node);
    return;
  }

  (*snake_node)->index = index;
  (*snake_node)->is_head = is_head;

  // 初始化KNode
  knode_init(&(*snake_node)->node);
}

// 初始化蛇，使用knode链表实现
void init_snake(GameState *state) {
  // 初始化蛇头链表
  knode_init(&state->snakeHead);

  // 计算网格中心位置
  int center_x = GRID_WIDTH / 2;
  int center_y = GRID_HEIGHT / 2;

  // 分配蛇节点内存
  SnakeNode *snake_node = NULL;
  create_snake_node(&snake_node, center_x * GRID_WIDTH + center_y, 1);
  if (snake_node == NULL) {
    return;
  }

  // 将蛇节点添加到链表
  knode_add(&snake_node->node, &state->snakeHead);
}

void clean_snake(GameState *state) {
  // 检查链表是否为空
  if (knode_empty(&state->snakeHead)) {
    return;
  }

  // 使用knode_for_each_safe安全遍历并清理链表
  KNode *node, *tmp;
  knode_for_each_safe(node, tmp, &state->snakeHead) {
    // 使用container_of宏获取包含KNode的SnakeNode结构体
    SnakeNode *snake_node = container_of(node, SnakeNode, node);

    // 从链表中删除节点
    knode_del(node);

    // 释放SnakeNode内存
    free(snake_node);
  }

  // 重新初始化蛇头链表
  knode_init(&state->snakeHead);
}

void update_grid(GameState *state) {
  // 先将所有网格类型设置为NOTHING
  for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
    state->grid[i].type = NOTHING;
  }

  // 遍历蛇身节点，更新网格类型
  KNode *node;
  knode_for_each(node, &state->snakeHead) {
    SnakeNode *snake_node = container_of(node, SnakeNode, node);
    int index = snake_node->index;
    if (index >= 0 && index < GRID_WIDTH * GRID_HEIGHT) {
      state->grid[index].type = SNAKE;
    }
  }
}

void check_collision(GameState *state, int new_index) {
  // 3.1 检查新索引是否超出边界
  if (new_index < 0 || new_index >= GRID_WIDTH * GRID_HEIGHT) {
    printf("Error: Snake moved out of bounds at index %d.\n", new_index);
    state->is_over = 1;
    return;
  }
  // 1. 获取头结点
  SnakeNode *head = container_of(state->snakeHead.next, SnakeNode, node);
  // 2. 检查头结点是否与其他节点冲突
  KNode *node;
  knode_for_each(node, &state->snakeHead) {
    if (node->next == &state->snakeHead) {
      break;
    }

    SnakeNode *snake_node = container_of(node->next, SnakeNode, node);
    if (snake_node->index == head->index) {
      printf("Error: Snake collided with itself at index %d.\n", head->index);
      state->is_over = 1;
      return;
    }
  }

  // 3.2 检查蛇头是否撞到网格边界
  int head_x = head->index % GRID_WIDTH;
  int head_y = head->index / GRID_WIDTH;
  int new_x = new_index % GRID_WIDTH;
  int new_y = new_index / GRID_WIDTH;

  // 检查是否从左边边界移到右边边界（或相反）
  if (abs(new_x - head_x) > 1) {
    printf("Error: Snake hit horizontal boundary at index %d.\n", new_index);
    state->is_over = 1;
    return;
  }

  // 检查是否从顶部边界移到底部边界（或相反）
  if (abs(new_y - head_y) > 1) {
    printf("Error: Snake hit vertical boundary at index %d.\n", new_index);
    state->is_over = 1;
    return;
  }
}

void move_snake(GameState *state) {
  SnakeNode *head = container_of(state->snakeHead.next, SnakeNode, node);

  int new_index = head->index;
  switch (state->direction) {
  case UP:
    new_index -= GRID_WIDTH;
    break;
  case DOWN:
    new_index += GRID_WIDTH;
    break;
  case LEFT:
    new_index -= 1;
    break;
  case RIGHT:
    new_index += 1;
    break;
  default:
    break;
  }

  check_collision(state, new_index);
  if (state->is_over) {
    return;
  }

  // 4. 将新头结点
  SnakeNode *new_head = NULL;
  create_snake_node(&new_head, new_index, 1);
  if (new_head == NULL) {
    printf("Error: Failed to create new snake head.\n");
    state->is_over = 1;
    return;
  }
  knode_add(&new_head->node, &state->snakeHead);

  // 5.
  // 依次遍历链表，将每个节点更新为其下一个节点（复制所有的字段）
  // index也需要复制
  KNode *node;
  knode_for_each(node, &state->snakeHead) {
    if (knode_is_last(node, &state->snakeHead)) {
      break;
    }
    SnakeNode *snake_node = container_of(node, SnakeNode, node);
    SnakeNode *next_snake_node = container_of(node->next, SnakeNode, node);
    snake_node->is_head = next_snake_node->is_head;
  }

  // 6. 移除链表最后一个节点（尾结点）
  KNode *tail = state->snakeHead.prev;
  if (!knode_is_head(tail, &state->snakeHead)) {
    knode_del(tail);
    SnakeNode *snake_tail = container_of(tail, SnakeNode, node);
    FREE(snake_tail);
  }
}

// 监听用户输入
void listen_input(GameState *state) {
  if (kbhit()) {
    int ch = getchar();
    switch (ch) {
    case 'w':
      if (state->direction != DOWN) {
        state->direction = UP;
      }
      break;
    case 's':
      if (state->direction != UP) {
        state->direction = DOWN;
      }
      break;
    case 'a':
      if (state->direction != RIGHT) {
        state->direction = LEFT;
      }
      break;
    case 'd':
      if (state->direction != LEFT) {
        state->direction = RIGHT;
      }
      break;
    default:
      break;
    }
  }
}

// 打印链表
void print_snake(GameState *state) {
  KNode *node;
  knode_for_each(node, &state->snakeHead) {
    SnakeNode *snake_node = container_of(node, SnakeNode, node);
    printf("index: %d, is_head: %d\n", snake_node->index, snake_node->is_head);

    if (knode_is_last(node, &state->snakeHead)) {
      break;
    }
  }
}
