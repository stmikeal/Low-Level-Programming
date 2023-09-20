#include "adv_types/tuple_tools.h"

static size_t how_long_string_is(FILE *file, uint64_t offset){
    fseek(file, offset, SEEK_SET);
    size_t len = 1;
    union tuple_header *temp_header = malloc(sizeof(union tuple_header));
    read_from_file(temp_header, file, sizeof(union tuple_header));
    while(temp_header->next){
        fseek(file, temp_header->next, SEEK_SET);
        read_from_file(temp_header, file, sizeof(union tuple_header));
        len++;
    }
    free(temp_header);
    return len;
}

enum file_read_status read_string_from_tuple(FILE *file, char **string, uint64_t pattern_size, uint64_t offset) {
    size_t str_len = how_long_string_is(file, offset);
    size_t rts = get_real_tuple_size(pattern_size);
    *string = malloc(str_len * rts);
    struct tuple *temp_tuple;
    for(size_t iter = 0; iter < str_len; iter++) {
        fseek(file, offset, SEEK_SET);
        read_tuple(&temp_tuple, file, pattern_size);
        offset = temp_tuple->header.next;
        strncpy((*string) + rts * iter, (char *) temp_tuple->data, rts);
        free_tuple(temp_tuple);
    }
    return 0;
}

enum crud_operation_status swap_tuple_to(FILE *file, uint64_t pos_from, uint64_t pos_to, size_t tuple_size) {
    if (pos_from != pos_to) {
        fseek(file, pos_from, SEEK_SET);
        uint64_t *buffer = malloc(tuple_size);

        read_from_file(buffer, file, tuple_size);
        fseek(file, pos_to, SEEK_SET);
        write_to_file(buffer, file, tuple_size);

        struct tree_header *header = malloc(sizeof(struct tree_header));
        size_t fpos;
        read_tree_header_no_id(header, file, &fpos);

        uint64_t id;
        fseek(file, pos_to, SEEK_SET);
        if (valid_offset_id(file, fpos, buffer[1], pos_from)) {
            for (size_t iter = 0; iter < header->subheader->pattern_size; iter++){
                if (header->pattern[iter]->header->type == STRING_TYPE) {
                    fseek(file, buffer[iter + (sizeof(union tuple_header)/SINGLE_TUPLE_VALUE_SIZE)], SEEK_SET);
                    write_to_file(&pos_to, file, sizeof(uint64_t));
                }
            }
            write_id_value(file, fpos, pos_to, buffer[1]);
        } else {
            uint64_t offset = buffer[0];

            union tuple_header *temp_header = malloc(sizeof(union tuple_header));
            fseek(file, offset, SEEK_SET);
            read_from_file(temp_header, file, sizeof(union tuple_header));

            if (temp_header->next == pos_from) {
                fseek(file, offset, SEEK_SET);
                temp_header->next = pos_to;
                write_to_file(temp_header, file, sizeof(union tuple_header));
            } else {
                uint64_t *data = malloc(tuple_size - sizeof(union tuple_header));
                read_from_file(data, file, tuple_size - sizeof(union tuple_header));
                for(uint64_t iter = 0; iter < tuple_size - sizeof(union tuple_header); iter ++){
                    if (data[iter] == pos_from) {
                        data[iter] = pos_to;
                        fseek(file, offset + sizeof(union tuple_header), SEEK_SET);
                        write_to_file(data, file, tuple_size - sizeof(union tuple_header));
                        break;
                    }
                }
                free(data);
            }
            free(temp_header);
        }
        free_tree_header_no_id(header);
        free(buffer);
    }
    fseek(file, 0, SEEK_END);
    return CRUD_OK;
}

enum crud_operation_status swap_with_last(FILE *file, uint64_t offset, uint64_t size) {
    uint64_t full_size = size + sizeof(union tuple_header);
    fseek(file, 0, SEEK_END);
    fseek(file, -(int64_t) full_size, SEEK_END);
    enum crud_operation_status status = swap_tuple_to(file, ftell(file), offset, full_size);
    fseek(file, -(int64_t) full_size, SEEK_END);
    status |= ftruncate(fileno(file), ftell(file));
    return status;
}

enum crud_operation_status insert_new_tuple(FILE *file, struct tuple *tuple, size_t full_tuple_size, uint64_t *tuple_pos) {
    fseek(file, 0, SEEK_END);
    *tuple_pos = ftell(file);
    int fd = fileno(file);
    enum crud_operation_status status = ftruncate(fd, ftell(file) + full_tuple_size);
    status |= write_tuple(file, tuple, full_tuple_size - sizeof(union tuple_header));
    return  status;
}

enum crud_operation_status insert_string_tuple(FILE *file, char *string, size_t tuple_size, uint64_t par_pos, uint64_t *str_pos) {
    size_t len = strlen(string);
    size_t count = len / tuple_size + (len % tuple_size ? 1 : 0);
    struct tuple *temp_tuple = malloc(sizeof(struct tuple));
    char *temp_tuple_content = string;
    size_t pos = (size_t) ftell(file);
    uint64_t fake_pos;
    fseek(file, 0, SEEK_END);
    *str_pos = ftell(file);
    for (size_t iter = 0; count > iter; iter++) {
        if (count - 1 == iter) {
            temp_tuple->header.next = 0;
        } else {
            temp_tuple->header.next = pos + (tuple_size + sizeof(union tuple_header)) * (iter + 1);
        }
        if (0 == iter) {
            temp_tuple->header.prev = par_pos;
        } else {
            temp_tuple->header.prev = pos + (tuple_size + sizeof(union tuple_header)) * (iter - 1);
        }
        temp_tuple->data = (uint64_t *) (temp_tuple_content + tuple_size * iter);
        insert_new_tuple(file, temp_tuple, tuple_size + sizeof(union tuple_header), &fake_pos);
    }
    free(temp_tuple);
    return 0;
}

enum crud_operation_status change_string_tuple(FILE *file, uint64_t offset, char *new_string, uint64_t size) {
    struct tuple *cur_tuple;
    fseek(file, offset, SEEK_SET);
    read_tuple(&cur_tuple, file, size);
    int64_t len = strlen(new_string);
    uint64_t old_offset = offset;
    do {
        free_tuple(cur_tuple);
        offset = old_offset;
        fseek(file, offset, SEEK_SET);
        read_tuple(&cur_tuple, file, size);
        fseek(file, offset, SEEK_SET);
        cur_tuple->data = (uint64_t * )(new_string);
        new_string = new_string + size;
        write_tuple(file, cur_tuple, size);
        old_offset = cur_tuple->header.next;
        len -= size;
    } while (cur_tuple->header.next && len > 0);
    uint64_t fpos;
    if (len > 0) {
        insert_string_tuple(file, new_string, size, offset,&fpos);
        cur_tuple->header.next = fpos;
        fseek(file, offset, SEEK_SET);
        write_tuple(file, cur_tuple, size);
        free(cur_tuple);
    }

    return CRUD_OK;
}

enum crud_operation_status remove_string_from_file(FILE* file, uint64_t offset, uint64_t size) {
    union tuple_header cur_header;
    uint64_t temp_offset = offset;
    while(temp_offset != NULL_VALUE) {
        fseek(file, offset, SEEK_SET);
        read_from_file(&cur_header, file, sizeof(union tuple_header));
        temp_offset = cur_header.next;
        swap_with_last(file, offset, size);
        offset = temp_offset;
    }
    return CRUD_OK;
}