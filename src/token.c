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

static void dealloc_buffer(Memory* memory, usize size) {
    EXIT_IF(memory->buffer_index < size);
    memory->buffer_index -= size;
}

#define IS_ALPHA(x) \
    ((('A' <= (x)) && ((x) <= 'Z')) || (('a' <= (x)) && ((x) <= 'z')))

#define IS_DIGIT(x) (('0' <= (x)) && ((x) <= '9'))

#define PARSE_DIGITS(fn, type)                               \
    static type fn(Memory* memory, usize* i) {               \
        type x = 0;                                          \
        while (IS_DIGIT(memory->file[*i])) {                 \
            x = (x * 10) + ((type)(memory->file[*i] - '0')); \
            ++(*i);                                          \
        }                                                    \
        return x;                                            \
    }

PARSE_DIGITS(parse_digits_i32, i32)

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
                token->i32 = -parse_digits_i32(memory, &i);
                token->tag = TOKEN_I32;
            } else {
                token->tag = TOKEN_MINUS;
            }
            break;
        }
        default: {
            Token* token = alloc_token(memory);
            token->line = line;
            if (IS_DIGIT(memory->file[i])) {
                token->i32 = parse_digits_i32(memory, &i);
                token->tag = TOKEN_I32;
                continue;
            }
            usize j = i;
            for (; j < memory->file_index; ++j) {
                if (!IS_ALPHA(memory->file[j])) {
                    break;
                }
            }
            EXIT_IF(i == j);
            usize size = j - i;
            char* buffer = alloc_buffer(memory, size);
            memcpy(buffer, &memory->file[i], size);
            i = j;
            if (!memcmp(buffer, "rax", size)) {
                token->tag = TOKEN_RAX;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "rbx", size)) {
                token->tag = TOKEN_RBX;
                dealloc_buffer(memory, size);

            } else if (!memcmp(buffer, "eax", size)) {
                token->tag = TOKEN_EAX;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "ebx", size)) {
                token->tag = TOKEN_EBX;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "edi", size)) {
                token->tag = TOKEN_EDI;
                dealloc_buffer(memory, size);

            } else if (!memcmp(buffer, "rbp", size)) {
                token->tag = TOKEN_RBP;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "rsp", size)) {
                token->tag = TOKEN_RSP;
                dealloc_buffer(memory, size);

            } else if (!memcmp(buffer, "mov", size)) {
                token->tag = TOKEN_MOV;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "add", size)) {
                token->tag = TOKEN_ADD;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "sub", size)) {
                token->tag = TOKEN_SUB;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "push", size)) {
                token->tag = TOKEN_PUSH;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "pop", size)) {
                token->tag = TOKEN_POP;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "call", size)) {
                token->tag = TOKEN_CALL;
                dealloc_buffer(memory, size);
            } else if (!memcmp(buffer, "ret", size)) {
                token->tag = TOKEN_RET;
                dealloc_buffer(memory, size);

            } else {
                alloc_buffer(memory, 1);
                buffer[size] = '\0';
                token->string = buffer;
                token->tag = TOKEN_STR;
            }
        }
        }
    }
}

#endif
