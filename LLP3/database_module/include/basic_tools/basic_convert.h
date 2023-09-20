#ifndef BASIC_CONVERT_H
#define BASIC_CONVERT_H
#include "configuration.h"
#include <inttypes.h>
#include <stdlib.h>

size_t get_real_tuple_size(uint64_t pattern_size);
size_t get_real_id_array_size(uint64_t pattern_size, uint64_t cur_id);
size_t get_real_string_size(size_t len);
void free_test(void *ptr);
void *malloc_test(size_t size);


#endif
