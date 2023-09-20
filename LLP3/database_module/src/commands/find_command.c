#include "commands/find_command.h"

static void print_result_list(FILE *file, struct result_list_tuple *list){
    if (list == NULL) printf("No result\n");
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    uint64_t count = 0;
    char *s;
    while(list != NULL) {
        printf("--- TUPLE %3ld ---\n", count++);
        for (uint64_t iter = 0; iter < header->subheader->pattern_size; iter++) {
            switch (header->pattern[iter]->header->type) {
                case INTEGER_TYPE: printf("%-20s: %ld\n", header->pattern[iter]->key_value, list->value->data[iter]); break;
                case BOOLEAN_TYPE: printf("%-20s: %d\n", header->pattern[iter]->key_value, list->value->data[iter] != 0); break;
                case FLOAT_TYPE: printf("%-20s: %lf\n", header->pattern[iter]->key_value, (float) list->value->data[iter]); break;
                default:
                    read_string_from_tuple(file, &s, header->subheader->pattern_size, list->value->data[iter]);
                    printf("%-20s: %s\n", header->pattern[iter]->key_value, s);
                    free(s);
                    break;
            }
        }
        list = list->next;
    }
    free_tree_header(header);
}

enum crud_operation_status find_all_res(FILE *file) {
    struct result_list_tuple *list = NULL;
    find_all(file, &list);
    print_result_list(file, list);
    free_result_list(list);
    return CRUD_OK;
}

enum crud_operation_status find_single(FILE *file) {
    printf("Enter id\n");
    printf("%-20s:", "Id");
    uint64_t  id;
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    enum crud_operation_status status = !scanf("%ld", &id);
    uint64_t *fields;
    if (header->subheader->cur_id < id){
        printf("Too large id\n");
        free_tree_header(header);
        return CRUD_INVALID;
    }
    status = get_tuple(file, &fields, id);
    if (status) {
        printf("No result\n");
        free_tree_header(header);
        return CRUD_INVALID;
    }
    for (uint64_t iter = 0; iter < header->subheader->pattern_size; iter++) {
        switch (header->pattern[iter]->header->type) {
            case INTEGER_TYPE: printf("%-20s: %ld\n", header->pattern[iter]->key_value, fields[iter]); break;
            case BOOLEAN_TYPE: printf("%-20s: %d\n", header->pattern[iter]->key_value, fields[iter] != 0); break;
            case FLOAT_TYPE: printf("%-20s: %lf\n", header->pattern[iter]->key_value, (double) fields[iter]); break;
            default:
                printf("%-20s: %s\n", header->pattern[iter]->key_value, (char *)fields[iter]);
                free((char *)fields[iter]);
                break;
        }
    }
    free(fields);
    free_tree_header(header);
    return CRUD_OK;
}

enum crud_operation_status find_parent(FILE *file) {
    printf("Enter parent id\n");
    printf("%-20s:", "Id");
    uint64_t  id;
    enum crud_operation_status status = !scanf("%ld", &id);
    struct result_list_tuple *list = NULL;
    find_by_parent(file, id, &list);
    print_result_list(file, list);
    free_result_list(list);
    return status ? CRUD_INVALID : CRUD_OK;
}

enum crud_operation_status find_condition(FILE *file) {
    printf("Choose field\n");
    struct tree_header *header = malloc(sizeof(struct tree_header));
    read_tree_header_np(header, file);
    for (uint64_t iter = 0; iter < header->subheader->pattern_size; iter++) {
        printf("%ld. %s\n", iter, header->pattern[iter]->key_value);
    }
    uint64_t  number;
    printf("Field: ");
    enum crud_operation_status status = !scanf("%ld", &number);
    if (number > header->subheader->pattern_size - 1) return CRUD_INVALID;
    printf("Enter expression to equaling\n");
    printf("Exp: ");
    uint64_t value;
    char s[BUFFER_FIELD_SIZE];
    switch (header->pattern[number]->header->type) {
        case INTEGER_TYPE: status |= !scanf("%ld", &value);; break;
        case BOOLEAN_TYPE: status |= !scanf("%ld", &value);; break;
        case FLOAT_TYPE: status |= !scanf("%lf", (double *) &value);; break;
        default: status |= !scanf("%s", s); value = (uint64_t) s; break;
    }
    struct result_list_tuple *list = NULL;
    //find_by_field(file, number, &value, &list);
    print_result_list(file, list);
    free_tree_header(header);
    free_result_list(list);
    return status ? CRUD_INVALID : CRUD_OK;
}

enum crud_operation_status find_execute(FILE *file){
    printf("Choose type of find\n");
    printf("1. Get full tree\n");
    printf("2. Find single by id\n");
    printf("3. Find by parent\n");
    printf("4. Find by condition\n");
    printf("%-20s:", "Type");
    uint64_t option;
    enum crud_operation_status status = !scanf("%ld", &option);
    switch (option) {
        case 1: status |= find_all_res(file); break;
        case 2: status |= find_single(file); break;
        case 3: status |= find_parent(file); break;
        case 4: status |= find_condition(file); break;
        default: printf("Incorrect type"); status =  CRUD_INVALID;
    }

    return status;
}