#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "prelude.h"

#define SIZE_CHAR_BUFFER 192
#define SIZE_BYTE_BUFFER 56

typedef struct {
    char  char_buffer[SIZE_CHAR_BUFFER];
    u8    byte_buffer[SIZE_BYTE_BUFFER];
    usize byte_buffer_index;
} Memory;

#endif
