#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "parse.h"
#include "token.h"

#define SIZE_FILE   256
#define SIZE_TOKENS 64
#define SIZE_BUFFER 32
#define SIZE_INSTS  16
#define SIZE_LABELS 4
#define SIZE_BYTES  64

typedef struct {
    char  file[SIZE_FILE];
    usize file_index;
    Token tokens[SIZE_TOKENS];
    usize tokens_index;
    char  buffer[SIZE_BUFFER];
    usize buffer_index;
    Inst  insts[SIZE_INSTS];
    usize insts_index;
    Label labels[SIZE_LABELS];
    usize labels_index;
    u8    bytes[SIZE_BYTES];
    usize bytes_index;
} Memory;

static void set_file(Memory* memory, const char* filename) {
    File* file = fopen(filename, "r");
    EXIT_IF(!file);
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    EXIT_IF(SIZE_FILE <= size);
    rewind(file);
    EXIT_IF(fread(&memory->file, sizeof(char), size, file) != size);
    memory->file[size] = '\0';
    memory->file_index = size;
    fclose(file);
}

static void reset(Memory* memory) {
    memory->file_index = 0;
    memory->tokens_index = 0;
    memory->buffer_index = 0;
    memory->insts_index = 0;
    memory->labels_index = 0;
    memory->bytes_index = 0;
}

#endif
