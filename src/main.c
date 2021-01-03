#include "compile.h"

i32 main(void) {
    printf("sizeof(i32)     : %zu\n"
           "sizeof(Memory)  : %zu\n"
           "sizeof(Program) : %zu\n"
           "\n",
           sizeof(i32),
           sizeof(Memory),
           sizeof(Program));
    Memory* memory = malloc(sizeof(Memory));
    EXIT_IF(!memory);
    test_compile(memory);
    free(memory);
    return EXIT_SUCCESS;
}
