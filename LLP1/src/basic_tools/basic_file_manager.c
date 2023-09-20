#include "basic_tools/basic_file_manager.h"

enum file_read_status read_from_file(void *buffer, FILE *file, size_t size) {
    size_t length = fread(buffer, size, 1, file);
    enum file_read_status code = READ_OK;
    if (length < 1) {
        code = READ_INVALID;
    } else if (sizeof(buffer) < length) {
        code = READ_END_OF_FILE;
    }
    return code;
}

enum file_write_status write_to_file(void *buffer, FILE *file, size_t size) {
    size_t length = fwrite(buffer, size, 1, file);
    enum file_write_status code = WRITE_OK;
    if (length < 1) {
        code = WRITE_WRONG_INTEGRITY;
    } else if (sizeof(buffer) < length) {
        code = WRITE_INVALID;
    }
    return code;
}

static enum file_open_status open_file(char *filename, FILE **file, char *open_descriptor){
    *file = fopen(filename, open_descriptor);
    enum file_open_status code = OPEN_OK;
    if (*file==NULL){
        code = OPEN_FAILED;
    }
    return code;
}

enum file_open_status open_exist_file(char *filename, FILE **file){
    return open_file(filename, file, "r+b");
}

enum file_open_status open_new_file(char *filename, FILE **file){
    if (!open_file(filename, file, "w+b")) return OPEN_NEW;
    else return OPEN_FAILED;
}

enum file_open_status open_file_write(char *filename, FILE **file){
    if (!open_file(filename, file, "w")) return OPEN_NEW;
    else return OPEN_FAILED;
}

void close_file(FILE *file){
    fclose(file);
}

enum file_open_status open_empty_file(char *filename, FILE **file){
    open_file(filename, file, "w");
    close_file(*file);
    return open_file(filename, file, "r+");
}
