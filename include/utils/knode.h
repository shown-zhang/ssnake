#pragma once

#include <stddef.h>

/**
    @brief 链表节点
*/
typedef struct KNode {
  struct KNode *next; // 指向下一个节点的指针
  struct KNode *prev; // 指向上一个节点的指针
} KNode;

#define offset_of(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offset_of(type, member));                        \
  })

// 节点初始化
#define knode_init(node)                                                       \
  do {                                                                         \
    if (node == NULL) {                                                        \
      return;                                                                  \
    }                                                                          \
    (node)->next = (node);                                                     \
    (node)->prev = (node);                                                     \
  } while (0)

// 在两个节点之间插入新节点
static inline void knode_insert_between(KNode *node, KNode *prev, KNode *next) {
  node->next = next;
  node->prev = prev;
  prev->next = node;
  next->prev = node;
}

// 在链表头添加节点
static inline void knode_add(KNode *node, KNode *head) {
  knode_insert_between(node, head, head->next);
}

// 在链表尾添加节点
static inline void knode_add_tail(KNode *node, KNode *head) {
  knode_insert_between(node, head->prev, head);
}

// 删除节点
static inline void __knode_del(KNode *prev, KNode *next) {
  prev->next = next;
  next->prev = prev;
}

// 删除节点并初始化
static inline void knode_del(KNode *node) {
  __knode_del(node->prev, node->next);
  knode_init(node);
}

// 判断节点是否是第一个有意义节点
static inline int knode_is_first(const KNode *node, const KNode *head) {
  return node->prev == head;
}

// 判断节点是否是尾节点
static inline int knode_is_last(const KNode *node, const KNode *head) {
  return node->next == head;
}

// 判断节点是否是头节点
static inline int knode_is_head(const KNode *node, const KNode *head) {
  return node == head;
}

// 判断链表是否为空
static inline int knode_empty(const KNode *head) { return head->next == head; }

// 遍历链表
#define knode_for_each(node, head)                                             \
  for (node = (head)->next; !knode_is_head(node, head); node = node->next)

// 遍历链表反向遍历
#define knode_for_each_reverse(node, head)                                     \
  for (node = (head)->prev; !knode_is_head(node, head); node = node->prev)

// 遍历链表并安全删除节点
#define knode_for_each_safe(node, tmp, head)                                   \
  for (node = (head)->next, tmp = node->next; !knode_is_head(node, head);      \
       node = tmp, tmp = node->next)
