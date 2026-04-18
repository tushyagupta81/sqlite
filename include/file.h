#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void * read_n_bytes(FILE* fileptr, size_t size);
uint64_t read_big_endian(const uint8_t *data, size_t num);
int read_page(FILE *db_file, uint16_t page_size, size_t page_no, uint8_t* buffer);

#endif // !FILE_H
