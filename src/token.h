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

#endif
