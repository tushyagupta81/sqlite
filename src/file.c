#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

void * read_n_bytes(FILE* fileptr, size_t size){
  char *buffer = malloc(sizeof(char) * size);
  if (buffer) {
    if (fread(buffer, size, 1, fileptr) != 1) {
      free(buffer);
      buffer = NULL;
    }
  }
  return buffer;
}

uint64_t read_big_endian(const uint8_t *data, size_t num){
  uint64_t res = 0;
  int i = 0;
  while(num > 0){
    res = (res << 8) | data[i];
    i++;
    num--;
  }

  return res;
}
