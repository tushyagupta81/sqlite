#include "file.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void *read_n_bytes(FILE *fileptr, size_t size) {
  char *buffer = malloc(sizeof(char) * size);
  if (buffer) {
    if (fread(buffer, size, 1, fileptr) != 1) {
      free(buffer);
      buffer = NULL;
    }
  }
  return buffer;
}

uint64_t read_big_endian(const uint8_t *data, size_t num) {
  uint64_t res = 0;
  int i = 0;
  while (num > 0) {
    res = (res << 8) | data[i];
    i++;
    num--;
  }

  return res;
}

int read_page(FILE *db_file, uint16_t page_size, size_t page_no,
               uint8_t *buffer) {
  if (fseek(db_file, (off_t)page_no * page_size, SEEK_SET) != 0) {
    return -1;
  }
  size_t num_el = fread(buffer, 1, page_size, db_file);

  return (int)num_el;
}
