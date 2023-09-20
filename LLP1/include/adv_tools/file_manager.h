#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "basic_types/tree_header.h"
#include "generator/empty_generator.h"

enum file_write_status init_empty_file(FILE *file, char **pattern, uint32_t *types, size_t pattern_size, size_t *key_sizes);
enum file_open_status open_file_anyway(FILE **file, char *filename);

#endif
