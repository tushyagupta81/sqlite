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

SerialType number_to_serial(uint64_t t) {
  if (t == 0) {
    return None; // NULL
  }

  if (t == 1) {
    return EightBit2Comp;
  }

  if (t == 2) {
    return SixteenBit2Comp;
  }

  if (t == 3) {
    return TwentyfourBit2Comp;
  }

  if (t == 4) {
    return ThirtytwoBit2Comp;
  }

  if (t == 5) {
    return FourtyeightBit2Comp;
  }

  if (t == 6) {
    return SixtyfourBit2Comp;
  }

  if (t == 7) {
    return SixtyfourBitFloat;
  }

  if (t == 8) {
    return ValZero; // integer 0
  }

  if (t == 9) {
    return ValOne; // integer 1
  }

  if (t == 10 || t == 11) {
    return Reserved;
  }

  if (t >= 12) {
    if (t % 2 == 0) {
      return Blob; // blob
    }
    return String; // text
  }

  return Invalid; // invalid
}

bool read_cell(uint8_t * buffer, size_t page_size, uint16_t offset, Cell *cell) {
  int rec_size_bytes = read_varint(buffer, page_size, offset, &cell->rec_size);
  if (rec_size_bytes == -1) {
    return false;
  }
  offset += rec_size_bytes;
  int row_id_bytes = read_varint(buffer, page_size, offset, &cell->row_id);
  if (row_id_bytes == -1) {
    return false;
  }
  offset += row_id_bytes;

  cell->rec_start_offset = row_id_bytes + rec_size_bytes;

  int size = read_varint(buffer, page_size, offset, &cell->rec.header_size);
  if (size == -1) {
    return false;
  }
  offset += size;

  uint64_t header_left = cell->rec.header_size - size;

  uint64_t tot_size = 0;

  uint8_t n_cols = 0;

  while (header_left > 0) {
    uint64_t serial;
    int used = read_varint(buffer, page_size, offset, &serial);
    if (used == -1) {
      return false;
    }
    offset += used;

    int col_size = serial_type_size(serial);
    if (col_size < 0) {
      return false;
    }

    push_back(&cell->rec.serials, serial);
    tot_size += col_size;
    n_cols++;

    if (used > header_left) {
      return false;
    }
    header_left -= used;
  }

  cell->rec.tot_size = tot_size;
  cell->rec.n_cols = n_cols;

  return true;
}

bool dump_tables(FILE *dbfile, Cell *cell) {
  uint64_t data_bytes = cell->rec_size - cell->rec.header_size;
  fseek(dbfile, (long)(cell->rec_start_offset + cell->rec.header_size),
        SEEK_CUR);

  for (int i = 0; i < cell->rec.serials.top; i++) {
    uint64_t serial = *(uint64_t *)get(&cell->rec.serials, i);
    uint64_t col_size = serial_type_size(serial);
    uint8_t *data = read_n_bytes(dbfile, col_size);
    if (!data) {
      return false;
    }

    SerialType s = number_to_serial(serial);

    if (s == String) {
      printf("%3lu: \"%.*s\"\n", col_size, (int)col_size, data);
    } else if (s == None || s == Reserved || s == Blob || s == Invalid) {
      printf("%3lu: --nothing--", col_size);
    } else if (s == ValOne) {
      printf("%3lu: \"1\"\n", col_size);
    } else if (s == ValZero) {
      printf("%3lu: \"0\"\n", col_size);
    } else {
      printf("%3lu: \"%lu\"\n", col_size, read_big_endian(data, col_size));
    }

    free(data);
  }

  return true;
}

int read_varint(uint8_t *buffer, size_t size, size_t offset, uint64_t *val) {
  if (size <= offset) {
    return -1;
  }

  uint64_t res = 0;
  size_t idx;
  for (idx = 0; idx < 8; idx++) {
    if (idx >= size - offset) {
      return -1;
    }

    uint8_t data = buffer[offset + idx];

    res = res << 7 | (data & 0x7F);

    if (!(data & 0x80)) {
      *val = res;
      return (int)idx + 1;
    }
  }

  if (offset + 8 >= size) {
    return -1;
  }
  uint8_t data = buffer[offset + 8];

  res = res << 8 | data;

  *val = res;

  return 9;
}

void init_cell(Cell *cell) {
  cell->rec_size = 0;
  cell->row_id = 0;
  cell->rec_start_offset = 0;
  cell->rec.tot_size = 0;
  cell->rec.header_size = 0;
  init_arr(&cell->rec.serials);
}
