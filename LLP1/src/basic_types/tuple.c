#include "basic_types/tuple.h"

enum file_read_status read_tuple(struct tuple **tuple, FILE *file, uint64_t pattern_size) {
    *tuple = (struct tuple *) malloc(sizeof(struct tuple));

    enum file_read_status code = read_from_file(*tuple, file, sizeof(union tuple_header));

    (*tuple)->data = (uint64_t *) malloc(get_real_tuple_size(pattern_size));
    code |= read_from_file((*tuple)->data, file, get_real_tuple_size(pattern_size));

    return code;
}

enum file_write_status write_tuple(FILE *file, struct tuple *tuple, size_t tuple_size) {
    enum file_write_status code = write_to_file(tuple, file, sizeof(union  tuple_header));
    code |= write_to_file(tuple->data, file, tuple_size);
    return code;
}

void free_tuple(struct tuple *tuple) {
    free(tuple->data);
    free(tuple);
}

