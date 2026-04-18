#ifndef CELLS_H
#define CELLS_H

#include "mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
  None,
  EightBit2Comp,
  SixteenBit2Comp,
  TwentyfourBit2Comp,
  ThirtytwoBit2Comp,
  FourtyeightBit2Comp,
  SixtyfourBit2Comp,
  SixtyfourBitFloat,
  ValZero,
  ValOne,
  Reserved,
  Blob,
  String,
  Invalid
} SerialType;

typedef struct {
  uint64_t tot_size;
  uint8_t n_cols;
  uint64_t header_size;
  Array serials;
} Record;

typedef struct {
  uint64_t rec_size;
  uint64_t row_id;
  uint64_t rec_start_offset;
  Record rec;
} Cell;

int read_varint(uint8_t *buffer, size_t size, size_t offset, uint64_t *val);
bool read_cell(uint8_t * buffer, size_t page_size, uint16_t offset, Cell *cell);
bool dump_tables(uint8_t *buffer, size_t size, Cell *cell);
void init_cell(Cell * cell);

#endif // !CELLS_H
