#ifndef LLP_1_TUPLE_TOOLS_H
#define LLP_1_TUPLE_TOOLS_H

#include "basic_types/tuple.h"
#include <string.h>
#include "crud_status.h"
#include "tree_header_tools.h"
#include <unistd.h>

struct tuple;

enum file_read_status read_string_from_tuple(FILE *file, char **string, uint64_t pattern_size, uint64_t offset);
enum crud_operation_status swap_tuple_to(FILE *file, uint64_t pos_from, uint64_t pos_to, size_t tuple_size);
enum crud_operation_status swap_with_last(FILE *file, uint64_t offset, uint64_t size);
enum crud_operation_status insert_new_tuple(FILE *file, struct tuple *tuple, size_t full_tuple_size, uint64_t *tuple_pos);
enum crud_operation_status insert_string_tuple(FILE *file, char *string, size_t tuple_size, uint64_t par_pos, uint64_t *str_pos);
enum crud_operation_status change_string_tuple(FILE *file, uint64_t offset, char *new_string, uint64_t size);
enum crud_operation_status remove_string_from_file(FILE* file, uint64_t offset, uint64_t size);

#endif
