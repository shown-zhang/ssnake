#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *malloc_or_die(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "内存分配失败，大小: %zu\n", size);
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void free_or_die(void *ptr) {
  if (ptr) {
    free(ptr);
  }
}

void *realloc_or_die(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (!new_ptr && size > 0) {
    fprintf(stderr, "内存重新分配失败，大小: %zu\n", size);
    exit(EXIT_FAILURE);
  }
  return new_ptr;
}

char *strdup_or_die(const char *str) {
  if (!str) {
    fprintf(stderr, "strdup失败: 空指针\n");
    exit(EXIT_FAILURE);
  }

  size_t len = strlen(str) + 1;
  char *new_str = MALLOC(len);
  strcpy(new_str, str);
  return new_str;
}