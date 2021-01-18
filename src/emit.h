#ifndef __EMIT_H__
#define __EMIT_H__

#include "prelude.h"

typedef struct {
    void* buffer;
} Program;

typedef i32 (*FnVoidI32)(void);
typedef f32 (*FnVoidF32)(void);

#endif
