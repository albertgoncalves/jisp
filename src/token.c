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

static u32 get_decimal(const char* decimal) {
    u32 result = 0;
    while (*decimal) {
        char digit = *(decimal++);
        if (('0' <= digit) && (digit <= '9')) {
            result = (result * 10) + ((u32)(digit - '0'));
        } else {
            ERROR();
        }
    }
    return result;
}

static void set_tokens(Memory* memory) {
    usize size = memory->file_index;
    u16   line = 1;
    for (usize i = 0; i < size;) {
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
            for (; i < size; ++i) {
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
            for (; j < size; ++j) {
                char x = memory->file[j];
                if (!((('a' <= x) && (x <= 'z')) ||
                      (('A' <= x) && (x <= 'Z')) ||
                      (('0' <= x) && (x <= '9'))))
                {
                    break;
                }
            }
            char* buffer = alloc_buffer(memory, (j - i) + 1);
            u32   k = 0;
            while (i < j) {
                buffer[k++] = memory->file[i++];
            }
            buffer[k] = '\0';
            token->string = buffer;
            if (('0' <= buffer[0]) && (buffer[0] <= '9')) {
                token->tag = TOKEN_NUM;
                token->number = get_decimal(buffer);

            } else if (!memcmp(buffer, "rax", k)) {
                token->tag = TOKEN_RAX;
            } else if (!memcmp(buffer, "rbx", k)) {
                token->tag = TOKEN_RBX;

            } else if (!memcmp(buffer, "eax", k)) {
                token->tag = TOKEN_EAX;
            } else if (!memcmp(buffer, "ebx", k)) {
                token->tag = TOKEN_EBX;
            } else if (!memcmp(buffer, "edi", k)) {
                token->tag = TOKEN_EDI;

            } else if (!memcmp(buffer, "mov", k)) {
                token->tag = TOKEN_MOV;
            } else if (!memcmp(buffer, "push", k)) {
                token->tag = TOKEN_PUSH;
            } else if (!memcmp(buffer, "pop", k)) {
                token->tag = TOKEN_POP;
            } else if (!memcmp(buffer, "call", k)) {
                token->tag = TOKEN_CALL;
            } else if (!memcmp(buffer, "ret", k)) {
                token->tag = TOKEN_RET;

            } else {
                token->tag = TOKEN_STR;
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
