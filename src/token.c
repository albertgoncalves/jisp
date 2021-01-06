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

#define GET_DECIMAL(fn, type)                                   \
    static type fn(const char* decimal) {                       \
        type result = 0;                                        \
        while (*decimal) {                                      \
            char digit = *(decimal++);                          \
            if (('0' <= digit) && (digit <= '9')) {             \
                result = (result * 10) + ((type)(digit - '0')); \
            } else {                                            \
                ERROR();                                        \
            }                                                   \
        }                                                       \
        return result;                                          \
    }

GET_DECIMAL(get_decimal_u32, u32)

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
        default: {
            Token* token = alloc_token(memory);
            token->line = line;
            usize j = i + 1;
            for (; j < memory->file_index; ++j) {
                char x = memory->file[j];
                if (!((('a' <= x) && (x <= 'z')) ||
                      (('A' <= x) && (x <= 'Z')) ||
                      (('0' <= x) && (x <= '9'))))
                {
                    break;
                }
            }
            usize size = (j - i) + 1;
            char* buffer = alloc_buffer(memory, size);
            memcpy(buffer, &memory->file[i], size - 1);
            i = j;
            buffer[size - 1] = '\0';
            if (('0' <= buffer[0]) && (buffer[0] <= '9')) {
                token->tag = TOKEN_NUM;
                token->number = get_decimal_u32(buffer);
                dealloc_buffer(memory, size);

            } else if (!memcmp(buffer, "rax", size)) {
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

            } else if (!memcmp(buffer, "mov", size)) {
                token->tag = TOKEN_MOV;
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
                token->tag = TOKEN_STR;
                token->string = buffer;
            }
        }
        }
    }
}

#define FMT_LINE "[%3hu]  "

static void print_token(Token* token) {
    switch (token->tag) {
    case TOKEN_RAX: {
        printf(FMT_LINE "rax", token->line);
        break;
    }
    case TOKEN_RBX: {
        printf(FMT_LINE "rbx", token->line);
        break;
    }
    case TOKEN_EAX: {
        printf(FMT_LINE "eax", token->line);
        break;
    }
    case TOKEN_EBX: {
        printf(FMT_LINE "ebx", token->line);
        break;
    }
    case TOKEN_EDI: {
        printf(FMT_LINE "edi", token->line);
        break;
    }
    case TOKEN_MOV: {
        printf(FMT_LINE "mov", token->line);
        break;
    }
    case TOKEN_PUSH: {
        printf(FMT_LINE "push", token->line);
        break;
    }
    case TOKEN_POP: {
        printf(FMT_LINE "pop", token->line);
        break;
    }
    case TOKEN_CALL: {
        printf(FMT_LINE "call", token->line);
        break;
    }
    case TOKEN_RET: {
        printf(FMT_LINE "ret", token->line);
        break;
    }
    case TOKEN_NUM: {
        printf(FMT_LINE "%u", token->line, token->number);
        break;
    }
    case TOKEN_STR: {
        printf(FMT_LINE "\"%s\"", token->line, token->string);
        break;
    }
    case TOKEN_COMMA: {
        printf(FMT_LINE ",", token->line);
        break;
    }
    case TOKEN_COLON: {
        printf(FMT_LINE ":", token->line);
        break;
    }
    }
    printf("\n");
}

#endif
