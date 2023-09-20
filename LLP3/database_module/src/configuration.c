#include "configuration.h"

struct file_config *new_file_config() {
    struct file_config *config = malloc(sizeof(struct file_config));
    config->filename=NULL_VALUE;
    config->generator_flag=NULL_VALUE;
    config->generator_filename=NULL_VALUE;
    return  config;
};
