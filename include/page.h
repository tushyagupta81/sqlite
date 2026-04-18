#ifndef PAGE_H
#define PAGE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool read_x_page(FILE *db_file, uint16_t page_size, size_t page_no);

#endif // !PAGE_H
