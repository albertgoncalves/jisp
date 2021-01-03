#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef size_t   usize;

typedef int32_t i32;

typedef FILE File;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

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

#define PRINT_FN_OK() printf("%-14s OK\n", __func__)

#endif
