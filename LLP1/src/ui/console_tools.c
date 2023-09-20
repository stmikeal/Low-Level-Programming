#include "ui/console_tools.h"


const char *command_hint = ">>> ";

int32_t read_command_line(char *buffer) {
    printf("%s", command_hint);
    return !scanf("%s", buffer);
}

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

void time_add_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id){
    clock_t cl = clock();
    add_tuple(file, fields, parent_id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void size_add_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id){
    add_tuple(file, fields, parent_id);
    fseek(file, 0, SEEK_END);
    printf("%ld\n", ftell(file));
}

void time_add_get_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id){
    add_tuple(file, fields, parent_id);
    clock_t cl = clock();
    get_tuple(file, &fields, id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void time_add_get_by_parent_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id){
    add_tuple(file, fields, parent_id);
    struct result_list_tuple *list = NULL;
    clock_t cl = clock();
    find_by_parent(file, parent_id, &list);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
    free_result_list(list);
}

void time_add_get_cond_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id){
    add_tuple(file, fields, parent_id);
    uint64_t c = 3342;
    struct result_list_tuple *res;
    clock_t cl = clock();
    find_by_field(file, 1, &c, &res);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void time_add_remove_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id){
    add_tuple(file, fields, parent_id);
    clock_t cl = clock();
    remove_tuple(file, id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void time_remove_wrapper(FILE *file, uint64_t id){
    clock_t cl = clock();
    remove_tuple(file, id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}

void time_add_update_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id){
    add_tuple(file, fields, parent_id);
    uint64_t c = 3342;
    clock_t cl = clock();
    update_tuple(file, 1, &c, id);
    printf("%f\n", (double) (clock() - cl) / CLOCKS_PER_SEC);
}