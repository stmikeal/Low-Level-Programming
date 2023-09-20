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

enum crud_operation_status get_tuple(FILE *file, uint64_t **fields, uint64_t id) {
    uint64_t offset;
    id_to_offset(file, id, &offset);
    if (offset == NULL_VALUE) return CRUD_INVALID;
    struct tuple *cur_tuple;
    uint32_t *types;
    size_t size;
    get_types(file, &types, &size);
    fseek(file, offset, SEEK_SET);
    read_tuple(&cur_tuple, file, (uint64_t) size);
    *fields = malloc(sizeof(uint64_t) * size);
    for (size_t iter = 0; iter < size; iter++) {
        if (types[iter] == STRING_TYPE) {
            char *s;
            read_string_from_tuple(file, &s, size, cur_tuple->data[iter]);
            (*fields)[iter] = (uint64_t) s;
        } else {
            (*fields)[iter] = cur_tuple->data[iter];
        }
    }
    free(types);
    free_tuple(cur_tuple);
    return CRUD_OK;
}

static enum crud_operation_status remove_recursive_tuple_with_values
        (FILE *file, uint64_t id, uint32_t *types, size_t pattern_size) {

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



enum crud_operation_status find_by_field(FILE *file, uint64_t field_number, uint64_t *condition, struct result_list_tuple **result){
    uint32_t *types;
    size_t size;
    get_types(file, &types, &size);
    uint64_t type = types[field_number];
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header(header, file, &pos);
    struct tuple* cur_tuple;
    for(size_t i = 0; i < header->subheader->cur_id; i++){
        if (header->id_sequence[i] == NULL_VALUE) continue;
        fseek(file, header->id_sequence[i], SEEK_SET);
        read_tuple(&cur_tuple, file, size);
        if (type == STRING_TYPE){
            char *s;
            read_string_from_tuple(file, &s, size, cur_tuple->data[field_number]);
            if (!strcmp(s, (char *) *condition)) {
                append_to_result_list(&cur_tuple, result);
            }
            free(s);
        } else {
            if (cur_tuple->data[field_number] == *condition) {
                append_to_result_list(&cur_tuple, result);
            }
        }

    }
    //free_tree_header(header);
    //free_tuple(cur_tuple);
    //free(types);
    return 0;
}

enum crud_operation_status find_by_parent(FILE *file, uint64_t parent_id, struct result_list_tuple **result){
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header_no_id(header, file, &pos);
    struct tuple* cur_tuple;
    uint64_t offset;
    for(size_t i = 0; i < header->subheader->cur_id; i++){
        read_from_file(&offset, file, sizeof(uint64_t));
        if (offset == NULL_VALUE) continue;
        fseek(file, offset, SEEK_SET);
        read_tuple(&cur_tuple, file, header->subheader->pattern_size);
        if (cur_tuple->header.parent == parent_id) {
            append_to_result_list(&cur_tuple, result);
        } else {
            free_tuple(cur_tuple);
        }

    }
    free_tree_header_no_id(header);
    return 0;
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
        append_to_result_list(&cur_tuple, result);

    }
    free_tree_header(header);
    return 0;
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