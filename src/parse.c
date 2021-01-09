#ifndef __PARSE_C__
#define __PARSE_C__

#include "memory.h"
#include "token.h"

static Inst* alloc_inst(Memory* memory) {
    EXIT_IF(SIZE_INSTS <= memory->insts_index);
    return &memory->insts[memory->insts_index++];
}

static Token pop_token(Memory* memory, usize* i) {
    EXIT_IF(SIZE_TOKENS <= *i);
    return memory->tokens[(*i)++];
}

static Label* alloc_label(Memory* memory) {
    EXIT_IF(SIZE_LABELS <= memory->labels_index);
    return &memory->labels[memory->labels_index++];
}

#define UNEXPECTED_TOKEN(token)                                      \
    {                                                                \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        print_token(stderr, token);                                  \
        exit(EXIT_FAILURE);                                          \
    }

#define UNEXPECTED_ARG(arg)                                          \
    {                                                                \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        print_arg(stderr, arg);                                      \
        exit(EXIT_FAILURE);                                          \
    }

#define EXPECTED_TOKEN(token_tag, memory, i) \
    {                                        \
        Token _ = pop_token(memory, i);      \
        if (_.tag != token_tag) {            \
            UNEXPECTED_TOKEN(_);             \
        }                                    \
    }

static void set_size_position(Inst* inst, u16* position, u16 size) {
    inst->size = size;
    inst->position = *position;
    *position = (u16)(*position + size);
}

static Arg get_arg(Memory* memory, usize* i) {
    Token token = pop_token(memory, i);
    switch (token.tag) {
    case TOKEN_COMMA:
    case TOKEN_COLON:
    case TOKEN_RBRACKET:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MOV:
    case TOKEN_ADD:
    case TOKEN_PUSH:
    case TOKEN_POP:
    case TOKEN_CALL:
    case TOKEN_RET: {
        break;
    }
    case TOKEN_STR: {
        return (Arg){
            .label = token.string,
            .tag = ARG_LABEL,
            .line = token.line,
        };
    }
    case TOKEN_RAX: {
        return (Arg){
            .reg = REG_RAX,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_RBX: {
        return (Arg){
            .reg = REG_RBX,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_EAX: {
        return (Arg){
            .reg = REG_EAX,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_EBX: {
        return (Arg){
            .reg = REG_EBX,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_EDI: {
        return (Arg){
            .reg = REG_EDI,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_RSP: {
        return (Arg){
            .reg = REG_RSP,
            .tag = ARG_REG,
            .line = token.line,
        };
    }
    case TOKEN_I32: {
        return (Arg){
            .imm_i32 = token.i32,
            .tag = ARG_IMM_I32,
            .line = token.line,
        };
    }
    case TOKEN_LBRACKET: {
        EXPECTED_TOKEN(TOKEN_RSP, memory, i);
        token = pop_token(memory, i);
        i32 offset;
        if (token.tag == TOKEN_RBRACKET) {
            offset = 0;
        } else if (token.tag == TOKEN_PLUS) {
            token = pop_token(memory, i);
            EXIT_IF(token.tag != TOKEN_I32);
            offset = token.i32;
        } else if (token.tag == TOKEN_MINUS) {
            token = pop_token(memory, i);
            EXIT_IF(token.tag != TOKEN_I32);
            offset = -token.i32;
        } else {
            UNEXPECTED_TOKEN(token);
        }
        EXPECTED_TOKEN(TOKEN_RBRACKET, memory, i);
        return (Arg){
            .addr_rsp_offset = offset,
            .tag = ARG_ADDR_RSP_OFFSET,
            .line = token.line,
        };
    }
    default: {
        ERROR();
    }
    }
    UNEXPECTED_TOKEN(token);
}

static void set_insts(Memory* memory) {
    u16 position = 0;
    for (usize i = 0; i < memory->tokens_index;) {
        Token token = pop_token(memory, &i);
        switch (token.tag) {
        case TOKEN_RAX:
        case TOKEN_RBX:
        case TOKEN_EAX:
        case TOKEN_EBX:
        case TOKEN_EDI:
        case TOKEN_RSP:
        case TOKEN_I32:
        case TOKEN_COMMA:
        case TOKEN_COLON:
        case TOKEN_LBRACKET:
        case TOKEN_RBRACKET:
        case TOKEN_PLUS:
        case TOKEN_MINUS: {
            UNEXPECTED_TOKEN(token);
        }
        case TOKEN_STR: {
            EXPECTED_TOKEN(TOKEN_COLON, memory, &i);
            usize n = strlen(token.string);
            for (usize j = 0; j < memory->labels_index; ++j) {
                if (!memcmp(memory->labels[j].string, token.string, n)) {
                    UNEXPECTED_TOKEN(token);
                }
            }
            Label* label = alloc_label(memory);
            label->string = token.string;
            label->position = position;
            break;
        }
        case TOKEN_MOV: {
            Arg dst = get_arg(memory, &i);
            EXPECTED_TOKEN(TOKEN_COMMA, memory, &i);
            Arg src = get_arg(memory, &i);

            Inst* inst = alloc_inst(memory);
            inst->dst = dst;
            inst->src = src;
            if (dst.tag == ARG_REG) {
                if (src.tag == ARG_REG) {
                    inst->tag = INST_MOV_REG_REG;
                    set_size_position(inst, &position, 2);
                    continue;
                } else if (src.tag == ARG_IMM_I32) {
                    inst->tag = INST_MOV_REG_IMM32;
                    set_size_position(inst, &position, 5);
                    continue;
                } else if (src.tag == ARG_ADDR_RSP_OFFSET) {
                    inst->tag = INST_MOV_REG_ADDR_RSP_OFFSET;
                    set_size_position(inst, &position, 7);
                    continue;
                }
                UNEXPECTED_ARG(src);
            } else if (dst.tag == ARG_ADDR_RSP_OFFSET) {
                if (src.tag == ARG_REG) {
                    inst->tag = INST_MOV_ADDR_RSP_OFFSET_REG;
                    set_size_position(inst, &position, 7);
                    continue;
                }
                UNEXPECTED_ARG(src);
            }
            UNEXPECTED_ARG(dst);
        }
        case TOKEN_ADD: {
            Arg dst = get_arg(memory, &i);
            EXPECTED_TOKEN(TOKEN_COMMA, memory, &i);
            Arg src = get_arg(memory, &i);
            if (dst.tag == ARG_REG) {
                Inst* inst = alloc_inst(memory);
                inst->dst = dst;
                inst->src = src;
                if (src.tag == ARG_IMM_I32) {
                    inst->tag = INST_ADD_REG_IMM32;
                    set_size_position(inst, &position, 5);
                    continue;
                }
                UNEXPECTED_ARG(src);
            }
            UNEXPECTED_ARG(dst);
        }
        case TOKEN_PUSH: {
            Arg src = get_arg(memory, &i);
            if (src.tag == ARG_REG) {
                Inst* inst = alloc_inst(memory);
                inst->src = src;
                inst->tag = INST_PUSH_REG;
                set_size_position(inst, &position, 1);
                continue;
            }
            UNEXPECTED_ARG(src);
        }
        case TOKEN_POP: {
            Arg dst = get_arg(memory, &i);
            if (dst.tag == ARG_REG) {
                Inst* inst = alloc_inst(memory);
                inst->dst = dst;
                inst->tag = INST_POP_REG;
                set_size_position(inst, &position, 1);
                continue;
            }
            UNEXPECTED_ARG(dst);
        }
        case TOKEN_CALL: {
            Arg   dst = get_arg(memory, &i);
            Inst* inst = alloc_inst(memory);
            inst->dst = dst;
            set_size_position(inst, &position, 5);
            if (dst.tag == ARG_IMM_I32) {
                inst->tag = INST_CALL_REL_IMM32;
                continue;
            } else if (dst.tag == ARG_LABEL) {
                inst->tag = INST_CALL_REL_IMM32_UNRESOLVED;
                continue;
            }
            UNEXPECTED_ARG(dst);
        }
        case TOKEN_RET: {
            Inst* inst = alloc_inst(memory);
            inst->tag = INST_RET;
            set_size_position(inst, &position, 1);
            break;
        }
        default: {
            ERROR();
        }
        }
    }
}

static u16 lookup_label_position(Memory* memory, const char* label) {
    usize n = strlen(label);
    for (usize i = 0; i < memory->labels_index; ++i) {
        if (!memcmp(memory->labels[i].string, label, n)) {
            return memory->labels[i].position;
        }
    }
    ERROR();
}

static void resolve_insts(Memory* memory) {
    for (usize i = 0; i < memory->insts_index; ++i) {
        Inst* inst = &memory->insts[i];
        if (inst->tag == INST_UNKNOWN) {
            ERROR();
        } else if (inst->tag == INST_CALL_REL_IMM32_UNRESOLVED) {
            inst->tag = INST_CALL_REL_IMM32;
            inst->dst.imm_i32 =
                ((i32)lookup_label_position(memory, inst->dst.label)) -
                ((i32)(inst->position + inst->size));
        }
    }
}

#endif
