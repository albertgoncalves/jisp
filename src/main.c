#include "compile.h"
#include "token.c"

static void test_tokens(Memory* memory) {
    {
        const char* source = "    mov     edi, 42\n"
                             "    call    label\n"
                             "    ret\n"
                             "    mov     eax, 43 ; NOTE: Never evaluated!\n"
                             "label:\n"
                             "    push    rbx\n"
                             "    mov     ebx, edi\n"
                             "    mov     eax, ebx\n"
                             "    pop     rbx\n"
                             "    ret\n";
        usize size = strlen(source);
        memcpy(memory->file, source, size);
        memory->file_index = size;
    }
    {
        set_tokens(memory);
        Token* tokens = memory->tokens;
        EXIT_IF(tokens[0].tag != TOKEN_MOV);
        EXIT_IF(tokens[0].line != 1);
        EXIT_IF(tokens[1].tag != TOKEN_EDI);
        EXIT_IF(tokens[1].line != 1);
        EXIT_IF(tokens[2].tag != TOKEN_COMMA);
        EXIT_IF(tokens[2].line != 1);
        EXIT_IF(tokens[3].tag != TOKEN_NUM);
        EXIT_IF(tokens[3].number != 42);
        EXIT_IF(tokens[3].line != 1);
        EXIT_IF(tokens[4].tag != TOKEN_CALL);
        EXIT_IF(tokens[4].line != 2);
        {
            EXIT_IF(tokens[5].tag != TOKEN_STR);
            EXIT_IF(tokens[5].line != 2);
            const char* string = tokens[5].string;
            EXIT_IF(memcmp(string, "label", strlen(string)));
        }
        EXIT_IF(tokens[6].tag != TOKEN_RET);
        EXIT_IF(tokens[6].line != 3);
        EXIT_IF(tokens[7].tag != TOKEN_MOV);
        EXIT_IF(tokens[7].line != 4);
        EXIT_IF(tokens[8].tag != TOKEN_EAX);
        EXIT_IF(tokens[8].line != 4);
        EXIT_IF(tokens[9].tag != TOKEN_COMMA);
        EXIT_IF(tokens[9].line != 4);
        EXIT_IF(tokens[10].tag != TOKEN_NUM);
        EXIT_IF(tokens[10].number != 43);
        EXIT_IF(tokens[10].line != 4);
        {
            EXIT_IF(tokens[11].tag != TOKEN_STR);
            EXIT_IF(tokens[11].line != 5);
            const char* string = tokens[11].string;
            EXIT_IF(memcmp(string, "label", strlen(string)));
        }
        EXIT_IF(tokens[12].tag != TOKEN_COLON);
        EXIT_IF(tokens[12].line != 5);
        EXIT_IF(tokens[13].tag != TOKEN_PUSH);
        EXIT_IF(tokens[13].line != 6);
        EXIT_IF(tokens[14].tag != TOKEN_RBX);
        EXIT_IF(tokens[14].line != 6);
        EXIT_IF(tokens[15].tag != TOKEN_MOV);
        EXIT_IF(tokens[15].line != 7);
        EXIT_IF(tokens[16].tag != TOKEN_EBX);
        EXIT_IF(tokens[16].line != 7);
        EXIT_IF(tokens[17].tag != TOKEN_COMMA);
        EXIT_IF(tokens[17].line != 7);
        EXIT_IF(tokens[18].tag != TOKEN_EDI);
        EXIT_IF(tokens[18].line != 7);
        EXIT_IF(tokens[19].tag != TOKEN_MOV);
        EXIT_IF(tokens[19].line != 8);
        EXIT_IF(tokens[20].tag != TOKEN_EAX);
        EXIT_IF(tokens[20].line != 8);
        EXIT_IF(tokens[21].tag != TOKEN_COMMA);
        EXIT_IF(tokens[21].line != 8);
        EXIT_IF(tokens[22].tag != TOKEN_EBX);
        EXIT_IF(tokens[22].line != 8);
        EXIT_IF(tokens[23].tag != TOKEN_POP);
        EXIT_IF(tokens[23].line != 9);
        EXIT_IF(tokens[24].tag != TOKEN_RBX);
        EXIT_IF(tokens[24].line != 9);
        EXIT_IF(tokens[25].tag != TOKEN_RET);
        EXIT_IF(tokens[25].line != 10);
        EXIT_IF(memory->tokens_index != 26);
        memory->file_index = 0;
        memory->tokens_index = 0;
        memory->buffer_index = 0;
    }
    PRINT_FN_OK();
}

static void test_emit_transform(Memory* memory) {
    memory->bytes_index = 0;
    i32 x = 42;
    {
        /*     - 0-     mov     edi, `x`
         *     - 5-     call    label
         *     -10-     ret
         *     -11-     mov     eax, `x + 1` ; NOTE: Never evaluated!
         */
        emit_mov_edi_imm32(memory);  //  0 + 1 ->  1
        emit_i32(memory, x);         //  1 + 4 ->  5
        emit_call_rel_imm32(memory); //  5 + 1 ->  6
        emit_i32(memory, 6);         //  6 + 4 -> 10
        emit_ret(memory);            // 10 + 1 -> 11
        emit_mov_eax_imm32(memory);  // 11 + 1 -> 12
        emit_i32(memory, x + 1);     // 12 + 4 -> 16
        EXIT_IF(memory->bytes_index != 16);
    }
    {
        /*          label:
         *     -16-     push    rbx
         *     -17-     mov     ebx, edi
         *     -19-     mov     eax, ebx
         *     -21-     pop     rbx
         *     -22-     ret
         */
        emit_push_rbx(memory);    // 16 + 1 -> 17
        emit_mov_ebx_edi(memory); // 17 + 2 -> 19
        emit_mov_eax_ebx(memory); // 19 + 2 -> 21
        emit_pop_rbx(memory);     // 21 + 1 -> 22
        emit_ret(memory);         // 22 + 1 -> 23
        EXIT_IF(memory->bytes_index != 23);
    }
    {
        Program program = transform(memory);
        i32     return_value = (*((FnVoidI32*)&program.buffer))();
        EXIT_IF(x != return_value);
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
        memory->bytes_index = 0;
    }
    PRINT_FN_OK();
}

i32 main(i32 n, const char** args) {
    Memory* memory = malloc(sizeof(Memory));
    EXIT_IF(!memory);
    printf("sizeof(Memory)         : %zu\n"
           "sizeof(memory->file)   : %zu\n"
           "sizeof(memory->bytes)  : %zu\n"
           "sizeof(memory->tokens) : %zu\n"
           "sizeof(memory->buffer) : %zu\n"
           "sizeof(TokenTag)       : %zu\n"
           "sizeof(Token)          : %zu\n"
           "sizeof(Program)        : %zu\n"
           "\n",
           sizeof(Memory),
           sizeof(memory->file),
           sizeof(memory->bytes),
           sizeof(memory->tokens),
           sizeof(memory->buffer),
           sizeof(TokenTag),
           sizeof(Token),
           sizeof(Program));
    EXIT_IF(n < 2);
    test_tokens(memory);
    test_emit_transform(memory);
    {
        set_file(memory, args[1]);
        set_tokens(memory);
        printf("\n%s\n", memory->file);
        for (usize i = 0; i < memory->tokens_index; ++i) {
            print_token(&memory->tokens[i]);
        }
    }
    free(memory);
    return EXIT_SUCCESS;
}
