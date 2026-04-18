#include "cells.h"
#include "file.h"
#include "page.h"
#include "read_headers.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage sqlite <file.db> [command]");
    return 1;
  }

  FILE *db_file = fopen(argv[1], "rb+");
  if (!db_file) {
    fprintf(stderr, "Failed to read file %s\n", argv[1]);
    return 1;
  }

  fseek(db_file, 0LL, SEEK_END);
  size_t file_size = ftell(db_file);
  fseek(db_file, 0LL, SEEK_SET);

  if (!check_magic_number(db_file, file_size)) {
    fprintf(stderr, "Missing magic header in %s\n", argv[1]);
    return 1;
  }

  fseek(db_file, 16LL, SEEK_SET);
  uint8_t *page_size_ptr = read_n_bytes(db_file, 2);
  if (!page_size_ptr) {
    fprintf(stderr, "Missing page size in %s\n", argv[1]);
    return 1;
  }
  uint16_t page_size = ((uint16_t)page_size_ptr[0] << 8) | page_size_ptr[1];
  if (page_size == 1) {
    page_size = UINT16_MAX;
  }
  free(page_size_ptr);

  uint8_t *page = malloc(sizeof(uint8_t) * page_size);
  if (!page) {
    free(page);
    page = NULL;
    fprintf(stderr, "Failed to create page buffer\n");
    return 1;
  }

  int page_read_size = read_page(db_file, page_size, 1, page);
  if (page_read_size == -1) {
    free(page);
    page = NULL;
    fprintf(stderr, "Failed to read page into buffer\n");
    return 1;
  }

  BTreeHeader bth;
  if (!read_btree_headers(page, page_read_size, &bth, true)) {
    fprintf(stderr, "Failed to read BTreeHeader in %s\n", argv[1]);
    return 1;
  }

  uint16_t *cell_ptr_arr = malloc(sizeof(uint16_t) * bth.cells);
  read_cell_ptr_array(page, page_read_size, cell_ptr_arr, bth.cells,
                      8 + (bth.right_ptr ? 4 : 0), 1);

  // for(int i=0;i<bth.cells;i++){
  //   printf("%hu\n", cell_ptr_arr[i]);
  // }

  if (argc == 3) {
    if (strcmp(argv[2], ".dbinfo") == 0) {
      printf("%hu\n", page_size);
      printf("%hu\n", bth.cells);
    } else if (strcmp(argv[2], ".tables") == 0) {
      for (int i = 0; i < bth.cells; i++) {
        printf("\n");
        Cell cell;
        init_cell(&cell);
        read_cell(page, page_read_size, cell_ptr_arr[i], &cell);

        dump_tables(page, page_read_size, &cell);
        printf("\n");
      }
    }
  }

  // read_x_page(db_file, page_size, 2);

  free(cell_ptr_arr);
  return 0;
}
