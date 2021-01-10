#ifndef __PARSE_H__
#define __PARSE_H__

#include "prelude.h"
#include "token.h"

typedef enum {
    INST_UNKNOWN,

    INST_MOV_REG_IMM_I32,
    INST_MOV_REG_REG,
    INST_MOV_REG_ADDR_RSP_OFFSET,
    INST_MOV_ADDR_RSP_OFFSET_REG,

    INST_ADD_REG_IMM_I32,

    INST_PUSH_REG,
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

    REG_RSP,
} Register;

typedef enum {
    ARG_LABEL,
    ARG_IMM_I32,
    ARG_ADDR_RSP_OFFSET,
    ARG_REG,
} ArgTag;

typedef struct {
    union {
        const char* label;
        i32         imm_i32;
        i32         addr_rsp_offset;
        Register    reg;
    };
    u16    line;
    ArgTag tag;
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
    case ARG_ADDR_RSP_OFFSET: {
        fprintf(stream, FMT_LINE "[rsp + %d]", arg.line, arg.addr_rsp_offset);
        break;
    }
    case ARG_REG: {
        switch (arg.reg) {
        case REG_RAX: {
            fprintf(stream, FMT_LINE "rax", arg.line);
            break;
        }
        case REG_RBX: {
            fprintf(stream, FMT_LINE "rbx", arg.line);
            break;
        }
        case REG_EAX: {
            fprintf(stream, FMT_LINE "eax", arg.line);
            break;
        }
        case REG_EBX: {
            fprintf(stream, FMT_LINE "ebx", arg.line);
            break;
        }
        case REG_EDI: {
            fprintf(stream, FMT_LINE "edi", arg.line);
            break;
        }
        case REG_RSP: {
            fprintf(stream, FMT_LINE "rsp", arg.line);
            break;
        }
        default: {
            ERROR();
        }
        }
        break;
    }
    default: {
        ERROR();
    }
    }
    fprintf(stream, "\n");
}

#endif
