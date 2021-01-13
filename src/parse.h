#ifndef __PARSE_H__
#define __PARSE_H__

#include "prelude.h"
#include "token.h"

typedef enum {
    INST_UNKNOWN,

    INST_MOV_REG_IMM_I32,
    INST_MOV_REG_REG,
    INST_MOV_REG_ADDR_OFFSET,
    INST_MOV_ADDR_OFFSET_REG,
    INST_MOV_ADDR_OFFSET_IMM_I32,

    INST_ADD_REG_REG,
    INST_ADD_REG_IMM_I32,

    INST_SUB_REG_REG,
    INST_SUB_REG_IMM_I32,

    INST_PUSH_REG,
    INST_PUSH_IMM_I32,
    INST_POP_REG,

    INST_CALL_REL_IMM_I32,
    INST_CALL_REL_IMM_I32_UNRESOLVED,

    INST_RET,
} InstTag;

typedef enum {
    REG_RAX,
    REG_RBX,

    REG_EAX,
    REG_EBX,
    REG_EDI,

    REG_RBP,
    REG_RSP,
} Register;

typedef enum {
    ARG_LABEL,
    ARG_IMM_I32,
    ARG_IMM_F32,
    ARG_ADDR_OFFSET,
    ARG_REG,
} ArgTag;

typedef struct {
    union {
        const char* label;
        i32         imm_i32;
        f32         imm_f32;
        i32         addr_offset;
    };
    u16      line;
    Register reg;
    ArgTag   tag;
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

static void print_reg(File* stream, Register reg) {
    switch (reg) {
    case REG_RAX: {
        fprintf(stream, "rax");
        break;
    }
    case REG_RBX: {
        fprintf(stream, "rbx");
        break;
    }
    case REG_EAX: {
        fprintf(stream, "eax");
        break;
    }
    case REG_EBX: {
        fprintf(stream, "ebx");
        break;
    }
    case REG_EDI: {
        fprintf(stream, "edi");
        break;
    }
    case REG_RBP: {
        fprintf(stream, "rbp");
        break;
    }
    case REG_RSP: {
        fprintf(stream, "rsp");
        break;
    }
    default: {
        ERROR();
    }
    }
}

static void print_arg(File* stream, Arg arg) {
    switch (arg.tag) {
    case ARG_LABEL: {
        fprintf(stream, FMT_LINE "\"%s\"", arg.line, arg.label);
        break;
    }
    case ARG_IMM_I32: {
        fprintf(stream, FMT_LINE "%d", arg.line, arg.imm_i32);
        break;
    }
    case ARG_IMM_F32: {
        fprintf(stream, FMT_LINE "%f", arg.line, (f64)arg.imm_f32);
        break;
    }
    case ARG_ADDR_OFFSET: {
        fprintf(stream, FMT_LINE "[", arg.line);
        print_reg(stream, arg.reg);
        fprintf(stream, " + %d]", arg.addr_offset);
        break;
    }
    case ARG_REG: {
        fprintf(stream, FMT_LINE, arg.line);
        print_reg(stream, arg.reg);
        break;
    }
    default: {
        ERROR();
    }
    }
    fprintf(stream, "\n");
}

#define UNEXPECTED_ARG(arg)                                          \
    {                                                                \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        print_arg(stderr, arg);                                      \
        exit(EXIT_FAILURE);                                          \
    }

#endif
