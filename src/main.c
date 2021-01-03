#include "compile.h"

static void set_file(Memory* memory, const char* filename) {
    File* file = fopen(filename, "r");
    EXIT_IF(!file);
    fseek(file, 0, SEEK_END);
    u32 file_size = (u32)ftell(file);
    EXIT_IF(sizeof(memory->char_buffer) <= file_size);
    rewind(file);
    EXIT_IF(fread(&memory->char_buffer, sizeof(char), file_size, file) !=
            file_size);
    memory->char_buffer[file_size] = '\0';
    fclose(file);
}

i32 main(i32 n, const char** args) {
    Memory* memory = malloc(sizeof(Memory));
    EXIT_IF(!memory);
    printf("sizeof(i32)                 : %zu\n"
           "sizeof(Memory)              : %zu\n"
           "sizeof(memory->byte_buffer) : %zu\n"
           "sizeof(memory->char_buffer) : %zu\n"
           "sizeof(Program)             : %zu\n"
           "\n",
           sizeof(i32),
           sizeof(Memory),
           sizeof(memory->byte_buffer),
           sizeof(memory->char_buffer),
           sizeof(Program));
    EXIT_IF(n < 2);
    set_file(memory, args[1]);
    printf("%s\n\n", memory->char_buffer);
    test_compile(memory);
    free(memory);
    return EXIT_SUCCESS;
}
