#include "XPATHParser.h"

#define MAX_COMMAND_SIZE 1024

struct view *parse(){
    char command[MAX_COMMAND_SIZE], *for_parse;
    enum states current_state;
    scanf("%s", command);
    for_parse = command + 1;
    struct view *view = parse_operation(command[0]);
    struct list_level *cur_level = create_list_level(0, 0 ,P_ROOT);
    if (*for_parse == '/'){
        current_state = S_NEXT;
    } else if (*for_parse){
        cur_level->place = P_FREE;
        command[0] = '/';
        for_parse = command;
        current_state = S_NEXT;
    } else {
        for_parse = command;
        current_state = S_ERROR;
    }
    while (for_parse && current_state != S_ERROR) {
        current_state = parse_state(current_state, &for_parse, cur_level);
        if (current_state == S_NEXT && *for_parse) {
            cur_level->next = view->tree;
            view->tree = cur_level;
            cur_level = create_list_level(0, 0, P_RELATIVE);
        }
    }
    if (cur_level) {
        cur_level->next = view->tree;
        view->tree = cur_level;
    }
    return view;
}