#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool push_back(Array *arr, uint64_t item) {
  if (arr->top >= arr->size) {
    if(arr->size < 8) {
      arr->size = 4;
    }
    size_t new_size = arr->size * 2;

    void *ptr = realloc(arr->items, new_size * sizeof(uint64_t));
    if (ptr == NULL) {
      return false;
    }

    arr->items = ptr;
    arr->size = new_size;
  }

  arr->items[arr->top++] = item;
  return true;
}

void *get(Array *arr, size_t i) {
  if (i >= arr->top) {
    return NULL;
  }
  return &arr->items[i];
}
