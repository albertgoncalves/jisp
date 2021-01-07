#ifndef __EMIT_H__
#define __EMIT_H__

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

#define EMIT_1_BYTE(fn, x)                          \
    static void fn(Memory* memory) {                \
        EXIT_IF(SIZE_BYTES <= memory->bytes_index); \
        memory->bytes[memory->bytes_index++] = x;   \
    }

#define EMIT_2_BYTES(fn, a, b)                           \
    static void fn(Memory* memory) {                     \
        EXIT_IF(SIZE_BYTES < (memory->bytes_index + 2)); \
        memory->bytes[memory->bytes_index++] = a;        \
        memory->bytes[memory->bytes_index++] = b;        \
    }

#define EMIT_1_VAR(fn, type)                                               \
    static void fn(Memory* memory, type value) {                           \
        EXIT_IF(SIZE_BYTES < (memory->bytes_index + sizeof(type)));        \
        memcpy(&memory->bytes[memory->bytes_index], &value, sizeof(type)); \
        memory->bytes_index += sizeof(type);                               \
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

static void check_size(Memory* memory, usize* position, usize size) {
    EXIT_IF((memory->bytes_index - *position) != size);
    *position += size;
}

static void set_bytes(Memory* memory) {
    usize position = 0;
    for (usize i = 0; i < memory->insts_index; ++i) {
        Inst inst = memory->insts[i];
        switch (inst.tag) {
        case INST_UNKNOWN:
        case INST_CALL_REL_IMM32_UNRESOLVED: {
            ERROR();
        }
        case INST_MOV_REG_IMM32: {
            if (inst.dst.reg == REG_EAX) {
                emit_mov_eax_imm32(memory);
            } else if (inst.dst.reg == REG_EDI) {
                emit_mov_edi_imm32(memory);
            } else {
                ERROR();
            }
            emit_i32(memory, inst.src.imm_i32);
            check_size(memory, &position, inst.size);
            break;
        }
        case INST_MOV_REG_REG: {
            Register dst = inst.dst.reg;
            Register src = inst.src.reg;
            if ((dst == REG_EAX) && (src == REG_EBX)) {
                emit_mov_eax_ebx(memory);
            } else if ((dst == REG_EBX) && (src == REG_EDI)) {
                emit_mov_ebx_edi(memory);
            } else {
                ERROR();
            }
            check_size(memory, &position, inst.size);
            break;
        }
        case INST_PUSH_REG: {
            if (inst.src.reg == REG_RBX) {
                emit_push_rbx(memory);
            } else {
                ERROR();
            }
            check_size(memory, &position, inst.size);
            break;
        }
        case INST_POP_REG: {
            if (inst.dst.reg == REG_RBX) {
                emit_pop_rbx(memory);
            } else {
                ERROR();
            }
            check_size(memory, &position, inst.size);
            break;
        }
        case INST_CALL_REL_IMM32: {
            emit_call_rel_imm32(memory);
            emit_i32(memory, inst.dst.imm_i32);
            check_size(memory, &position, inst.size);
            break;
        }
        case INST_RET: {
            emit_ret(memory);
            check_size(memory, &position, inst.size);
            break;
        }
        }
    }
}

static Program transform(Memory* memory) {
    Program program = {0};
    program.buffer = mmap(NULL,
                          memory->bytes_index,
                          PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_PRIVATE,
                          -1,
                          0);
    EXIT_IF(program.buffer == MAP_FAILED);
    memcpy(program.buffer, &memory->bytes, memory->bytes_index);
    EXIT_IF(mprotect(program.buffer, memory->bytes_index, PROT_EXEC));
    return program;
}

#endif
