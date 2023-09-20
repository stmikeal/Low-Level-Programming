#include "printer.h"

static void print_string(struct field *string){
    for (size_t iter = 0; iter < string->size; iter++){
        printf("%c", string->value[iter]);
    }
}

static void print_operator(struct operator *op, uint8_t number) {
    printf("----OPERATOR %d: ", number);
    switch (op->type){
        case STRING_TYPE:print_string(op->value.string); break;
        case INTEGER_TYPE: printf("%ld", op->value.integer); break;
        case FLOAT_TYPE: printf("%f", op->value.real); break;
        default: printf("UNKNOWN TYPE"); break;
    }
    if (op->field) printf(" (IS FIELD)");
    printf("\n");
}

static void print_filter(struct filter_list *list) {
    printf("-FILTERS\n");
    size_t level = 1;
    if (!list) return;
    struct comparator_list *comp_list;
    while (list) {

        printf("--FILTER: %zu\n", level++);
        printf("--IS NEGATIVE: %d\n", list->value->negative);
        size_t comp_level = 1;
        comp_list = list->value->comparators;
        if (comp_list) {
            printf("---COMPARATORS---\n");
            while (comp_list) {
                printf("----COMPARATOR: %zu\n", comp_level++);
                printf("----IS NEGATIVE: %d\n", comp_list->value->negative);
                if (comp_list->value->is_true) {
                    printf("----COMPARATOR IS TRUE\n");
                } else {
                    print_operator(comp_list->value->op1, 1);
                    printf("----OPERATION: %c\n", comp_list->value->operation);
                    print_operator(comp_list->value->op2, 2);
                }
                printf("----END OF COMPARATOR\n");
                comp_list = comp_list->next;
            }
            printf("---END OF COMPARATORS\n");
        }
        printf("--END OF FILTER\n");
        list = list->next;
    }
    printf("-END OF FILTERS\n");
}

static void print_view(struct view *view) {
    if (!view) return;
    printf("--------------------------------\n");
    printf("OPERATION: %c\n", view->crud_operation);
    printf("--------------------------------\n");
    size_t level = 1;
    struct list_level *list = view->tree;
    while (list) {
        printf("LEVEL: %zu\n", level++);
        switch (list->place) {
            case P_FREE: printf("FREE RELATION\n"); break;
            case P_ROOT: printf("ROOT RELATION\n"); break;
            case P_RELATIVE: printf("CHILD RELATION\n"); break;
            default: printf("UNKNOWN RELATION\n"); break;
        }
        printf("IS NEGATIVE: %d\n", list->negative);
        if (list->any) {
            printf("ANY ID: *\n");
        } else {
            printf("ID: %ld\n", list->value->element);
        }
        print_filter(list->filters);
        printf("--------------------------------\n");
        list = list->next;
    }
}
static void print_error(){
    printf("Syntax error!");
}

static void print_wtf_happened(){
    printf("Unsupported exception!");
}

void print_result(enum states final_state, char *res_string, struct view *view){
    if (final_state == S_ERROR && !*res_string){
        print_view(view);
    } else if (final_state == S_ERROR){
        print_error();
    } else {
        print_wtf_happened();
    }
}

