#ifndef __PARSE_H__
#define __PARSE_H__

#include "prelude.h"

typedef enum {
    INST_UNKNOWN,

    INST_MOV_REG_IMM32,
    INST_MOV_REG_REG,

    INST_ADD_REG_IMM32,

    INST_PUSH_REG,
    INST_POP_REG,

    INST_CALL_REL_IMM32,
    INST_CALL_REL_IMM32_UNRESOLVED,

    INST_RET,
} InstTag;

typedef enum {
    REG_RAX,
    REG_RBX,

    REG_EAX,
    REG_EBX,
    REG_EDI,
} Register;

typedef union {
    const char* label;
    i32         imm_i32;
    Register    reg;
} Arg;

typedef struct {
    Arg     dst;
    Arg     src;
    u16     position;
    u16     size;
    InstTag tag;
} Inst;

typedef struct {
    const char* string;
    u16         position;
} Label;

#endif
