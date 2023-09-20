#ifndef CRUD_H
#define CRUD_H

#include "adv_types/tuple_tools.h"

enum find_operation {
    OP_EQUAL = 0,
    OP_GREATER,
    OP_LESS,
    OP_NOT_GREATER,
    OP_NOT_LESS,
    OP_SUBSTR
};

struct operator_type {
    uint8_t is_field;
    uint8_t op_type;
    union {
        uint64_t integer;
        uint64_t boolean;
        double real;
        char *string;
    };
};

enum crud_operation_status add_tuple(FILE *file, uint64_t *fields, uint64_t parent_id);
enum crud_operation_status get_tuple(uint64_t id, struct result_list_tuple **result, struct result_list_tuple **new_result);
enum crud_operation_status remove_tuple(FILE *file, uint64_t id);
enum crud_operation_status find_by_field(struct operator_type op1, struct operator_type op2, enum find_operation operation,
                                         struct result_list_tuple **result, struct result_list_tuple **new_result);
enum crud_operation_status find_all(FILE *file, struct result_list_tuple **result);
enum crud_operation_status find_by_parent(uint64_t parent_id, struct result_list_tuple **result, struct result_list_tuple **new_result);
enum crud_operation_status update_tuple(FILE *file, uint64_t field_number, uint64_t *new_value, uint64_t id);

#endif