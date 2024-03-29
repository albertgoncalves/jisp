#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef size_t   usize;

typedef int32_t i32;

typedef float  f32;
typedef double f64;

typedef FILE File;

#define ERROR()                                                      \
    do {                                                             \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        exit(EXIT_FAILURE);                                          \
    } while (0)

#if 1
    #define EXIT_IF(condition)             \
        do {                               \
            if (condition) {               \
                fprintf(stderr,            \
                        "%s:%s:%d `%s`\n", \
                        __FILE__,          \
                        __func__,          \
                        __LINE__,          \
                        #condition);       \
                exit(EXIT_FAILURE);        \
            }                              \
        } while (0)
#else
    #define EXIT_IF(condition) \
        do {                   \
            if (condition) {   \
            }                  \
        } while (0)
#endif

#define PRINT_FN_OK() printf("%-20s OK\n", __func__)

#endif
