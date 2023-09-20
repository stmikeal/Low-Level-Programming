#include "adv_types/tree_header_tools.h"

enum crud_operation_status offset_to_id(FILE *file, uint64_t* id, uint64_t offset) {
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    for (size_t iter = 0; iter < header->subheader->cur_id; iter++){
        if (header->id_sequence[iter] == offset){
            *id = iter;
            free_tree_header(header);
            return CRUD_OK;
        }
    }
    free_tree_header(header);
    return CRUD_INVALID;
}

int valid_offset_id(FILE *file, size_t fpos, uint64_t id, uint64_t offset) {
    fseek(file, fpos + id * sizeof(uint64_t), SEEK_SET);
    uint64_t offset_file;
    read_from_file(&offset_file, file, sizeof(uint64_t));
    return offset_file == offset;
}

enum crud_operation_status id_to_offset(FILE *file, uint64_t id, uint64_t* offset) {
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header_no_id(header, file, &pos);
    fseek(file, pos + id * sizeof(uint64_t), SEEK_SET);
    read_from_file(offset, file, sizeof(uint64_t));
    free_tree_header_no_id(header);
    return CRUD_OK;
}

struct uint64_list *get_childs_by_id(FILE *file, uint64_t id) {
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    struct uint64_list *result = NULL;
    union tuple_header cur_header;
    for (uint64_t iter = 0; iter < header->subheader->cur_id; iter++) {
        fseek(file, (int64_t) header->id_sequence[iter], SEEK_SET);
        read_from_file(&cur_header, file, sizeof(union tuple_header));
        if (cur_header.parent == id) append_to_uint64_list(iter, &result);
    }
    free_tree_header(header);
    return result;
}

uint64_t remove_from_id_array(FILE *file, uint64_t id) {
    uint64_t offset;
    uint64_t null_val = 0;
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header_no_id(header, file, &pos);
    if (header->subheader->cur_id-1 == id) {
        header->subheader->cur_id--;
    }
    write_tree_header_no_id(file, header);    fseek(file, pos + id * sizeof(uint64_t), SEEK_SET);
    read_from_file(&offset, file, sizeof(uint64_t));
    fseek(file, pos + id * sizeof(uint64_t), SEEK_SET);
    write_to_file(&null_val, file, sizeof(uint64_t));
    free_tree_header_no_id(header);
    return offset;
}

enum crud_operation_status append_to_id_array(FILE *file, uint64_t offset) {
    enum crud_operation_status status = CRUD_OK;
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t array_pos;
    read_tree_header_no_id(header, file, &array_pos);
    uint64_t real_tuple_size = get_real_id_array_size(header->subheader->pattern_size, header->subheader->cur_id);
    if (!((header->subheader->cur_id + 1) % real_tuple_size)){
        uint64_t from = ftell(file) + real_tuple_size * sizeof(uint64_t);
        fseek(file, 0, SEEK_END);
        uint64_t cur_end = ftell(file);
        status |= ftruncate(fileno(file), cur_end + get_real_tuple_size(header->subheader->pattern_size) + sizeof(union tuple_header));
        swap_tuple_to(file, from, cur_end, get_real_tuple_size(header->subheader->pattern_size) + sizeof(union tuple_header));
    }
    write_id_value(file, array_pos, offset, header->subheader->cur_id++);
    write_tree_header_no_id(file, header);
    free_tree_header_no_id(header);
    return status;
}

void get_types(FILE *file, uint32_t **types, size_t *size) {
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = malloc(sizeof(struct tree_header));
    size_t pos;
    read_tree_header_no_id(header, file, &pos);
    uint32_t *temp_types = malloc(header->subheader->pattern_size * sizeof(uint32_t));
    for (size_t iter = 0; iter < header->subheader->pattern_size; iter++) {
        temp_types[iter] = header->pattern[iter]->header->type;
    }
    *types = temp_types;
    *size = header->subheader->pattern_size;
    free_tree_header_no_id(header);
}
