#include "cells.h"
#include "file.h"
#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int serial_type_size(uint64_t t) {
  if (t == 0) {
    return 0; // NULL
  }

  if (t == 1) {
    return 1;
  }

  if (t == 2) {
    return 2;
  }

  if (t == 3) {
    return 3;
  }

  if (t == 4) {
    return 4;
  }

  if (t == 5) {
    return 6;
  }

  if (t == 6) {
    return 8;
  }

  if (t == 7) {
    return 8; // float
  }

  if (t == 8) {
    return 0; // integer 0
  }

  if (t == 9) {
    return 0; // integer 1
  }

  if (t >= 12) {
    if (t % 2 == 0) {
      return ((int)t - 12) / 2; // blob
    }
    return ((int)t - 13) / 2; // text
  }

  return -1; // invalid
}

bool read_cell(FILE *dbfile, uint16_t offset, Cell *cell) {
  fseek(dbfile, offset, SEEK_SET);

  if (read_varint(dbfile, &cell->rec_size) == -1) {
    return false;
  }
  if (read_varint(dbfile, &cell->row_id) == -1) {
    return false;
  }

  int size = read_varint(dbfile, &cell->header_size);
  if (size == -1) {
    return false;
  }
  uint64_t header_left = cell->header_size - size;

  uint64_t tot_size = 0;

  while (header_left > 0) {
    uint64_t serial;
    int used = read_varint(dbfile, &serial);
    if (used == -1) {
      return false;
    }

    int col_size = serial_type_size(serial);
    if (col_size < 0) {
      return false;
    }

    push_back(&cell->rec.size, col_size);
    tot_size += col_size;

    if (used > cell->header_size) {
      return false;
    }
    header_left -= used;
  }

  cell->rec.tot_size = tot_size;

  return true;
}

bool dump_tables(FILE *dbfile, Cell *cell) {
  uint64_t data_bytes = cell->rec_size - cell->header_size;
  uint8_t *data = read_n_bytes(dbfile, data_bytes);
  if (!data) {
    return false;
  }
  uint8_t *ptr = data;

  for (int i = 0; i < cell->rec.size.top; i++) {
    uint64_t sz = *(uint64_t *)get(&cell->rec.size, i);

    printf("\"%.*s\"\n", (int)sz, ptr);
    ptr += sz;
  }

  free(data);
  return true;
}

int read_varint(FILE *dbfile, uint64_t *val) {
  uint64_t res = 0;
  int idx;
  for (idx = 0; idx < 8; idx++) {
    int data = fgetc(dbfile);
    if (data == EOF) {
      return -1;
    }

    res = res << 7 | (data & 0x7F);

    if (!(data & 0x80)) {
      *val = res;
      return idx + 1;
    }
  }

  int data = fgetc(dbfile);
  if (data == EOF) {
    return -1;
  }

  res = res << 8 | data;

  *val = res;

  return 9;
}
