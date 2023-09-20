#ifndef TOOLS_H
#define TOOLS_H
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "configuration.h"
#include "time.h"
#include "crud_tools/crud.h"

int32_t read_command_line(char *buffer);
uint64_t get_hash(char *string);
void init_file(FILE *file);
void time_add_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id);
void time_add_get_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id);
void time_add_get_cond_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id);
void time_add_remove_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id);
void time_remove_wrapper(FILE *file, uint64_t id);
void time_add_update_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id, uint64_t id);
void size_add_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id);
void time_add_get_by_parent_wrapper(FILE *file, uint64_t *fields, uint64_t parent_id);

#endif
