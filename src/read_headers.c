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

bool read_btree_headers(FILE *dbfile, BTreeHeader *bth) {
  void *data = read_n_bytes(dbfile, btree_header);
  if (data == NULL) {
    return false;
  }

  uint8_t *conv_data = (uint8_t *)data;

  bth->node_type = conv_data[0];
  bth->freeblocks = conv_data[1] << 8 | conv_data[2];
  bth->cells = conv_data[3] << 8 | conv_data[4];
  bth->cell_start = conv_data[5] << 8 | conv_data[6];
  bth->frag_trees = conv_data[7];

  if (bth->node_type == 2 || bth->node_type == 5) {
    free(data);
    data = read_n_bytes(dbfile, interior_btree_header_extra);
    if (data == NULL) {
      return false;
    }
    conv_data = (uint8_t *)data;
    bth->right_ptr = conv_data[0] << 24 | conv_data[1] << 16 |
                     conv_data[2] << 8 | conv_data[3];
    free(data);
  }

  return true;
}

bool read_cell_ptr_array(FILE *dbfile, uint16_t *cell_ptr_arr, uint16_t cells) {
  for (int i = 0; i < cells; i++) {
    uint8_t *data = read_n_bytes(dbfile, 2);
    if (!data) {
      return false;
    }
    cell_ptr_arr[i] = data[0] << 8 | data[1];
    free(data);
  }
  return true;
}
