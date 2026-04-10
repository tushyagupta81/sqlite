#ifndef READ_HEADERS_H
#define READ_HEADERS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct{
  uint8_t node_type;
  uint16_t freeblocks;
  uint16_t cells;
  uint16_t cell_start;
  uint8_t frag_trees;
  uint32_t right_ptr;
} BTreeHeader;

bool check_magic_number(FILE *dbfile, size_t file_len);
bool read_btree_headers(FILE *dbfile, BTreeHeader *bth);
bool read_cell_ptr_array(FILE *dbfile, uint16_t *cell_ptr_arr, uint16_t cells);

#endif // !READ_HEADERS_H
