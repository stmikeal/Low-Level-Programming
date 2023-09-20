#include "ui/interactive.h"
#include "message.pb.h"
void print_help();



enum commands_desc{
    ADD_COMMAND = 0,
    REMOVE_COMMAND,
    UPDATE_COMMAND,
    FIND_COMMAND,
    HELP_COMMAND,
    EXIT_COMMAND,
    WRONG_COMMAND
};

uint64_t commands_hash[6];

void init_params() {
    commands_hash[0] = get_hash("add");
    commands_hash[1] = get_hash("remove");
    commands_hash[2] = get_hash("update");
    commands_hash[3] = get_hash("find");
    commands_hash[4] = get_hash("help");
    commands_hash[5] = get_hash("exit");
}

enum commands_desc parse_command(char *command) {
    uint64_t hash = get_hash(command);
    for (size_t iter = 0; iter < 6; iter++){
        if (commands_hash[iter] == hash) return iter;
    }
    return WRONG_COMMAND;
}

static void print_need_help() {
    printf("Type 'help' for available commands info.\n");
}

static void print_program_header() {
    printf("Welcome to bigdata program!\n");
    printf("File opened successfully!\n");
    print_need_help();
}

static void print_wrong_command() {
    printf("You typed wrong command\n");
    print_need_help();
}

/*
int32_t interactive_mode(struct file_config *config) {
    FILE *file;
    enum file_open_status open_status = open_file_anyway(&file, config->filename);
    if (open_status == OPEN_FAILED) return open_status;

    init_params();
    print_program_header();
    if (open_status == OPEN_NEW) init_file(file);

    if (config->generator_flag) {
        FILE *gen_file = fopen(config->generator_filename, "r");
        char s[BUFFER_FIELD_SIZE];
        uint64_t parent;
        uint64_t code;
        uint64_t fields[2];
        uint64_t id =0;
        while(!feof(gen_file)){
            if(fscanf(gen_file, "%ld code=%ld name=%s\n", &parent, &code, s)){
                fields[0] = (uint64_t) s;
                fields[1] = code;
                add_tuple(file, fields, parent);
            }
        }
        close_file(gen_file);
    }

    char command[BUFFER_COMMAND_SIZE];
    while(1) {
        read_command_line(command);
        switch (parse_command(command)) {
            case ADD_COMMAND: add_execute(file); break;
            case REMOVE_COMMAND: remove_execute(file); break;
            case UPDATE_COMMAND: update_execute(file); break;
            case FIND_COMMAND: find_execute(file); break;
            case HELP_COMMAND: print_help();break;
            case EXIT_COMMAND: break;
            default: print_wrong_command(); break;
        }
    }


    close_file(file);

    return CRUD_OK;
}*/



void print_help() {
    printf("add - command to add new tuple\n");
    printf("remove - command to remove tuple by id\n");
    printf("update - update tuple by id\n");
    printf("find - find tuples by filter, id or view of all tuples\n");
    printf("exit - save and exit\n");
}