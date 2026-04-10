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
