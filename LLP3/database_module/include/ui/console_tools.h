#ifndef TOOLS_H
#define TOOLS_H
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "configuration.h"
#include "time.h"
#include "crud_tools/crud.h"
#include "message.pb.h"

int32_t read_command_line(char *buffer);
uint64_t get_hash(char *string);
void init_file(FILE *file);
uint64_t parse_request(char *filename, Query_tree tree, struct result_list_tuple **list);
#endif
