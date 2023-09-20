#include "basic_tools/basic_convert.h"

struct map_data {
    void *ptr;
    size_t size;
};

struct map_data map[10000] = {0};
size_t glob_size = 0;
size_t iter = 0;

size_t get_real_tuple_size(uint64_t pattern_size) {
    return pattern_size * SINGLE_TUPLE_VALUE_SIZE < MINIMAL_TUPLE_SIZE
           ? MINIMAL_TUPLE_SIZE
           : pattern_size * SINGLE_TUPLE_VALUE_SIZE;
}

static uint64_t max(uint64_t n1, uint64_t n2) {
    if (n1 > n2) return n1;
    return n2;
}

size_t get_real_id_array_size(uint64_t pattern_size, uint64_t cur_id){
    size_t real_tuple_size = get_real_tuple_size(pattern_size) + sizeof(union tuple_header);
    if (cur_id == 0) cur_id++;
    size_t whole = (cur_id * OFFSET_VALUE_SIZE / real_tuple_size);
    size_t frac = (cur_id * OFFSET_VALUE_SIZE % real_tuple_size ? 1: 0);
    size_t value = max( (frac + whole) * real_tuple_size / OFFSET_VALUE_SIZE, MIN_ID_ARRAY_SIZE * real_tuple_size / OFFSET_VALUE_SIZE);
    return value;
}

size_t get_real_string_size(size_t len) {
    return (len/FILE_GRANULARITY + (len % FILE_GRANULARITY ? 1: 0)) * FILE_GRANULARITY;
}

void *malloc_test(size_t size){
    glob_size += size;
    void *ptr = malloc(size);
    map[iter].ptr = ptr;
    map[iter++].size = size;
    return ptr;
}

void free_test(void *ptr){
    free(ptr);
    for(size_t i = 0; i < 10000; i++) {
        if (map[i].ptr == ptr) {
            glob_size -= map[i].size;
            map[i].ptr = 0;
            break;
        }
    }
}

void print_ram() {
    printf("%zu\n", glob_size);
}