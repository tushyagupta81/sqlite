#ifndef MEM_H
#define MEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t *items;
  size_t size;
  size_t top;
} Array;

bool push_back(Array *arr, uint64_t item);
void *get(Array *arr, size_t i);
void init_arr(Array * arr);

#endif // !MEM_H
