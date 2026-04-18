#include "page.h"
#include "cells.h"
#include "file.h"
#include "read_headers.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool read_x_page(FILE *db_file, uint16_t page_size, size_t page_no) {
  uint8_t *page = malloc(sizeof(uint8_t) * page_size);
  if (!page) {
    free(page);
    fprintf(stderr, "Failed to create page buffer\n");
    return false;
  }

  int page_read_size = read_page(db_file, page_size, page_no, page);
  if (page_read_size == -1) {
    free(page);
    fprintf(stderr, "Failed to read page into buffer\n");
    return 1;
  }

  BTreeHeader bth;
  if (!read_btree_headers(page, page_read_size, &bth, page_no == 1)) {
    free(page);
    fprintf(stderr, "Failed to read BTreeHeader");
    return false;
  }

  dump_bth(&bth);

  uint16_t *cell_ptr_arr = malloc(sizeof(uint16_t) * bth.cells);
  read_cell_ptr_array(page, page_read_size, cell_ptr_arr, bth.cells,
                      8 + (bth.right_ptr ? 4 : 0), page_no);

  for (int i = 0; i < bth.cells; i++) {
    printf("\n");
    Cell cell;
    init_cell(&cell);
    read_cell(page, page_read_size, cell_ptr_arr[i], &cell);
    dump_tables(page, page_read_size, &cell);
    printf("\n");
  }

  return true;
}
