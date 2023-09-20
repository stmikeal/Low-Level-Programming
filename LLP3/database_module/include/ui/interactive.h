#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "crud_tools/crud.h"
#include "adv_tools/file_manager.h"
#include <string.h>
#include "console_tools.h"
#include "commands/add_command.h"
#include "commands/find_command.h"
#include "commands/remove_command.h"
#include "commands/update_command.h"

int32_t interactive_mode(struct file_config *config);
int32_t generator_mode(char *filename, char *data_filename);

#endif
