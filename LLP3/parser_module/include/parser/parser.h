#ifndef PARSER_H
#define PARSER_H
#include "finite_state.h"

struct view *parse_operation(char op_char);

enum states parse_state(enum states cur_state, char **string, struct list_level *level);

#endif
