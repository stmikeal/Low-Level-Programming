#include "crud_tools/crud.h"
#include <time.h>

enum crud_operation_status add_tuple(FILE *file, uint64_t *fields, uint64_t parent_id) {
    uint32_t *types;
    size_t size;
    get_types(file, &types, &size);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t fpos = 0;
    read_tree_header_no_id(header, file, &fpos);
    size_t full_tuple_size = sizeof(union tuple_header) + get_real_tuple_size(size);
    struct tuple *new_tuple = malloc(sizeof(struct tuple));
    new_tuple->header.parent = parent_id;
    new_tuple->header.alloc = header->subheader->cur_id;
    new_tuple->data = malloc(get_real_tuple_size(size));
    uint64_t *link = malloc(sizeof(uint64_t));
    uint64_t par_pos;
    enum crud_operation_status status = insert_new_tuple(file, new_tuple, full_tuple_size, &par_pos);
    append_to_id_array(file, par_pos);
    for (size_t iter = 0; iter < size; iter++) {
        if (types[iter] == STRING_TYPE) {
            insert_string_tuple(file, (char *) fields[iter], get_real_tuple_size(size), par_pos, link);
            new_tuple->data[iter] = *link;
        } else {
            new_tuple->data[iter] = (uint64_t) fields[iter];
        }
    }
    fseek(file, par_pos, SEEK_SET);
    status |= write_tuple(file, new_tuple, get_real_tuple_size(size));
    free(link);
    free_tuple(new_tuple);
    free(types);
    return status;
}

enum crud_operation_status update_tuple(FILE *file, uint64_t field_number, uint64_t *new_value, uint64_t id){
    uint32_t *types;
    size_t size;
    get_types(file, &types, &size);
    uint64_t type = types[field_number];
    uint64_t  offset;
    id_to_offset(file, id, &offset);
    struct tuple* cur_tuple;
    fseek(file, offset, SEEK_SET);
    read_tuple(&cur_tuple, file, size);
    if (type == STRING_TYPE){
        change_string_tuple(file, cur_tuple->data[field_number], (char *) *new_value, get_real_tuple_size(size));
    } else {
        cur_tuple->data[field_number] = *new_value;
        fseek(file, offset, SEEK_SET);
        write_tuple(file, cur_tuple, get_real_tuple_size(size));
    }
    free_tuple(cur_tuple);
    free(types);
    return 0;
}

static enum crud_operation_status remove_recursive_tuple_with_values(FILE *file, uint64_t id, uint32_t *types, size_t pattern_size) {
    uint64_t size = get_real_tuple_size(pattern_size);
    uint64_t offset = remove_from_id_array(file, id);
    if (offset == NULL_VALUE) return CRUD_INVALID;
    struct uint64_list *childs = get_childs_by_id(file, id);
    for(struct uint64_list *iter = childs; iter != NULL; iter = iter->next) {
        remove_recursive_tuple_with_values(file, iter->value, types, pattern_size);
    }
    struct tuple *cur_tuple;
    fseek(file, (int32_t) offset, SEEK_SET);
    read_tuple(&cur_tuple, file, pattern_size);
    for(size_t iter = 0; iter < pattern_size; iter++) {
        if (types[iter] == STRING_TYPE) {
            remove_string_from_file(file, cur_tuple->data[iter], size);
        }

    }
    free_uint64_list(childs);
    free_tuple(cur_tuple);
    return swap_with_last(file, offset, size);
}

enum crud_operation_status remove_tuple(FILE *file, uint64_t id) {
    uint32_t *types;
    size_t size;
    get_types(file, &types, &size);
    enum crud_operation_status status = remove_recursive_tuple_with_values(file, id, types, size);
    free(types);
    return status;
}

enum crud_operation_status get_tuple(uint64_t id, struct result_list_tuple **result, struct result_list_tuple **new_result) {
    struct result_list_tuple *next;
    while((*result) != NULL){
        if (((*result)->id) == id) {
            append_to_result_list(&((*result)->value), new_result, (*result)->id);
        } else {
            free_tuple((*result)->value);
        }
        next = (*result)->next;
        free(*result);
        *result = next;
    }
    return CRUD_OK;
}


static uint8_t op_equal(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case INTEGER_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.integer) == (op2.is_field ? tuple->data[op2.integer] : op2.integer);
        case BOOLEAN_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.boolean) == (op2.is_field ? tuple->data[op2.integer] : op2.boolean);
        case FLOAT_TYPE: return (op1.is_field ? (double) tuple->data[op1.integer] : op1.real) == (op2.is_field ? (double) tuple->data[op2.integer] : op2.real);
        case STRING_TYPE: return strcmp((op1.is_field ? (char *) tuple->data[op1.integer] : op1.string), (op2.is_field ? (char *) tuple->data[op2.integer] : op2.string));
        default: return 0;
    }
}

static uint8_t op_greater(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case INTEGER_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.integer) > (op2.is_field ? tuple->data[op2.integer] : op2.integer);
        case BOOLEAN_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.boolean) > (op2.is_field ? tuple->data[op2.integer] : op2.boolean);
        case FLOAT_TYPE: return (op1.is_field ? (double) tuple->data[op1.integer] : op1.real) > (op2.is_field ? (double) tuple->data[op2.integer] : op2.real);
        default: return 0;
    }
}

static uint8_t op_less(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case INTEGER_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.integer) < (op2.is_field ? tuple->data[op2.integer] : op2.integer);
        case BOOLEAN_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.boolean) < (op2.is_field ? tuple->data[op2.integer] : op2.boolean);
        case FLOAT_TYPE: return (op1.is_field ? (double) tuple->data[op1.integer] : op1.real) < (op2.is_field ? (double) tuple->data[op2.integer] : op2.real);
        default: return 0;
    }
}

static uint8_t op_not_greater(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case INTEGER_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.integer) <= (op2.is_field ? tuple->data[op2.integer] : op2.integer);
        case BOOLEAN_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.boolean) <= (op2.is_field ? tuple->data[op2.integer] : op2.boolean);
        case FLOAT_TYPE: return (op1.is_field ? (double) tuple->data[op1.integer] : op1.real) <= (op2.is_field ? (double) tuple->data[op2.integer] : op2.real);
        default: return 0;
    }
}

static uint8_t op_not_less(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case INTEGER_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.integer) >= (op2.is_field ? tuple->data[op2.integer] : op2.integer);
        case BOOLEAN_TYPE: return (op1.is_field ? tuple->data[op1.integer] : op1.boolean) >= (op2.is_field ? tuple->data[op2.integer] : op2.boolean);
        case FLOAT_TYPE: return (op1.is_field ? (double) tuple->data[op1.integer] : op1.real) >= (op2.is_field ? (double) tuple->data[op2.integer] : op2.real);
        default: return 0;
    }
}

static uint8_t op_substr(struct operator_type op1, struct operator_type op2, struct tuple *tuple){
    if (op1.op_type == op2.op_type) return 0;
    switch (op1.op_type) {
        case STRING_TYPE: return strstr((op1.is_field ? (char *) tuple->data[op1.integer] : op1.string), (op2.is_field ? (char *) tuple->data[op2.integer] : op2.string)) != NULL;
        default: return 0;
    }
}


static uint8_t  do_filter(struct operator_type op1, struct operator_type op2, enum find_operation operation, struct tuple *tuple){
    switch (operation) {
        case OP_EQUAL: return op_equal(op1, op2, tuple);
        case OP_GREATER: return op_greater(op1, op2, tuple);
        case OP_LESS: return op_less(op1, op2, tuple);
        case OP_NOT_GREATER: return op_not_greater(op1, op2, tuple);
        case OP_NOT_LESS: return op_not_less(op1, op2, tuple);
        case OP_SUBSTR: return op_substr(op1, op2, tuple);
        default: return 0;
    }
}

enum crud_operation_status find_by_field(struct operator_type op1, struct operator_type op2, enum find_operation operation,
        struct result_list_tuple **result, struct result_list_tuple **new_result){
    struct result_list_tuple *next;
    while((*result) != NULL){
        if (do_filter(op1, op2, operation, (*result)->value)) {
            append_to_result_list(&((*result)->value), new_result, (*result)->id);
        } else {
            free_tuple((*result)->value);
        }
        next = (*result)->next;
        free(*result);
        *result = next;
    }
    return CRUD_OK;
}

enum crud_operation_status find_by_parent(uint64_t parent_id, struct result_list_tuple **result, struct result_list_tuple **new_result){
    struct result_list_tuple *next;
    while((*result) != NULL){
        if (((*result)->value)->header.parent == parent_id) {
            append_to_result_list(&((*result)->value), new_result, (*result)->id);
        } else {
            free_tuple((*result)->value);
        }
        next = (*result)->next;
        free(*result);
        *result = next;
    }
    return CRUD_OK;
}

enum crud_operation_status find_all(FILE *file, struct result_list_tuple **result){
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header(header, file, &pos);
    struct tuple* cur_tuple;
    for(size_t i = 0; i < header->subheader->cur_id; i++){
        if (header->id_sequence[i] == NULL_VALUE) continue;
        fseek(file, header->id_sequence[i], SEEK_SET);
        read_tuple(&cur_tuple, file, header->subheader->pattern_size);
        append_to_result_list(&cur_tuple, result, i);
    }
    free_tree_header(header);
    return CRUD_OK;
}


