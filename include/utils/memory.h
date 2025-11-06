/**
  在此文件中定义内存分配以及释放函数
*/

#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief 分配内存
 *
 * @param size 内存大小
 * @return void* 内存指针
 */
void *malloc_or_die(size_t size);

/**
 * @brief 释放内存
 *
 * @param ptr 内存指针
 */
void free_or_die(void *ptr);

/**
 * @brief 重新分配内存
 *
 * @param ptr 内存指针
 * @param size 新内存大小
 * @return void* 新内存指针
 */
void *realloc_or_die(void *ptr, size_t size);

/**
 * @brief 复制字符串
 *
 * @param str 源字符串
 * @return char* 新字符串指针
 */
char *strdup_or_die(const char *str);

// 内存分配宏定义

/**
 * @brief 安全分配内存，失败时退出程序
 */
#define MALLOC(size) malloc_or_die(size)

/**
 * @brief 安全释放内存
 */
#define FREE(ptr)                                                              \
  do {                                                                         \
    free_or_die(ptr);                                                          \
    (ptr) = NULL;                                                              \
  } while (0)

/**
 * @brief 类型安全的分配宏
 */
#define NEW(type) ((type *)MALLOC(sizeof(type)))

/**
 * @brief 类型安全的数组分配宏
 */
#define NEW_ARRAY(type, count) ((type *)MALLOC(sizeof(type) * (count)))

/**
 * @brief 分配并初始化为0
 */
#define CALLOC(size)                                                           \
  ({                                                                           \
    void *ptr = MALLOC(size);                                                  \
    memset(ptr, 0, size);                                                      \
    ptr;                                                                       \
  })

/**
 * @brief 类型安全的分配并初始化为0
 */
#define NEW_ZEROED(type) ((type *)CALLOC(sizeof(type)))

/**
 * @brief 类型安全的数组分配并初始化为0
 */
#define NEW_ARRAY_ZEROED(type, count) ((type *)CALLOC(sizeof(type) * (count)))

/**
 * @brief 重新分配内存
 */
#define REALLOC(ptr, size) realloc_or_die(ptr, size)

/**
 * @brief 复制字符串
 */
#define STRDUP(str) strdup_or_die(str)

#endif
