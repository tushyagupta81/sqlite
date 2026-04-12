#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdio.h>

void * read_n_bytes(FILE* fileptr, size_t size);
uint64_t read_big_endian(const uint8_t *data, size_t num);

#endif // !FILE_H
