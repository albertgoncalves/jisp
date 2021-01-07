#ifndef __TOKEN_H__
#define __TOKEN_H__

typedef enum {
    TOKEN_RAX,
    TOKEN_RBX,

    TOKEN_EAX,
    TOKEN_EBX,
    TOKEN_EDI,

    TOKEN_NUM,
    TOKEN_STR,

    TOKEN_COMMA,
    TOKEN_COLON,

    TOKEN_MOV,
    TOKEN_PUSH,
    TOKEN_POP,
    TOKEN_CALL,
    TOKEN_RET,
} TokenTag;

typedef struct {
    const char* string;
    i32         number;
    u16         line;
    TokenTag    tag;
} Token;

static Bool is_register(Token token) {
    return token.tag < TOKEN_NUM;
}

#define FMT_LINE "( ln. %-3hu) "

static void print_token(File* stream, Token token) {
    switch (token.tag) {
    case TOKEN_RAX: {
        fprintf(stream, FMT_LINE "rax", token.line);
        break;
    }
    case TOKEN_RBX: {
        fprintf(stream, FMT_LINE "rbx", token.line);
        break;
    }
    case TOKEN_EAX: {
        fprintf(stream, FMT_LINE "eax", token.line);
        break;
    }
    case TOKEN_EBX: {
        fprintf(stream, FMT_LINE "ebx", token.line);
        break;
    }
    case TOKEN_EDI: {
        fprintf(stream, FMT_LINE "edi", token.line);
        break;
    }
    case TOKEN_MOV: {
        fprintf(stream, FMT_LINE "mov", token.line);
        break;
    }
    case TOKEN_ADD: {
        fprintf(stream, FMT_LINE "add", token.line);
        break;
    }
    case TOKEN_PUSH: {
        fprintf(stream, FMT_LINE "push", token.line);
        break;
    }
    case TOKEN_POP: {
        fprintf(stream, FMT_LINE "pop", token.line);
        break;
    }
    case TOKEN_CALL: {
        fprintf(stream, FMT_LINE "call", token.line);
        break;
    }
    case TOKEN_RET: {
        fprintf(stream, FMT_LINE "ret", token.line);
        break;
    }
    case TOKEN_NUM: {
        fprintf(stream, FMT_LINE "%d", token.line, token.number);
        break;
    }
    case TOKEN_STR: {
        fprintf(stream, FMT_LINE "\"%s\"", token.line, token.string);
        break;
    }
    case TOKEN_COMMA: {
        fprintf(stream, FMT_LINE ",", token.line);
        break;
    }
    case TOKEN_COLON: {
        fprintf(stream, FMT_LINE ":", token.line);
        break;
    }
    }
    fprintf(stream, "\n");
}

#endif
