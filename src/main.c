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
    if (SIZE_BUFFER <= memory->buffer_index) {
        ERROR("emit_op_code");
    }
    memory->buffer[memory->buffer_index++] = op_code;
}

static void emit_i32(Memory* memory, i32 value) {
    usize n = sizeof(i32);
    usize index = memory->buffer_index + n;
    if (SIZE_BUFFER < index) {
        ERROR("emit_i32");
    }
    memcpy(&memory->buffer[memory->buffer_index], &value, n);
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
    if (program.buffer == MAP_FAILED) {
        ERROR("mmap");
    }
    memcpy(program.buffer, &memory->buffer, memory->buffer_index);
    if (mprotect(program.buffer, memory->buffer_index, PROT_EXEC)) {
        ERROR("mprotect");
    }
    return program;
}

i32 main(void) {
    printf("sizeof(Memory)  : %zu\n"
           "sizeof(Program) : %zu\n"
           "\n",
           sizeof(Memory),
           sizeof(Program));
    Memory* memory = calloc(1, sizeof(Memory));
    if (!memory) {
        ERROR("calloc");
    }
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
            if (return_value != x) {
                ERROR("return_value");
            }
            printf("%d\n", return_value);
        }
        if (munmap(program.buffer, memory->buffer_index)) {
            ERROR("munmap");
        }
    }
    free(memory);
    return EXIT_SUCCESS;
}
