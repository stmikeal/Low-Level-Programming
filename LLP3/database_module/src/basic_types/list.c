#include "basic_types/list.h"

void append_to_uint64_list(uint64_t id, struct uint64_list **result){
    if ((*result) == NULL) {
        *result = malloc(sizeof(struct uint64_list));
        (*result)->next = NULL;
    } else {
        struct uint64_list *new_result = malloc(sizeof(struct uint64_list));
        new_result->next = *result;
        *result = new_result;
    }
    (*result)->value= id;
}

void free_uint64_list(struct uint64_list *result){
    if (result != NULL){
        struct uint64_list *next;
        while(result != NULL){
            next = result->next;
            free(result);
            result = next;
        }
    }
}

void append_to_result_list(struct tuple **tuple_to_add, struct result_list_tuple **result, uint64_t id) {
    if ((*result) == NULL) {
        *result = malloc(sizeof(struct result_list_tuple));
        (*result)->next = NULL;
    } else {
        struct result_list_tuple *new_result = malloc(sizeof(struct result_list_tuple));
        new_result->next = *result;
        *result = new_result;
    }
    (*result)->value = *tuple_to_add;
    (*result)->id = id;
}

void free_result_list(struct result_list_tuple *result) {
    if (result != NULL){
        struct result_list_tuple *next;
        while(result != NULL){
            next = result->next;
            free_tuple(result->value);
            free(result);
            result = next;
        }
    }
}