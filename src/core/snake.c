#include "core/snake.h"
#include "core/state.h"
#include "utils/memory.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

void init_snake(Snake *snake, int startX, int startY, int initialLength) {
  if (snake == NULL) {
    return;
  }

  // 初始化链表头
  knode_init(&snake->head);
  snake->length = 0;
  snake->isAlive = true;

  // 创建初始蛇身
  for (int i = 0; i < initialLength; i++) {
    SnakeSegment *segment = (SnakeSegment *)MALLOC(sizeof(SnakeSegment));
    if (segment == NULL) {
      continue;
    }

    // 手动初始化链表节点，避免宏中的return语句
    segment->node.next = &segment->node;
    segment->node.prev = &segment->node;

    // 设置位置（蛇头在起始位置，身体向后延伸）
    // 注意：i=0应该是蛇头，i=1,2...应该是蛇身
    segment->x = startX - i;
    segment->y = startY;

    // 初始化新增字段
    segment->elementType = 0;    // 普通元素
    segment->elementLevel = 0;   // 默认级别
    segment->subElementType = 0; // 默认子类型

    // 添加到链表尾部，确保第一个创建的段(i=0)是蛇头
    // 使用knode_add_tail而不是knode_add
    knode_add_tail(&segment->node, &snake->head);
    snake->length++;
  }
}

void cleanup_snake(Snake *snake) {
  if (snake == NULL) {
    return;
  }

  // 遍历链表并释放所有节点
  KNode *node, *tmp;
  knode_for_each_safe(node, tmp, &snake->head) {
    SnakeSegment *segment = container_of(node, SnakeSegment, node);
    knode_del(node);
    FREE(segment);
  }

  snake->length = 0;
  snake->isAlive = false;
}

bool move_snake(Snake *snake, int direction, int gridWidth, int gridHeight,
                bool shouldGrow) {
  if (snake == NULL || !snake->isAlive) {
    return false;
  }

  // 获取蛇头位置
  int headX, headY;
  get_snake_head(snake, &headX, &headY);

  // 计算新的蛇头位置
  int newHeadX = headX;
  int newHeadY = headY;

  switch (direction) {
  case DIRECTION_UP:
    newHeadY--;
    break;
  case DIRECTION_DOWN:
    newHeadY++;
    break;
  case DIRECTION_LEFT:
    newHeadX--;
    break;
  case DIRECTION_RIGHT:
    newHeadX++;
    break;
  default:
    return false;
  }

  // 检查边界碰撞
  if (newHeadX < 0 || newHeadX >= gridWidth || newHeadY < 0 ||
      newHeadY >= gridHeight) {
    snake->isAlive = false;
    return false;
  }

  // 检查自身碰撞
  if (check_snake_collision(snake, newHeadX, newHeadY)) {
    snake->isAlive = false;
    return false;
  }

  // 移动蛇：添加新蛇头
  SnakeSegment *newHead = (SnakeSegment *)MALLOC(sizeof(SnakeSegment));
  if (newHead == NULL) {
    return false;
  }

  // 手动初始化节点，避免宏中的return语句
  newHead->node.next = &newHead->node;
  newHead->node.prev = &newHead->node;
  newHead->x = newHeadX;
  newHead->y = newHeadY;

  // 初始化新增字段
  newHead->elementType = 0;    // 普通元素
  newHead->elementLevel = 0;   // 默认级别
  newHead->subElementType = 0; // 默认子类型

  // 添加到链表头部
  knode_add(&newHead->node, &snake->head);

  // 如果不增长，移除蛇尾；如果增长，保持蛇尾（长度增加）
  if (!shouldGrow) {
    KNode *tail = snake->head.prev;
    SnakeSegment *tailSegment = container_of(tail, SnakeSegment, node);
    knode_del(tail);
    FREE(tailSegment);
  } else {
    // 增长时，蛇的长度增加
    snake->length++;
  }

  return true;
}

bool check_snake_collision(const Snake *snake, int x, int y) {
  if (snake == NULL) {
    return false;
  }

  KNode *node;
  knode_for_each(node, &snake->head) {
    SnakeSegment *segment = container_of(node, SnakeSegment, node);
    if (segment->x == x && segment->y == y) {
      return true;
    }
  }

  return false;
}

bool check_self_collision(const Snake *snake) {
  if (snake == NULL || snake->length <= 1) {
    return false;
  }

  int headX, headY;
  get_snake_head(snake, &headX, &headY);

  // 从第二个节点开始检查（跳过蛇头）
  KNode *node = snake->head.next->next;
  while (!knode_is_head(node, &snake->head)) {
    SnakeSegment *segment = container_of(node, SnakeSegment, node);
    if (segment->x == headX && segment->y == headY) {
      return true;
    }
    node = node->next;
  }

  return false;
}

void get_snake_head(const Snake *snake, int *x, int *y) {
  if (snake == NULL || x == NULL || y == NULL || snake->length == 0) {
    return;
  }

  // 蛇头是链表中的第一个节点
  KNode *headNode = snake->head.next;
  SnakeSegment *headSegment = container_of(headNode, SnakeSegment, node);

  *x = headSegment->x;
  *y = headSegment->y;
}

void get_snake_tail(const Snake *snake, int *x, int *y) {
  if (snake == NULL || x == NULL || y == NULL || snake->length == 0) {
    return;
  }

  // 蛇尾是链表中的最后一个节点
  KNode *tailNode = snake->head.prev;
  SnakeSegment *tailSegment = container_of(tailNode, SnakeSegment, node);

  *x = tailSegment->x;
  *y = tailSegment->y;
}