#ifndef CRUD_H
#define CRUD_H

#include "adv_types/tuple_tools.h"

enum crud_operation_status add_tuple(FILE *file, uint64_t *fields, uint64_t parent_id);
enum crud_operation_status get_tuple(FILE *file, uint64_t **fields, uint64_t id);
enum crud_operation_status remove_tuple(FILE *file, uint64_t id);
enum crud_operation_status find_by_field(FILE *file, uint64_t field_number, uint64_t *condition, struct result_list_tuple **result);
enum crud_operation_status find_by_parent(FILE *file, uint64_t parent_id, struct result_list_tuple **result);
enum crud_operation_status find_all(FILE *file, struct result_list_tuple **result);
enum crud_operation_status update_tuple(FILE *file, uint64_t field_number, uint64_t *new_value, uint64_t id);

#endif