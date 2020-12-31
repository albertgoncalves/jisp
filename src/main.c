#include "prelude.h"

// NOTE: See `https://bernsteinbear.com/blog/compiling-a-lisp-1/`.

#include <string.h>
#include <sys/mman.h>

#define SIZE_BUFFER 56

typedef struct {
    u8    buffer[SIZE_BUFFER];
    usize buffer_index;
} Memory;

typedef struct {
    void* buffer;
} Program;

typedef enum {
    OP_MOV_EAX = 0xB8,
    OP_RET = 0xC3,
} OpCode;

typedef i32 (*JitFn)(void);

static void emit_op_code(Memory* memory, OpCode op_code) {
    EXIT_IF(SIZE_BUFFER <= memory->buffer_index);
    memory->buffer[memory->buffer_index++] = op_code;
}

static void emit_i32(Memory* memory, i32 value) {
    usize index = memory->buffer_index + sizeof(i32);
    EXIT_IF(SIZE_BUFFER < index);
    memcpy(&memory->buffer[memory->buffer_index], &value, sizeof(i32));
    memory->buffer_index = index;
}

static Program transform(Memory* memory) {
    Program program = {0};
    program.buffer = mmap(NULL,
                          memory->buffer_index,
                          PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_PRIVATE,
                          -1,
                          0);
    EXIT_IF(program.buffer == MAP_FAILED);
    memcpy(program.buffer, &memory->buffer, memory->buffer_index);
    EXIT_IF(mprotect(program.buffer, memory->buffer_index, PROT_EXEC));
    return program;
}

i32 main(void) {
    printf("sizeof(Memory)  : %zu\n"
           "sizeof(Program) : %zu\n"
           "\n",
           sizeof(Memory),
           sizeof(Program));
    Memory* memory = calloc(1, sizeof(Memory));
    EXIT_IF(!memory);
    i32 x = 42;
    {
        emit_op_code(memory, OP_MOV_EAX);
        emit_i32(memory, x);
        emit_op_code(memory, OP_RET);
    }
    {
        Program program = transform(memory);
        {
            i32 return_value = (*((JitFn*)&program.buffer))();
            EXIT_IF(x != return_value);
            printf("%d\n", return_value);
        }
        EXIT_IF(munmap(program.buffer, memory->buffer_index));
    }
    free(memory);
    return EXIT_SUCCESS;
}
