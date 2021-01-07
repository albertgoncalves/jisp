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

#define EXPECTED_TOKEN(token_tag, memory, i) \
    {                                        \
        Token _ = pop_token(memory, &i);     \
        if (_.tag != token_tag) {            \
            UNEXPECTED_TOKEN(_);             \
        }                                    \
    }

static Register get_register(TokenTag tag) {
    switch (tag) {
    case TOKEN_NUM:
    case TOKEN_STR:
    case TOKEN_COMMA:
    case TOKEN_COLON:
    case TOKEN_MOV:
    case TOKEN_ADD:
    case TOKEN_PUSH:
    case TOKEN_POP:
    case TOKEN_CALL:
    case TOKEN_RET: {
        break;
    }
    case TOKEN_RAX: {
        return REG_RAX;
    }
    case TOKEN_RBX: {
        return REG_RBX;
    }
    case TOKEN_EAX: {
        return REG_EAX;
    }
    case TOKEN_EBX: {
        return REG_EBX;
    }
    case TOKEN_EDI: {
        return REG_EDI;
    }
    }
    ERROR();
}

static void set_size_position(Inst* inst, u16* position, u16 size) {
    inst->size = size;
    inst->position = *position;
    *position = (u16)(*position + size);
}

static void set_insts(Memory* memory) {
    u16 position = 0;
    for (usize i = 0; i < memory->tokens_index;) {
        Token token = memory->tokens[i++];
        switch (token.tag) {
        case TOKEN_RAX:
        case TOKEN_RBX:

        case TOKEN_EAX:
        case TOKEN_EBX:
        case TOKEN_EDI:

        case TOKEN_NUM:
        case TOKEN_COMMA:
        case TOKEN_COLON: {
            UNEXPECTED_TOKEN(token);
        }
        case TOKEN_STR: {
            EXPECTED_TOKEN(TOKEN_COLON, memory, i);
            Label* label = alloc_label(memory);
            label->string = token.string;
            label->position = position;
            break;
        }
        case TOKEN_MOV: {
            Token dst = pop_token(memory, &i);
            EXPECTED_TOKEN(TOKEN_COMMA, memory, i);
            Token src = pop_token(memory, &i);
            Inst* inst = alloc_inst(memory);
            if (is_register(dst)) {
                inst->dst = (Arg){
                    .reg = get_register(dst.tag),
                };
                if (is_register(src)) {
                    inst->src = (Arg){
                        .reg = get_register(src.tag),
                    };
                    set_size_position(inst, &position, 2);
                    inst->tag = INST_MOV_REG_REG;
                    continue;
                } else if (src.tag == TOKEN_NUM) {
                    inst->src = (Arg){
                        .imm_i32 = src.number,
                    };
                    set_size_position(inst, &position, 5);
                    inst->tag = INST_MOV_REG_IMM32;
                    continue;
                }
            }
            UNEXPECTED_TOKEN(token);
            break;
        }
        case TOKEN_ADD: {
            Token dst = pop_token(memory, &i);
            EXPECTED_TOKEN(TOKEN_COMMA, memory, i);
            Token src = pop_token(memory, &i);
            Inst* inst = alloc_inst(memory);
            if (is_register(dst)) {
                inst->dst = (Arg){
                    .reg = get_register(dst.tag),
                };
                if (src.tag == TOKEN_NUM) {
                    inst->src = (Arg){
                        .imm_i32 = src.number,
                    };
                    set_size_position(inst, &position, 5);
                    inst->tag = INST_ADD_REG_IMM32;
                    continue;
                }
            }
            UNEXPECTED_TOKEN(token);
            break;
        }
        case TOKEN_PUSH: {
            Token src = pop_token(memory, &i);
            if (is_register(src)) {
                Inst* inst = alloc_inst(memory);
                inst->src = (Arg){
                    .reg = get_register(src.tag),
                };
                set_size_position(inst, &position, 1);
                inst->tag = INST_PUSH_REG;
                continue;
            }
            UNEXPECTED_TOKEN(token);
            break;
        }
        case TOKEN_POP: {
            Token dst = pop_token(memory, &i);
            if (is_register(dst)) {
                Inst* inst = alloc_inst(memory);
                inst->dst = (Arg){
                    .reg = get_register(dst.tag),
                };
                set_size_position(inst, &position, 1);
                inst->tag = INST_POP_REG;
                continue;
            }
            UNEXPECTED_TOKEN(token);
            break;
        }
        case TOKEN_CALL: {
            Token dst = pop_token(memory, &i);
            Inst* inst = alloc_inst(memory);
            set_size_position(inst, &position, 5);
            if (dst.tag == TOKEN_NUM) {
                inst->dst = (Arg){
                    .imm_i32 = dst.number,
                };
                inst->tag = INST_CALL_REL_IMM32;
                continue;
            } else if (dst.tag == TOKEN_STR) {
                inst->dst = (Arg){
                    .label = dst.string,
                };
                inst->tag = INST_CALL_REL_IMM32_UNRESOLVED;
                continue;
            }
            UNEXPECTED_TOKEN(token);
            break;
        }
        case TOKEN_RET: {
            Inst* inst = alloc_inst(memory);
            set_size_position(inst, &position, 1);
            inst->tag = INST_RET;
            break;
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
