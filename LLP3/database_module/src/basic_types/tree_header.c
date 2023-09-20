#include "basic_types/tree_header.h"

static enum file_read_status read_tree_subheader(struct tree_subheader *header, FILE *file) {
    enum file_read_status code = read_from_file(header, file, sizeof(struct tree_subheader));
    return code;
}

static enum file_read_status read_key(struct key *key, FILE *file) {
    struct key_header *header = malloc(sizeof(struct key_header));
    enum file_read_status code = read_from_file(header, file, sizeof(struct key_header));
    key->header = header;

    char *key_value = (char *) malloc(get_real_string_size(header->size));
    code |= read_from_file(key_value, file, header->size);
    key->key_value = key_value;

    return code;
}

enum file_read_status read_tree_header(struct tree_header *header, FILE *file, size_t *fpos) {
    fseek(file, 0, SEEK_SET);

    header->subheader = (struct tree_subheader *) malloc(sizeof(struct tree_subheader));
    enum file_read_status code = read_tree_subheader(header->subheader , file);

    struct key **array_of_key = malloc(sizeof(struct key *) * header->subheader->pattern_size);
    header->pattern = array_of_key;
    for (size_t iter = header->subheader->pattern_size; iter-- > 0; array_of_key++){
        *array_of_key = malloc(sizeof(struct key));
        code |= read_key(*array_of_key, file);
    }

    size_t real_id_array_size = get_real_id_array_size(header->subheader->pattern_size, header->subheader->cur_id);
    header->id_sequence = (uint64_t *) malloc(sizeof(uint64_t)*real_id_array_size);
    code |= read_from_file(header->id_sequence, file, sizeof(uint64_t)*real_id_array_size);

    *fpos = ftell(file);

    return code;
}

enum file_read_status read_tree_header_no_id(struct tree_header *header, FILE *file, size_t *fpos) {
    fseek(file, 0, SEEK_SET);

    header->subheader = (struct tree_subheader *) malloc(sizeof(struct tree_subheader));
    enum file_read_status code = read_tree_subheader(header->subheader , file);

    struct key **array_of_key = malloc(sizeof(struct key *) * header->subheader->pattern_size);
    header->pattern = array_of_key;
    for (size_t iter = header->subheader->pattern_size; iter-- > 0; array_of_key++){
        *array_of_key = malloc(sizeof(struct key));
        code |= read_key(*array_of_key, file);
    }

    *fpos = ftell(file);

    return code;
}



enum file_read_status read_tree_header_np(struct tree_header *header, FILE *file) {
    size_t pos;
    return read_tree_header(header, file, &pos);
}

void free_tree_header_no_id(struct tree_header *header) {
    for (size_t iter = 0; iter < header->subheader->pattern_size; iter++){
        free(header->pattern[iter]->header);
        free(header->pattern[iter]->key_value);
        free(header->pattern[iter]);
    }
    free(header->pattern);
    free(header->subheader);
    free(header);
}

void free_tree_header(struct tree_header *header) {
    free(header->id_sequence);
    free_tree_header_no_id(header);
}

static enum file_write_status write_tree_subheader(FILE *file, struct tree_subheader *subheader){
    enum file_write_status code = write_to_file(subheader, file, sizeof(struct tree_subheader));
    return code;
}

static enum file_write_status write_pattern(FILE *file, struct key **pattern, size_t pattern_size){
    enum file_write_status code = NULL_VALUE;
    for (; pattern_size-- > 0; pattern++){
        code |= write_to_file((*pattern)->header, file, sizeof(struct key_header));
        code |= write_to_file((*pattern)->key_value, file, (*pattern)->header->size);
    }
    return code;
}

static enum file_write_status write_id_sequence(FILE *file, uint64_t *id_sequence, size_t size){
    enum file_write_status code = write_to_file(id_sequence, file, size);
    return code;
}

enum file_write_status write_tree_header(FILE *file, struct tree_header *header) {
    fseek(file, 0, SEEK_SET);

    enum file_write_status code = write_tree_subheader(file, header->subheader);

    code |= write_pattern(file, header->pattern, header->subheader->pattern_size);

    size_t real_id_array_size = get_real_id_array_size(header->subheader->pattern_size, header->subheader->cur_id);
    code |= write_id_sequence(file, header->id_sequence, sizeof(uint64_t)*real_id_array_size);

    return code;
}

enum file_write_status write_tree_header_no_id(FILE *file, struct tree_header *header) {
    fseek(file, 0, SEEK_SET);

    enum file_write_status code = write_tree_subheader(file, header->subheader);

    code |= write_pattern(file, header->pattern, header->subheader->pattern_size);

    return code;
}

enum file_write_status write_id_value(FILE *file, size_t fpos, uint64_t offset, size_t index) {
    fseek(file, fpos + index * sizeof(uint64_t), SEEK_SET);
    return write_to_file(&offset, file, sizeof(uint64_t));
}