#ifndef __EMIT_C__
#define __EMIT_C__

#include <string.h>
#include <sys/mman.h>

#include "emit.h"
#include "memory.h"

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

#define EMIT_3_BYTES(fn, a, b, c)                        \
    static void fn(Memory* memory) {                     \
        EXIT_IF(SIZE_BYTES < (memory->bytes_index + 3)); \
        memory->bytes[memory->bytes_index++] = a;        \
        memory->bytes[memory->bytes_index++] = b;        \
        memory->bytes[memory->bytes_index++] = c;        \
    }

#define EMIT_1_VAR(fn, type)                                               \
    static void fn(Memory* memory, type value) {                           \
        EXIT_IF(SIZE_BYTES < (memory->bytes_index + sizeof(type)));        \
        memcpy(&memory->bytes[memory->bytes_index], &value, sizeof(type)); \
        memory->bytes_index += sizeof(type);                               \
    }

EMIT_1_BYTE(emit_mov_eax_imm_i32, 0xB8)
EMIT_1_BYTE(emit_mov_edi_imm_i32, 0xBF)

EMIT_2_BYTES(emit_mov_eax_ebx, 0x89, 0xD8)
EMIT_2_BYTES(emit_mov_ebx_edi, 0x89, 0xFB)
EMIT_3_BYTES(emit_mov_rbp_rsp, 0x48, 0x89, 0xE5)
EMIT_3_BYTES(emit_mov_rsp_rbp, 0x48, 0x89, 0xEC)

EMIT_2_BYTES(emit_mov_addr_rbp_offset_imm_i32, 0xC7, 0x85)
EMIT_3_BYTES(emit_mov_addr_rsp_offset_imm_i32, 0xC7, 0x84, 0x24)

EMIT_2_BYTES(emit_mov_eax_addr_rbp_offset, 0x8B, 0x85)
EMIT_2_BYTES(emit_mov_ebx_addr_rbp_offset, 0x8B, 0x9D)
EMIT_3_BYTES(emit_mov_eax_addr_rsp_offset, 0x8B, 0x84, 0x24)
EMIT_3_BYTES(emit_mov_ebx_addr_rsp_offset, 0x8B, 0x9C, 0x24)

EMIT_2_BYTES(emit_mov_addr_rbp_offset_eax, 0x89, 0x85)
EMIT_3_BYTES(emit_mov_addr_rsp_offset_eax, 0x89, 0x84, 0x24)
EMIT_3_BYTES(emit_mov_addr_rsp_offset_edi, 0x89, 0xBC, 0x24)

EMIT_2_BYTES(emit_add_eax_ebx, 0x01, 0xD8)
EMIT_1_BYTE(emit_add_eax_imm_i32, 0x05)

EMIT_2_BYTES(emit_sub_eax_ebx, 0x29, 0xD8)
EMIT_1_BYTE(emit_sub_eax_imm_i32, 0x2D)
EMIT_3_BYTES(emit_sub_rsp_imm_i32, 0x48, 0x81, 0xEC)

EMIT_1_BYTE(emit_push_rbx, 0x53)
EMIT_1_BYTE(emit_push_rbp, 0x55)
EMIT_1_BYTE(emit_push_imm_i32, 0x68)
EMIT_1_BYTE(emit_pop_rbx, 0x5B)
EMIT_1_BYTE(emit_pop_rbp, 0x5D)

EMIT_1_BYTE(emit_call_rel_imm_i32, 0xE8)
EMIT_1_BYTE(emit_ret, 0xC3)

EMIT_1_VAR(emit_i32, i32) // NOTE: 4 bytes!

#define CHECK_SIZE(memory, position, size)                 \
    {                                                      \
        EXIT_IF((memory->bytes_index - position) != size); \
        position += size;                                  \
    }

static void set_bytes(Memory* memory) {
    usize position = 0;
    for (usize i = 0; i < memory->insts_index; ++i) {
        Inst inst = memory->insts[i];
        switch (inst.tag) {
        case INST_MOV_REG_IMM_I32: {
            if (inst.dst.reg == REG_EAX) {
                emit_mov_eax_imm_i32(memory);
            } else if (inst.dst.reg == REG_EDI) {
                emit_mov_edi_imm_i32(memory);
            } else {
                ERROR();
            }
            emit_i32(memory, inst.src.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_MOV_REG_REG: {
            Register dst = inst.dst.reg;
            Register src = inst.src.reg;
            if ((dst == REG_EAX) && (src == REG_EBX)) {
                emit_mov_eax_ebx(memory);
            } else if ((dst == REG_EBX) && (src == REG_EDI)) {
                emit_mov_ebx_edi(memory);
            } else if ((dst == REG_RBP) && (src == REG_RSP)) {
                emit_mov_rbp_rsp(memory);
            } else if ((dst == REG_RSP) && (src == REG_RBP)) {
                emit_mov_rsp_rbp(memory);
            } else {
                ERROR();
            }
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_MOV_REG_ADDR_OFFSET: {
            if (inst.src.reg == REG_RBP) {
                if (inst.dst.reg == REG_EAX) {
                    emit_mov_eax_addr_rbp_offset(memory);
                } else if (inst.dst.reg == REG_EBX) {
                    emit_mov_ebx_addr_rbp_offset(memory);
                } else {
                    ERROR();
                }
            } else if (inst.src.reg == REG_RSP) {
                if (inst.dst.reg == REG_EAX) {
                    emit_mov_eax_addr_rsp_offset(memory);
                } else if (inst.dst.reg == REG_EBX) {
                    emit_mov_ebx_addr_rsp_offset(memory);
                } else {
                    ERROR();
                }
            } else {
                ERROR();
            }
            emit_i32(memory, inst.src.addr_offset);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_MOV_ADDR_OFFSET_REG: {
            if (inst.dst.reg == REG_RBP) {
                if (inst.src.reg == REG_EAX) {
                    emit_mov_addr_rbp_offset_eax(memory);
                } else {
                    ERROR();
                }
            } else if (inst.dst.reg == REG_RSP) {
                if (inst.src.reg == REG_EAX) {
                    emit_mov_addr_rsp_offset_eax(memory);
                } else if (inst.src.reg == REG_EDI) {
                    emit_mov_addr_rsp_offset_edi(memory);
                } else {
                    ERROR();
                }
            } else {
                ERROR();
            }
            emit_i32(memory, inst.dst.addr_offset);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_MOV_ADDR_OFFSET_IMM_I32: {
            if (inst.dst.reg == REG_RBP) {
                emit_mov_addr_rbp_offset_imm_i32(memory);
            } else if (inst.dst.reg == REG_RSP) {
                emit_mov_addr_rsp_offset_imm_i32(memory);
            } else {
                ERROR();
            }
            emit_i32(memory, inst.dst.addr_offset);
            emit_i32(memory, inst.src.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_ADD_REG_REG: {
            if ((inst.dst.reg == REG_EAX) && (inst.src.reg == REG_EBX)) {
                emit_add_eax_ebx(memory);
            } else {
                ERROR();
            }
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_ADD_REG_IMM_I32: {
            if (inst.dst.reg == REG_EAX) {
                emit_add_eax_imm_i32(memory);
            } else {
                ERROR();
            }
            emit_i32(memory, inst.src.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_SUB_REG_REG: {
            if ((inst.dst.reg == REG_EAX) && (inst.src.reg == REG_EBX)) {
                emit_sub_eax_ebx(memory);
            } else {
                ERROR();
            }
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_SUB_REG_IMM_I32: {
            if (inst.dst.reg == REG_EAX) {
                emit_sub_eax_imm_i32(memory);
            } else if (inst.dst.reg == REG_RSP) {
                emit_sub_rsp_imm_i32(memory);
            } else {
                ERROR();
            }
            emit_i32(memory, inst.src.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_PUSH_REG: {
            if (inst.src.reg == REG_RBX) {
                emit_push_rbx(memory);
            } else if (inst.src.reg == REG_RBP) {
                emit_push_rbp(memory);
            } else {
                ERROR();
            }
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_PUSH_IMM_I32: {
            emit_push_imm_i32(memory);
            emit_i32(memory, inst.src.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_POP_REG: {
            if (inst.dst.reg == REG_RBX) {
                emit_pop_rbx(memory);
            } else if (inst.dst.reg == REG_RBP) {
                emit_pop_rbp(memory);
            } else {
                ERROR();
            }
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_CALL_REL_IMM_I32: {
            emit_call_rel_imm_i32(memory);
            emit_i32(memory, inst.dst.imm_i32);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_RET: {
            emit_ret(memory);
            CHECK_SIZE(memory, position, inst.size);
            break;
        }
        case INST_UNKNOWN:
        case INST_CALL_REL_IMM_I32_UNRESOLVED:
        default: {
            ERROR();
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
