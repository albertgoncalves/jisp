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

typedef i32 (*FnVoidI32)(void);

#define EMIT_1_BYTE(fn, x)                            \
    static void fn(Memory* memory) {                  \
        EXIT_IF(SIZE_BUFFER <= memory->buffer_index); \
        memory->buffer[memory->buffer_index++] = x;   \
    }

#define EMIT_2_BYTES(fn, a, b)                             \
    static void fn(Memory* memory) {                       \
        EXIT_IF(SIZE_BUFFER < (memory->buffer_index + 2)); \
        memory->buffer[memory->buffer_index++] = a;        \
        memory->buffer[memory->buffer_index++] = b;        \
    }

#define EMIT_1_VAR(fn, type)                                                 \
    static void fn(Memory* memory, type value) {                             \
        EXIT_IF(SIZE_BUFFER < (memory->buffer_index + sizeof(type)));        \
        memcpy(&memory->buffer[memory->buffer_index], &value, sizeof(type)); \
        memory->buffer_index += sizeof(type);                                \
    }

EMIT_1_BYTE(emit_mov_ebx_imm32, 0xBB)
EMIT_1_BYTE(emit_push_rbx, 0x53)
EMIT_1_BYTE(emit_pop_rbx, 0x5B)
EMIT_1_BYTE(emit_ret, 0xC3)
EMIT_2_BYTES(emit_mov_eax_ebx, 0x89, 0xD8)
EMIT_1_VAR(emit_i32, i32)

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
    printf("sizeof(i32)     : %zu\n"
           "sizeof(Memory)  : %zu\n"
           "sizeof(Program) : %zu\n"
           "\n",
           sizeof(i32),
           sizeof(Memory),
           sizeof(Program));
    Memory* memory = calloc(1, sizeof(Memory));
    EXIT_IF(!memory);
    i32 x = 42;
    {
        emit_push_rbx(memory);
        emit_mov_ebx_imm32(memory);
        emit_i32(memory, x);
        emit_mov_eax_ebx(memory);
        emit_pop_rbx(memory);
        emit_ret(memory);
    }
    {
        Program program = transform(memory);
        {
            i32 return_value = (*((FnVoidI32*)&program.buffer))();
            printf("%d\n", return_value);
            EXIT_IF(x != return_value);
        }
        EXIT_IF(munmap(program.buffer, memory->buffer_index));
    }
    free(memory);
    return EXIT_SUCCESS;
}
