#ifndef TREE_HEADER_TOOLS_H
#define TREE_HEADER_TOOLS_H

#include "basic_types/tree_header.h"
#include "tuple_tools.h"
#include "basic_types/list.h"
#include "crud_status.h"

enum crud_operation_status offset_to_id(FILE *file, uint64_t* id, uint64_t offset);
enum crud_operation_status id_to_offset(FILE *file, uint64_t id, uint64_t* offset);
struct uint64_list *get_childs_by_id(FILE *file, uint64_t id);
uint64_t remove_from_id_array(FILE *file, uint64_t id);
enum crud_operation_status append_to_id_array(FILE *file, uint64_t offset);
void get_types(FILE *file, uint32_t **types, size_t *size);
int valid_offset_id(FILE *file, size_t fpos, uint64_t id, uint64_t offset);

#endif
