#ifndef __TOKEN_H__
#define __TOKEN_H__

typedef enum {
    TOKEN_RAX,
    TOKEN_RBX,

    TOKEN_EAX,
    TOKEN_EBX,
    TOKEN_EDI,

    TOKEN_RBP,
    TOKEN_RSP,

    TOKEN_I32,
    TOKEN_STR,

    TOKEN_COMMA,
    TOKEN_COLON,

    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_PLUS,
    TOKEN_MINUS,

    TOKEN_MOV,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_PUSH,
    TOKEN_POP,
    TOKEN_CALL,
    TOKEN_RET,
} TokenTag;

typedef struct {
    const char* string;
    i32         i32;
    u16         line;
    TokenTag    tag;
} Token;

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
    case TOKEN_RBP: {
        fprintf(stream, FMT_LINE "rbp", token.line);
        break;
    }
    case TOKEN_RSP: {
        fprintf(stream, FMT_LINE "rsp", token.line);
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
    case TOKEN_SUB: {
        fprintf(stream, FMT_LINE "sub", token.line);
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
    case TOKEN_I32: {
        fprintf(stream, FMT_LINE "%d", token.line, token.i32);
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
    case TOKEN_LBRACKET: {
        fprintf(stream, FMT_LINE "[", token.line);
        break;
    }
    case TOKEN_RBRACKET: {
        fprintf(stream, FMT_LINE "]", token.line);
        break;
    }
    case TOKEN_PLUS: {
        fprintf(stream, FMT_LINE "+", token.line);
        break;
    }
    case TOKEN_MINUS: {
        fprintf(stream, FMT_LINE "-", token.line);
        break;
    }
    default: {
        ERROR();
    }
    }
    fprintf(stream, "\n");
}

#endif
