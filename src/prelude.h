#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef size_t   usize;

typedef int32_t i32;

typedef FILE File;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

#define ERROR()                                                      \
    {                                                                \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        exit(EXIT_FAILURE);                                          \
    }

#define EXIT_IF(condition)         \
    if (condition) {               \
        fprintf(stderr,            \
                "%s:%s:%d `%s`\n", \
                __FILE__,          \
                __func__,          \
                __LINE__,          \
                #condition);       \
        exit(EXIT_FAILURE);        \
    }

#define PRINT_FN_OK() printf("%-20s OK\n", __func__)

#endif
