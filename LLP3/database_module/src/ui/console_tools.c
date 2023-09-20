#include "ui/console_tools.h"
#include "message.pb.h"
#include "view.h"
#include "crud_tools/crud.h"

uint64_t get_hash(char *string) {
    uint64_t value = INITHASH;
    while(*string) value = (value * AHASH) ^ (*(string++) * BHASH);
    return value % CHASH;
}

void init_file(FILE *file) {
    printf("We notice, you haven't got initialized file yet.\n");
    printf("So, you need to make file pattern...\n");
    printf("Firstly, enter how many fields in each tuple do you need: ");
    size_t count;
    enum crud_operation_status status = !scanf("%ld", &count);
    printf("Then you need to describe each field:\n");
    char *str;
    char **str_array = malloc(count * sizeof(char *));
    uint32_t type;
    uint32_t *types = malloc(count * sizeof(uint32_t));
    size_t *sizes = malloc(count * sizeof(size_t));
    size_t temp_size;
    for(size_t iter = 0; iter < count; iter++){
        printf("--- Field %-3zu ---\n", iter);
        str = malloc(BUFFER_FIELD_SIZE);
        printf("Enter field name: ");
        status |= !scanf("%s", str);
        str_array[iter] = str;
        temp_size = strlen(str);
        sizes[iter] = temp_size + (!(temp_size % FILE_GRANULARITY) ? 1 : 0);
        printf("%d. Boolean\n", BOOLEAN_TYPE);
        printf("%d. Integer\n", INTEGER_TYPE);
        printf("%d. Float\n", FLOAT_TYPE);
        printf("%d. String\n", STRING_TYPE);
        printf("Choose field type by entering number: ");
        status |= !scanf("%d", &type);
        types[iter] = type;
    }
    init_empty_file(file, str_array, types, count, sizes);
    for(size_t iter = 0; iter < count; iter++) free(str_array[iter]);
    free(str_array);
    free(sizes);
    free(types);
    if (status) printf("Invalid init of file!");
}

size_t find_by_level(Level level, struct result_list_tuple **list) {

}

size_t do_filter(FILE *file, Query_tree tree, struct result_list_tuple **list) {
    uint64_t parent_id = -1;
    size_t len = 0;
    if (tree.level_count > 1) {
        len = find_by_level(tree.level[0], list);
        if (len > 1) return 0;
        parent_id = (*list)->id;
        free_result_list(*list);
    }
    find_by_level(tree.level[tree.level_count > 1 ? 1 : 0], list);
    if (parent_id >= 0) {

    }
}

uint64_t parse_request(char *filename, Query_tree tree, struct result_list_tuple **list){
    FILE *file;
    uint64_t code = 0;

    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);

    open_file_anyway(&file, filename);
    size_t len = do_filter(file, tree, list);

    switch (tree.command) {
        case CRUD_INSERT:
            if (len != 1) {
                code = 42; // Return code mean not unique result;
                break;
            }
            char *str = "null";
            uint64_t *pattern = malloc(header->subheader->pattern_size);
            for(size_t i = 0; i < header->subheader->pattern_size; i++){
                if (header->pattern[i]->header->type == STRING_TYPE) {
                    pattern[i] = (uint64_t) str;
                } else {
                    pattern[i] = 0;
                }
            }
            code = add_tuple(file, pattern, len);
            break;
        case CRUD_DELETE:
            if (!len) {
                code = 1; // Return code mean no result present;
                break;
            }
            struct result_list_tuple *copy = *list;
            while(copy != NULL){
                remove_tuple(file, copy->id);
                copy = copy->next;
            }
            break;
        case CRUD_UPDATE:
            if (!len) {
                code = 1; // Return code mean no result present;
                break;
            }
            int64_t index = -1;
            for(int64_t i = 0; i < header->subheader->pattern_size; i++){
                if (strcmp(header->pattern[i]->key_value, tree.name) == 0) {
                    index = i;
                    break;
                }
            }
            if (index < 0) return 13; // Return code mean invalid field name;
            struct result_list_tuple *copy2 = *list;
            while(copy2 != NULL){
                update_tuple(file, index, (uint64_t *)&tree.value, copy2->id);
                copy2 = copy2->next;
            }
        default: break;
    }
    free_result_list(*list);
    close_file(file);
    return code;
}