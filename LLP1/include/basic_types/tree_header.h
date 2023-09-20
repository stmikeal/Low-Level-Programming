#ifndef TREE_HEADER_H
#define TREE_HEADER_H

#include <stdio.h>
#include <inttypes.h>
#include "basic_tools/basic_file_manager.h"
#include "basic_tools/basic_convert.h"

/**
 *  Определение типов данных
 */

struct tree_subheader {
    uint64_t ASCII_signature;
    uint64_t root_offset;
    uint64_t first_seq;
    uint64_t second_seq;
    uint64_t cur_id;
    uint64_t pattern_size;
};

#pragma pack(push, 4)
struct key_header {
    uint32_t size;
    uint32_t type;
};
struct key {
    struct key_header *header;
    char *key_value;
};
#pragma pack(pop)

struct tree_header {
    struct tree_subheader *subheader;
    struct key **pattern;
    uint64_t *id_sequence;
};

/**
 *  Определение функций
 */

enum file_read_status read_tree_header(struct tree_header *header, FILE *file, size_t *fpos);
enum file_read_status read_tree_header_np(struct tree_header *header, FILE *file);
void free_tree_header(struct tree_header *header);
enum file_write_status write_tree_header(FILE *file, struct tree_header *header);
enum file_read_status read_tree_header_no_id(struct tree_header *header, FILE *file, size_t *fpos);
enum file_write_status write_id_value(FILE *file, size_t fpos, uint64_t offset, size_t index);
enum file_write_status write_tree_header_no_id(FILE *file, struct tree_header *header);
void free_tree_header_no_id(struct tree_header *header);

#endif
