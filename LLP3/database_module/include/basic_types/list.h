#ifndef LIST_H
#define LIST_H

#include "tuple.h"

/**
 *  Определение типов данных
 */

struct uint64_list{
    uint64_t value;
    struct uint64_list *next;
};

struct result_list_tuple {
    uint64_t id;
    struct tuple *value;
    struct result_list_tuple *next;
};

/**
 *  Определение функций
 */

void append_to_uint64_list(uint64_t id, struct uint64_list **result);
void free_uint64_list(struct uint64_list *result);
void append_to_result_list(struct tuple **tuple_to_add, struct result_list_tuple **result, uint64_t id);
void free_result_list(struct result_list_tuple *result);

#endif
