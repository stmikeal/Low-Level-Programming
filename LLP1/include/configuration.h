#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define FILE_GRANULARITY 4
#define BIG_ENDIAN_SIGNATURE 0xFFFE
/* size of one value in tuple [bytes] */
#define SINGLE_TUPLE_VALUE_SIZE 8
/* size of offset in tuple [bytes] */
#define OFFSET_VALUE_SIZE 8
/* minimal id array size [tuple sizes] */
#define MIN_ID_ARRAY_SIZE 1
#define MINIMAL_TUPLE_SIZE 256
/* size of string to read command */
#define BUFFER_COMMAND_SIZE 256
/* size of string to read field value */
#define BUFFER_FIELD_SIZE 1024

/* default value to fill empty containers */
#define NULL_VALUE 0

/* basic types that can contains in tuple  */
#define BOOLEAN_TYPE 0
#define INTEGER_TYPE 1
#define FLOAT_TYPE 2
#define STRING_TYPE 3

/* Prime numbers to hash */
#define AHASH 54059
#define BHASH 76963
#define CHASH 86969
#define INITHASH 37

#include "adv_tools/file_manager.h"

struct file_config {
    char *filename;
    uint8_t generator_flag;
    char *generator_filename;
};

struct file_config *new_file_config();

#endif
