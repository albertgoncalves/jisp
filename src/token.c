#ifndef __TOKEN_C__
#define __TOKEN_C__

#include "memory.h"

static Token* alloc_token(Memory* memory) {
    EXIT_IF(SIZE_TOKENS <= memory->tokens_index);
    return &memory->tokens[memory->tokens_index++];
}

static char* alloc_buffer(Memory* memory, usize size) {
    EXIT_IF(SIZE_BUFFER < (memory->buffer_index + size));
    char* buffer = &memory->buffer[memory->buffer_index];
    memory->buffer_index += size;
    return buffer;
}

#define IS_ALPHA(x) \
    ((('A' <= (x)) && ((x) <= 'Z')) || (('a' <= (x)) && ((x) <= 'z')))

#define IS_DIGIT(x) (('0' <= (x)) && ((x) <= '9'))

#define IS_ALPHA_OR_DIGIT(x) (IS_ALPHA(x) || IS_DIGIT(x))

static i32 parse_digits_i32(Memory* memory, usize* i) {
    i32 x = 0;
    while (IS_DIGIT(memory->file[*i])) {
        x = (x * 10) + ((i32)(memory->file[(*i)++] - '0'));
    }
    return x;
}

static f32 parse_decimal_f32(Memory* memory, usize* i) {
    f32 a = 0.0f;
    f32 b = 1.0f;
    while (IS_DIGIT(memory->file[*i])) {
        a = (a * 10.0f) + ((f32)(memory->file[(*i)++] - '0'));
        b *= 10.0f;
    }
    return a / b;
}

static void set_tokens(Memory* memory) {
    u16 line = 1;
    for (usize i = 0; i < memory->file_index;) {
        switch (memory->file[i]) {
        case ' ':
        case '\t': {
            ++i;
            break;
        };
        case '\n': {
            ++line;
            ++i;
            break;
        }
        case ';': {
            for (; i < memory->file_index; ++i) {
                if (memory->file[i] == '\n') {
                    break;
                }
            }
            break;
        }
        case ':': {
            Token* token = alloc_token(memory);
            token->tag = TOKEN_COLON;
            token->line = line;
            ++i;
            break;
        }
        case ',': {
            Token* token = alloc_token(memory);
            token->tag = TOKEN_COMMA;
            token->line = line;
            ++i;
            break;
        }
        case '[': {
            Token* token = alloc_token(memory);
            token->tag = TOKEN_LBRACKET;
            token->line = line;
            ++i;
            break;
        }
        case ']': {
            Token* token = alloc_token(memory);
            token->tag = TOKEN_RBRACKET;
            token->line = line;
            ++i;
            break;
        }
        case '+': {
            Token* token = alloc_token(memory);
            token->tag = TOKEN_PLUS;
            token->line = line;
            ++i;
            break;
        }
        case '-': {
            EXIT_IF(memory->file_index <= ++i);
            Token* token = alloc_token(memory);
            token->line = line;
            if (IS_DIGIT(memory->file[i])) {
                i32 x = parse_digits_i32(memory, &i);
                if (memory->file[i] == '.') {
                    ++i;
                    token->f32 = -((f32)x + parse_decimal_f32(memory, &i));
                    token->tag = TOKEN_F32;
                } else {
                    token->i32 = -x;
                    token->tag = TOKEN_I32;
                }
            } else {
                token->tag = TOKEN_MINUS;
            }
            break;
        }
        default: {
            Token* token = alloc_token(memory);
            token->line = line;
            if (IS_DIGIT(memory->file[i])) {
                i32 x = parse_digits_i32(memory, &i);
                if (memory->file[i] == '.') {
                    ++i;
                    token->f32 = (f32)x + parse_decimal_f32(memory, &i);
                    token->tag = TOKEN_F32;
                } else {
                    token->i32 = x;
                    token->tag = TOKEN_I32;
                }
                continue;
            }
            usize j = i;
            for (; j < memory->file_index; ++j) {
                if (!(IS_ALPHA_OR_DIGIT(memory->file[j]))) {
                    break;
                }
            }
            EXIT_IF(i == j);
            usize size = j - i;
            if (!memcmp(&memory->file[i], "rax", size)) {
                token->tag = TOKEN_RAX;
            } else if (!memcmp(&memory->file[i], "rbx", size)) {
                token->tag = TOKEN_RBX;

            } else if (!memcmp(&memory->file[i], "eax", size)) {
                token->tag = TOKEN_EAX;
            } else if (!memcmp(&memory->file[i], "ebx", size)) {
                token->tag = TOKEN_EBX;
            } else if (!memcmp(&memory->file[i], "edi", size)) {
                token->tag = TOKEN_EDI;

            } else if (!memcmp(&memory->file[i], "rbp", size)) {
                token->tag = TOKEN_RBP;
            } else if (!memcmp(&memory->file[i], "rsp", size)) {
                token->tag = TOKEN_RSP;

            } else if (!memcmp(&memory->file[i], "xmm0", size)) {
                token->tag = TOKEN_XMM0;
            } else if (!memcmp(&memory->file[i], "xmm1", size)) {
                token->tag = TOKEN_XMM1;

            } else if (!memcmp(&memory->file[i], "mov", size)) {
                token->tag = TOKEN_MOV;
            } else if (!memcmp(&memory->file[i], "movss", size)) {
                token->tag = TOKEN_MOVSS;
            } else if (!memcmp(&memory->file[i], "add", size)) {
                token->tag = TOKEN_ADD;
            } else if (!memcmp(&memory->file[i], "addss", size)) {
                token->tag = TOKEN_ADDSS;
            } else if (!memcmp(&memory->file[i], "sub", size)) {
                token->tag = TOKEN_SUB;
            } else if (!memcmp(&memory->file[i], "xorps", size)) {
                token->tag = TOKEN_XORPS;
            } else if (!memcmp(&memory->file[i], "push", size)) {
                token->tag = TOKEN_PUSH;
            } else if (!memcmp(&memory->file[i], "pop", size)) {
                token->tag = TOKEN_POP;
            } else if (!memcmp(&memory->file[i], "call", size)) {
                token->tag = TOKEN_CALL;
            } else if (!memcmp(&memory->file[i], "ret", size)) {
                token->tag = TOKEN_RET;

            } else {
                char* buffer = alloc_buffer(memory, size + 1);
                memcpy(buffer, &memory->file[i], size);
                buffer[size] = '\0';
                token->string = buffer;
                token->tag = TOKEN_STR;
            }
            i = j;
        }
        }
    }
}

#endif
