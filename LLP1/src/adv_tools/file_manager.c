#include "adv_tools/file_manager.h"

enum file_write_status init_empty_file(FILE *file, char **pattern, uint32_t *types, size_t pattern_size, size_t *key_sizes) {
    fseek(file, 0, SEEK_SET);
    struct tree_header *header = (struct tree_header *) malloc(sizeof(struct tree_header));
    generate_empty_tree_header(pattern, types, pattern_size, key_sizes, header);
    enum file_write_status code =  write_tree_header(file, header);
    free_tree_header(header);
    return  code;
}

enum file_open_status open_file_anyway(FILE **file, char *filename){
    enum file_open_status code = open_exist_file(filename, file);
    if (code == OPEN_FAILED) {
        code = open_new_file(filename, file);
    }
    return code;
}
