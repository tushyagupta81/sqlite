#ifndef CELLS_H
#define CELLS_H

#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint64_t tot_size;
  Array size;
} Record;

typedef struct {
  uint64_t rec_size;
  uint64_t row_id;
  uint64_t header_size;
  Record rec;
} Cell;

int read_varint(FILE *dbfile, uint64_t *val);
bool read_cell(FILE *dbfile, uint16_t offset, Cell *cell);
bool dump_tables(FILE *dbfile, Cell *cell);

#endif // !CELLS_H
