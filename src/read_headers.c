#include "read_headers.h"
#include "file.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int magic_string_len = 16;
const int db_file_header = 100;
const int btree_header = 8;
const int interior_btree_header_extra = 4;

bool check_magic_number(FILE *dbfile, size_t file_len) {
  if (file_len < magic_string_len) {
    return false;
  }

  char *magic = read_n_bytes(dbfile, magic_string_len);
  if (!magic) {
    return false;
  }

  if (strcmp("SQLite format 3\0", magic) != 0) {
    free(magic);
    return false;
  }

  free(magic);
  return true;
}

bool read_btree_headers(uint8_t *buffer, size_t size, BTreeHeader *bth,
                        bool first_page) {
  size_t initial_size = first_page ? 100 : 0;
  if (size < initial_size + 8) {
    return false;
  }

  uint8_t *conv_data = buffer + initial_size;
  bth->node_type = conv_data[0];
  bth->freeblocks = ((uint16_t)conv_data[1] << 8) | conv_data[2];
  bth->cells = ((uint16_t)conv_data[3] << 8) | conv_data[4];
  bth->cell_start = ((uint16_t)conv_data[5] << 8) | conv_data[6];
  bth->frag_trees = conv_data[7];

  if (bth->node_type != 0x02 && bth->node_type != 0x05 &&
      bth->node_type != 0x0A && bth->node_type != 0x0D) {
    return false;
  }

  if (bth->node_type == 2 || bth->node_type == 5) {
    if (size < initial_size + 12) {
      return false;
    }
    bth->right_ptr = ((uint32_t)conv_data[8] << 24) |
                     ((uint32_t)conv_data[9] << 16) |
                     ((uint32_t)conv_data[10] << 8) | (uint32_t)conv_data[11];
  }

  return true;
}

bool read_cell_ptr_array(uint8_t *buffer, size_t size, uint16_t *cell_ptr_arr,
                         uint16_t cells, uint16_t bth_size, bool page_no) {
  const size_t initial_offset = (page_no == 1 ? 100: 0) + bth_size;
  if (size < initial_offset) {
    return false;
  }

  uint8_t *data = buffer + initial_offset;
  size -= initial_offset;

  if (size < (size_t)cells * 2) {
    return false;
  }

  for (int i = 0; i < cells; i++) {
    cell_ptr_arr[i] = ((uint16_t)data[0] << 8) | data[1];
    data += 2;
  }

  return true;
}

void dump_bth(BTreeHeader *bth) {
  printf("Node type = %hu\n", bth->node_type);
  printf("Free blocks = %hu\n", bth->freeblocks);
  printf("Cells = %hu\n", bth->cells);
  printf("Cell start = %hu\n", bth->cell_start);
  printf("Frag trees = %hu\n", bth->frag_trees);
  if (bth->right_ptr) {
    printf("Right ptr = %hu\n", bth->right_ptr);
  }
}
