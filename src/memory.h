#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "prelude.h"
#include "token.h"

#define SIZE_FILE   184
#define SIZE_TOKENS 32
#define SIZE_BUFFER 20
#define SIZE_BYTES  26

typedef struct {
    char  file[SIZE_FILE];
    usize file_index;
    Token tokens[SIZE_TOKENS];
    usize tokens_index;
    char  buffer[SIZE_BUFFER];
    usize buffer_index;
    u8    bytes[SIZE_BYTES];
    usize bytes_index;
} Memory;

static void set_file(Memory* memory, const char* filename) {
    File* file = fopen(filename, "r");
    EXIT_IF(!file);
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    EXIT_IF(sizeof(memory->file) <= size);
    rewind(file);
    EXIT_IF(fread(&memory->file, sizeof(char), size, file) != size);
    memory->file[size] = '\0';
    memory->file_index = size;
    fclose(file);
}

#endif
