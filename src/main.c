#include "prelude.h"

// NOTE: See `https://bernsteinbear.com/blog/compiling-a-lisp-1/`.

#include <string.h>
#include <sys/mman.h>

typedef i32 (*JitFn)(void);

// clang-format off
const u8 PROGRAM[] = {
    // mov eax,     42
    0xB8,           0x2A, 0x00, 0x00, 0x00,
    // ret
    0xC3,
};
// clang-format on

const usize SIZE_PROGRAM = sizeof(PROGRAM);

i32 main(void) {
    void* program = mmap(NULL,
                         SIZE_PROGRAM,
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE,
                         -1,
                         0);
    if (program == MAP_FAILED) {
        ERROR("mmap");
    }
    memcpy(program, PROGRAM, SIZE_PROGRAM);
    if (mprotect(program, SIZE_PROGRAM, PROT_EXEC)) {
        ERROR("mprotect");
    }
    {
        JitFn jit_fn = *((JitFn*)&program);
        printf("%d\n", jit_fn());
    }
    if (munmap(program, SIZE_PROGRAM)) {
        ERROR("munmap");
    }
    return EXIT_SUCCESS;
}
