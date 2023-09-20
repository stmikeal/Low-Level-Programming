#include "commands/add_command.h"

enum crud_operation_status add_execute(FILE *file){
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    printf("Enter fields of new tuple\n");
    uint64_t *pattern = malloc(header->subheader->pattern_size);
    enum crud_operation_status status = CRUD_OK;
    for(uint64_t iter = 0; iter < header->subheader->pattern_size; iter++) {
        printf("%-20s:", header->pattern[iter]->key_value);
        char *s;
        switch (header->pattern[iter]->header->type) {
            case BOOLEAN_TYPE:
            case INTEGER_TYPE:
            case FLOAT_TYPE: status |= !scanf("%ld", pattern + iter); break;
            default: s = malloc(BUFFER_FIELD_SIZE); status |= !scanf("%s", s); pattern[iter] = (uint64_t) s; break;
        }

    }
    uint64_t parent_id;
    printf("%-20s:", "Write parent id");
    status |= !scanf("%ld", &parent_id);
    status |= add_tuple(file, pattern, parent_id);
    if (status == CRUD_OK) printf("TUPLE %3ld INSERTED\n", header->subheader->cur_id);
    else printf("NOT INSERTED\n");
    free_tree_header(header);
    return status;
}