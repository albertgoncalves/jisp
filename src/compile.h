#ifndef __COMPILE_H__
#define __COMPILE_H__

#include "memory.h"

/* NOTE: Useful resources:
 * `https://bernsteinbear.com/blog/lisp/`
 * `http://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf`
 * `http://6.s081.scripts.mit.edu/sp18/x86-64-architecture-guide.html`
 * `https://www.felixcloutier.com/x86/call`
 * `https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html`
 */

#include <string.h>
#include <sys/mman.h>

typedef struct {
    void* buffer;
} Program;

typedef i32 (*FnVoidI32)(void);

#define EMIT_1_BYTE(fn, x)                                                 \
    static void fn(Memory* memory) {                                       \
        EXIT_IF(sizeof(memory->byte_buffer) <= memory->byte_buffer_index); \
        memory->byte_buffer[memory->byte_buffer_index++] = x;              \
    }

#define EMIT_2_BYTES(fn, a, b)                                \
    static void fn(Memory* memory) {                          \
        EXIT_IF(sizeof(memory->byte_buffer) <                 \
                (memory->byte_buffer_index + 2));             \
        memory->byte_buffer[memory->byte_buffer_index++] = a; \
        memory->byte_buffer[memory->byte_buffer_index++] = b; \
    }

#define EMIT_1_VAR(fn, type)                                    \
    static void fn(Memory* memory, type value) {                \
        EXIT_IF(sizeof(memory->byte_buffer) <                   \
                (memory->byte_buffer_index + sizeof(type)));    \
        memcpy(&memory->byte_buffer[memory->byte_buffer_index], \
               &value,                                          \
               sizeof(type));                                   \
        memory->byte_buffer_index += sizeof(type);              \
    }

EMIT_1_BYTE(emit_mov_eax_imm32, 0xB8)
EMIT_1_BYTE(emit_mov_edi_imm32, 0xBF)
EMIT_1_BYTE(emit_push_rbx, 0x53)
EMIT_1_BYTE(emit_pop_rbx, 0x5B)
EMIT_1_BYTE(emit_call_rel_imm32, 0xE8)
EMIT_1_BYTE(emit_ret, 0xC3)
EMIT_2_BYTES(emit_mov_eax_ebx, 0x89, 0xD8)
EMIT_2_BYTES(emit_mov_ebx_edi, 0x89, 0xFB)
EMIT_1_VAR(emit_i32, i32) // NOTE: 4 bytes!

static Program transform(Memory* memory) {
    Program program = {0};
    program.buffer = mmap(NULL,
                          memory->byte_buffer_index,
                          PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_PRIVATE,
                          -1,
                          0);
    EXIT_IF(program.buffer == MAP_FAILED);
    memcpy(program.buffer, &memory->byte_buffer, memory->byte_buffer_index);
    EXIT_IF(mprotect(program.buffer, memory->byte_buffer_index, PROT_EXEC));
    return program;
}

static void test_compile(Memory* memory) {
    memory->byte_buffer_index = 0;
    i32 x = 42;
    {
        /*     - 0-     mov     edi, `x`
         *     - 5-     call    label
         *     -10-     ret
         *     -11-     mov     eax, `x + 1` ; NOTE: Never evaluated!
         */
        emit_mov_edi_imm32(memory);  //  0 + 1 ->  1
        emit_i32(memory, x);         //  1 + 4 ->  5
        emit_call_rel_imm32(memory); //  5 + 1 ->  6
        emit_i32(memory, 6);         //  6 + 4 -> 10
        emit_ret(memory);            // 10 + 1 -> 11
        emit_mov_eax_imm32(memory);  // 11 + 1 -> 12
        emit_i32(memory, x + 1);     // 12 + 4 -> 16
        EXIT_IF(memory->byte_buffer_index != 16);
    }
    {
        /*          label:
         *     -16-     push    rbx
         *     -17-     mov     ebx, edi
         *     -19-     mov     eax, ebx
         *     -21-     pop     rbx
         *     -22-     ret
         */
        emit_push_rbx(memory);    // 16 + 1 -> 17
        emit_mov_ebx_edi(memory); // 17 + 2 -> 19
        emit_mov_eax_ebx(memory); // 19 + 2 -> 21
        emit_pop_rbx(memory);     // 21 + 1 -> 22
        emit_ret(memory);         // 22 + 1 -> 23
        EXIT_IF(memory->byte_buffer_index != 23);
    }
    {
        Program program = transform(memory);
        i32     return_value = (*((FnVoidI32*)&program.buffer))();
        EXIT_IF(x != return_value);
        EXIT_IF(munmap(program.buffer, memory->byte_buffer_index));
    }
    PRINT_FN_OK();
}

#endif
