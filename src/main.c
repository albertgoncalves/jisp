#include "emit.c"
#include "parse.c"
#include "token.c"

/* NOTE: Useful resources:
 * `https://bernsteinbear.com/blog/lisp/`
 * `http://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf`
 * `http://6.s081.scripts.mit.edu/sp18/x86-64-architecture-guide.html`
 * `https://www.felixcloutier.com/x86/call`
 * `https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html`
 */

#define EPSILON 0.00001f

static void test_compile_1(Memory* memory) {
    reset(memory);
    {
        const char* source = "    mov     edi, 42\n"
                             "    call    label\n"
                             "    ret\n"
                             "    mov     eax, -43 ; NOTE: Never evaluated!\n"
                             "label:\n"
                             "    push    rbx\n"
                             "    mov     ebx, edi\n"
                             "    mov     eax, ebx\n"
                             "    pop     rbx\n"
                             "    ret\n";
        usize       size = strlen(source);
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

        EXIT_IF(tokens[3].tag != TOKEN_I32);
        EXIT_IF(tokens[3].i32 != 42);
        EXIT_IF(tokens[3].line != 1);

        EXIT_IF(tokens[4].tag != TOKEN_CALL);
        EXIT_IF(tokens[4].line != 2);
        {
            EXIT_IF(tokens[5].tag != TOKEN_STR);
            const char* string = tokens[5].string;
            EXIT_IF(memcmp(string, "label", strlen(string)));
            EXIT_IF(tokens[5].line != 2);
        }
        EXIT_IF(tokens[6].tag != TOKEN_RET);
        EXIT_IF(tokens[6].line != 3);

        EXIT_IF(tokens[7].tag != TOKEN_MOV);
        EXIT_IF(tokens[7].line != 4);

        EXIT_IF(tokens[8].tag != TOKEN_EAX);
        EXIT_IF(tokens[8].line != 4);

        EXIT_IF(tokens[9].tag != TOKEN_COMMA);
        EXIT_IF(tokens[9].line != 4);

        EXIT_IF(tokens[10].tag != TOKEN_I32);
        EXIT_IF(tokens[10].i32 != -43);
        EXIT_IF(tokens[10].line != 4);
        {
            EXIT_IF(tokens[11].tag != TOKEN_STR);
            const char* string = tokens[11].string;
            EXIT_IF(memcmp(string, "label", strlen(string)));
            EXIT_IF(tokens[11].line != 5);
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
        EXIT_IF(memory->buffer_index != 12);
    }
    {
        set_insts(memory);
        EXIT_IF(memory->labels_index != 1);
        {
            const char* string = memory->labels[0].string;
            EXIT_IF(memcmp(string, "label", strlen(string)));
            EXIT_IF(memory->labels[0].position != 16);
        }
        resolve_insts(memory);
        Inst* insts = memory->insts;

        EXIT_IF(insts[0].tag != INST_MOV_REG_IMM_I32);
        EXIT_IF(insts[0].dst.reg != REG_EDI);
        EXIT_IF(insts[0].dst.line != 1);
        EXIT_IF(insts[0].src.imm_i32 != 42);
        EXIT_IF(insts[0].src.line != 1);
        EXIT_IF(insts[0].position != 0);
        EXIT_IF(insts[0].size != 5);

        EXIT_IF(insts[1].tag != INST_CALL_REL_IMM_I32);
        EXIT_IF(insts[1].dst.imm_i32 != 6);
        EXIT_IF(insts[1].dst.line != 2);
        EXIT_IF(insts[1].position != 5);
        EXIT_IF(insts[1].size != 5);

        EXIT_IF(insts[2].tag != INST_RET);
        EXIT_IF(insts[2].position != 10);
        EXIT_IF(insts[2].size != 1);

        EXIT_IF(insts[3].tag != INST_MOV_REG_IMM_I32);
        EXIT_IF(insts[3].dst.reg != REG_EAX);
        EXIT_IF(insts[3].dst.line != 4);
        EXIT_IF(insts[3].src.imm_i32 != -43);
        EXIT_IF(insts[3].src.line != 4);
        EXIT_IF(insts[3].position != 11);
        EXIT_IF(insts[3].size != 5);

        EXIT_IF(insts[4].tag != INST_PUSH_REG);
        EXIT_IF(insts[4].src.reg != REG_RBX);
        EXIT_IF(insts[4].src.line != 6);
        EXIT_IF(insts[4].position != 16);
        EXIT_IF(insts[4].size != 1);

        EXIT_IF(insts[5].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[5].dst.reg != REG_EBX);
        EXIT_IF(insts[5].dst.line != 7);
        EXIT_IF(insts[5].src.reg != REG_EDI);
        EXIT_IF(insts[5].src.line != 7);
        EXIT_IF(insts[5].position != 17);
        EXIT_IF(insts[5].size != 2);

        EXIT_IF(insts[6].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[6].dst.reg != REG_EAX);
        EXIT_IF(insts[6].dst.line != 8);
        EXIT_IF(insts[6].src.reg != REG_EBX);
        EXIT_IF(insts[6].src.line != 8);
        EXIT_IF(insts[6].position != 19);
        EXIT_IF(insts[6].size != 2);

        EXIT_IF(insts[7].tag != INST_POP_REG);
        EXIT_IF(insts[7].dst.reg != REG_RBX);
        EXIT_IF(insts[7].dst.line != 9);
        EXIT_IF(insts[7].position != 21);
        EXIT_IF(insts[7].size != 1);

        EXIT_IF(insts[8].tag != INST_RET);
        EXIT_IF(insts[8].position != 22);
        EXIT_IF(insts[8].size != 1);

        EXIT_IF(memory->insts_index != 9);
    }
    {
        set_bytes(memory);
        EXIT_IF(memory->bytes_index != 23);
        Program program = transform(memory);
        EXIT_IF(42 != (*((FnVoidI32*)&program.buffer))());
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

static void test_compile_2(Memory* memory) {
    reset(memory);
    {
        const char* source = "    mov     edi, 1\n"
                             "    call    main\n"
                             "    ret\n"
                             "f:\n"
                             "    push    rbx\n"
                             "    mov     [rsp - 4], edi\n"
                             "    mov     ebx, [rsp - 4]\n"
                             "    mov     eax, ebx\n"
                             "    add     eax, -87\n"
                             "    pop     rbx\n"
                             "    ret\n"
                             "main:\n"
                             "    mov     edi, 10\n"
                             "    call    f\n"
                             "    ret\n";
        usize       size = strlen(source);
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

        EXIT_IF(tokens[3].tag != TOKEN_I32);
        EXIT_IF(tokens[3].i32 != 1);
        EXIT_IF(tokens[3].line != 1);

        EXIT_IF(tokens[4].tag != TOKEN_CALL);
        EXIT_IF(tokens[4].line != 2);
        {
            EXIT_IF(tokens[5].tag != TOKEN_STR);
            const char* string = tokens[5].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(tokens[5].line != 2);
        }
        EXIT_IF(tokens[6].tag != TOKEN_RET);
        EXIT_IF(tokens[6].line != 3);
        {
            EXIT_IF(tokens[7].tag != TOKEN_STR);
            const char* string = tokens[7].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(tokens[7].line != 4);
        }
        EXIT_IF(tokens[8].tag != TOKEN_COLON);
        EXIT_IF(tokens[8].line != 4);

        EXIT_IF(tokens[9].tag != TOKEN_PUSH);
        EXIT_IF(tokens[9].line != 5);

        EXIT_IF(tokens[10].tag != TOKEN_RBX);
        EXIT_IF(tokens[10].line != 5);

        EXIT_IF(tokens[11].tag != TOKEN_MOV);
        EXIT_IF(tokens[11].line != 6);

        EXIT_IF(tokens[12].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[12].line != 6);

        EXIT_IF(tokens[13].tag != TOKEN_RSP);
        EXIT_IF(tokens[13].line != 6);

        EXIT_IF(tokens[14].tag != TOKEN_MINUS);
        EXIT_IF(tokens[14].line != 6);

        EXIT_IF(tokens[15].tag != TOKEN_I32);
        EXIT_IF(tokens[15].i32 != 4);
        EXIT_IF(tokens[15].line != 6);

        EXIT_IF(tokens[16].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[16].line != 6);

        EXIT_IF(tokens[17].tag != TOKEN_COMMA);
        EXIT_IF(tokens[17].line != 6);

        EXIT_IF(tokens[18].tag != TOKEN_EDI);
        EXIT_IF(tokens[18].line != 6);

        EXIT_IF(tokens[19].tag != TOKEN_MOV);
        EXIT_IF(tokens[19].line != 7);

        EXIT_IF(tokens[20].tag != TOKEN_EBX);
        EXIT_IF(tokens[20].line != 7);

        EXIT_IF(tokens[21].tag != TOKEN_COMMA);
        EXIT_IF(tokens[21].line != 7);

        EXIT_IF(tokens[22].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[22].line != 7);

        EXIT_IF(tokens[23].tag != TOKEN_RSP);
        EXIT_IF(tokens[23].line != 7);

        EXIT_IF(tokens[24].tag != TOKEN_MINUS);
        EXIT_IF(tokens[24].line != 7);

        EXIT_IF(tokens[25].tag != TOKEN_I32);
        EXIT_IF(tokens[25].i32 != 4);
        EXIT_IF(tokens[25].line != 7);

        EXIT_IF(tokens[26].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[26].line != 7);

        EXIT_IF(tokens[27].tag != TOKEN_MOV);
        EXIT_IF(tokens[27].line != 8);

        EXIT_IF(tokens[28].tag != TOKEN_EAX);
        EXIT_IF(tokens[28].line != 8);

        EXIT_IF(tokens[29].tag != TOKEN_COMMA);
        EXIT_IF(tokens[29].line != 8);

        EXIT_IF(tokens[30].tag != TOKEN_EBX);
        EXIT_IF(tokens[30].line != 8);

        EXIT_IF(tokens[31].tag != TOKEN_ADD);
        EXIT_IF(tokens[31].line != 9);

        EXIT_IF(tokens[32].tag != TOKEN_EAX);
        EXIT_IF(tokens[32].line != 9);

        EXIT_IF(tokens[33].tag != TOKEN_COMMA);
        EXIT_IF(tokens[33].line != 9);

        EXIT_IF(tokens[34].tag != TOKEN_I32);
        EXIT_IF(tokens[34].i32 != -87);
        EXIT_IF(tokens[34].line != 9);

        EXIT_IF(tokens[35].tag != TOKEN_POP);
        EXIT_IF(tokens[35].line != 10);

        EXIT_IF(tokens[36].tag != TOKEN_RBX);
        EXIT_IF(tokens[36].line != 10);

        EXIT_IF(tokens[37].tag != TOKEN_RET);
        EXIT_IF(tokens[37].line != 11);
        {
            EXIT_IF(tokens[38].tag != TOKEN_STR);
            const char* string = tokens[38].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(tokens[38].line != 12);
        }
        EXIT_IF(tokens[39].tag != TOKEN_COLON);
        EXIT_IF(tokens[39].line != 12);

        EXIT_IF(tokens[40].tag != TOKEN_MOV);
        EXIT_IF(tokens[40].line != 13);

        EXIT_IF(tokens[41].tag != TOKEN_EDI);
        EXIT_IF(tokens[41].line != 13);

        EXIT_IF(tokens[42].tag != TOKEN_COMMA);
        EXIT_IF(tokens[42].line != 13);

        EXIT_IF(tokens[43].tag != TOKEN_I32);
        EXIT_IF(tokens[43].i32 != 10);
        EXIT_IF(tokens[43].line != 13);

        EXIT_IF(tokens[44].tag != TOKEN_CALL);
        EXIT_IF(tokens[44].line != 14);
        {
            EXIT_IF(tokens[45].tag != TOKEN_STR);
            const char* string = tokens[45].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(tokens[45].line != 14);
        }
        EXIT_IF(tokens[46].tag != TOKEN_RET);
        EXIT_IF(tokens[46].line != 15);

        EXIT_IF(memory->tokens_index != 47);
        EXIT_IF(memory->buffer_index != 14);
    }
    {
        set_insts(memory);
        EXIT_IF(memory->labels_index != 2);
        {
            const char* string = memory->labels[0].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(memory->labels[0].position != 11);
        }
        {
            const char* string = memory->labels[1].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(memory->labels[1].position != 35);
        }
        resolve_insts(memory);
        Inst* insts = memory->insts;

        EXIT_IF(insts[0].tag != INST_MOV_REG_IMM_I32);
        EXIT_IF(insts[0].dst.reg != REG_EDI);
        EXIT_IF(insts[0].dst.line != 1);
        EXIT_IF(insts[0].src.imm_i32 != 1);
        EXIT_IF(insts[0].src.line != 1);
        EXIT_IF(insts[0].position != 0);
        EXIT_IF(insts[0].size != 5);

        EXIT_IF(insts[1].tag != INST_CALL_REL_IMM_I32);
        EXIT_IF(insts[1].dst.imm_i32 != 25);
        EXIT_IF(insts[1].dst.line != 2);
        EXIT_IF(insts[1].position != 5);
        EXIT_IF(insts[1].size != 5);

        EXIT_IF(insts[2].tag != INST_RET);
        EXIT_IF(insts[2].position != 10);
        EXIT_IF(insts[2].size != 1);

        EXIT_IF(insts[3].tag != INST_PUSH_REG);
        EXIT_IF(insts[3].src.reg != REG_RBX);
        EXIT_IF(insts[3].src.line != 5);
        EXIT_IF(insts[3].position != 11);
        EXIT_IF(insts[3].size != 1);

        EXIT_IF(insts[4].tag != INST_MOV_ADDR_OFFSET_REG);
        EXIT_IF(insts[4].dst.reg != REG_RSP);
        EXIT_IF(insts[4].dst.addr_offset != -4);
        EXIT_IF(insts[4].dst.line != 6);
        EXIT_IF(insts[4].src.reg != REG_EDI);
        EXIT_IF(insts[4].src.line != 6);
        EXIT_IF(insts[4].position != 12);
        EXIT_IF(insts[4].size != 7);

        EXIT_IF(insts[5].tag != INST_MOV_REG_ADDR_OFFSET);
        EXIT_IF(insts[5].dst.reg != REG_EBX);
        EXIT_IF(insts[5].dst.line != 7);
        EXIT_IF(insts[5].src.reg != REG_RSP);
        EXIT_IF(insts[5].src.addr_offset != -4);
        EXIT_IF(insts[5].src.line != 7);
        EXIT_IF(insts[5].position != 19);
        EXIT_IF(insts[5].size != 7);

        EXIT_IF(insts[6].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[6].dst.reg != REG_EAX);
        EXIT_IF(insts[6].dst.line != 8);
        EXIT_IF(insts[6].src.reg != REG_EBX);
        EXIT_IF(insts[6].src.line != 8);
        EXIT_IF(insts[6].position != 26);
        EXIT_IF(insts[6].size != 2);

        EXIT_IF(insts[7].tag != INST_ADD_REG_IMM_I32);
        EXIT_IF(insts[7].dst.reg != REG_EAX);
        EXIT_IF(insts[7].dst.line != 9);
        EXIT_IF(insts[7].src.imm_i32 != -87);
        EXIT_IF(insts[7].src.line != 9);
        EXIT_IF(insts[7].position != 28);
        EXIT_IF(insts[7].size != 5);

        EXIT_IF(insts[8].tag != INST_POP_REG);
        EXIT_IF(insts[8].dst.reg != REG_RBX);
        EXIT_IF(insts[8].dst.line != 10);
        EXIT_IF(insts[8].position != 33);
        EXIT_IF(insts[8].size != 1);

        EXIT_IF(insts[9].tag != INST_RET);
        EXIT_IF(insts[9].position != 34);
        EXIT_IF(insts[9].size != 1);

        EXIT_IF(insts[10].tag != INST_MOV_REG_IMM_I32);
        EXIT_IF(insts[10].dst.reg != REG_EDI);
        EXIT_IF(insts[10].dst.line != 13);
        EXIT_IF(insts[10].src.imm_i32 != 10);
        EXIT_IF(insts[10].src.line != 13);
        EXIT_IF(insts[10].position != 35);
        EXIT_IF(insts[10].size != 5);

        EXIT_IF(insts[11].tag != INST_CALL_REL_IMM_I32);
        EXIT_IF(insts[11].dst.imm_i32 != -34);
        EXIT_IF(insts[11].dst.line != 14);
        EXIT_IF(insts[11].position != 40);
        EXIT_IF(insts[11].size != 5);

        EXIT_IF(insts[12].tag != INST_RET);
        EXIT_IF(insts[12].position != 45);
        EXIT_IF(insts[12].size != 1);

        EXIT_IF(memory->insts_index != 13);
    }
    {
        set_bytes(memory);
        EXIT_IF(memory->bytes_index != 46);
        Program program = transform(memory);
        EXIT_IF(-77 != (*((FnVoidI32*)&program.buffer))());
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

static void test_compile_3(Memory* memory) {
    reset(memory);
    {
        const char* source = "    call    main\n"
                             "    ret\n"
                             "f:\n"
                             "    push    rbp\n"
                             "    mov     rbp, rsp\n"
                             "    push    rbx\n"
                             "    mov     eax, [rbp + 20]\n"
                             "    mov     ebx, [rbp + 16]\n"
                             "    sub     eax, ebx\n"
                             "    pop     rbx\n"
                             "    mov     rsp, rbp\n"
                             "    pop     rbp\n"
                             "    ret\n"
                             "main:\n"
                             "    push    rbp\n"
                             "    mov     rbp, rsp\n"
                             "    sub     rsp, 8\n"
                             "    mov     [rsp + 4], 45 ; => `[rbp - 4]`\n"
                             "    mov     [rsp], 99     ; => `[rbp - 8]`\n"
                             "    call    f\n"
                             "    mov     rsp, rbp\n"
                             "    pop     rbp\n"
                             "    ret\n";
        usize       size = strlen(source);
        memcpy(memory->file, source, size);
        memory->file_index = size;
    }
    {
        set_tokens(memory);
        Token* tokens = memory->tokens;

        EXIT_IF(tokens[0].tag != TOKEN_CALL);
        EXIT_IF(tokens[0].line != 1);
        {
            EXIT_IF(tokens[1].tag != TOKEN_STR);
            const char* string = tokens[1].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(tokens[1].line != 1);
        }
        EXIT_IF(tokens[2].tag != TOKEN_RET);
        EXIT_IF(tokens[2].line != 2);
        {
            EXIT_IF(tokens[3].tag != TOKEN_STR);
            const char* string = tokens[3].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(tokens[3].line != 3);
        }
        EXIT_IF(tokens[4].tag != TOKEN_COLON);
        EXIT_IF(tokens[4].line != 3);

        EXIT_IF(tokens[5].tag != TOKEN_PUSH);
        EXIT_IF(tokens[5].line != 4);

        EXIT_IF(tokens[6].tag != TOKEN_RBP);
        EXIT_IF(tokens[6].line != 4);

        EXIT_IF(tokens[7].tag != TOKEN_MOV);
        EXIT_IF(tokens[7].line != 5);

        EXIT_IF(tokens[8].tag != TOKEN_RBP);
        EXIT_IF(tokens[8].line != 5);

        EXIT_IF(tokens[9].tag != TOKEN_COMMA);
        EXIT_IF(tokens[9].line != 5);

        EXIT_IF(tokens[10].tag != TOKEN_RSP);
        EXIT_IF(tokens[10].line != 5);

        EXIT_IF(tokens[11].tag != TOKEN_PUSH);
        EXIT_IF(tokens[11].line != 6);

        EXIT_IF(tokens[12].tag != TOKEN_RBX);
        EXIT_IF(tokens[12].line != 6);

        EXIT_IF(tokens[13].tag != TOKEN_MOV);
        EXIT_IF(tokens[13].line != 7);

        EXIT_IF(tokens[14].tag != TOKEN_EAX);
        EXIT_IF(tokens[14].line != 7);

        EXIT_IF(tokens[15].tag != TOKEN_COMMA);
        EXIT_IF(tokens[15].line != 7);

        EXIT_IF(tokens[16].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[16].line != 7);

        EXIT_IF(tokens[17].tag != TOKEN_RBP);
        EXIT_IF(tokens[17].line != 7);

        EXIT_IF(tokens[18].tag != TOKEN_PLUS);
        EXIT_IF(tokens[18].line != 7);

        EXIT_IF(tokens[19].tag != TOKEN_I32);
        EXIT_IF(tokens[19].i32 != 20);
        EXIT_IF(tokens[19].line != 7);

        EXIT_IF(tokens[20].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[20].line != 7);

        EXIT_IF(tokens[21].tag != TOKEN_MOV);
        EXIT_IF(tokens[21].line != 8);

        EXIT_IF(tokens[22].tag != TOKEN_EBX);
        EXIT_IF(tokens[22].line != 8);

        EXIT_IF(tokens[23].tag != TOKEN_COMMA);
        EXIT_IF(tokens[23].line != 8);

        EXIT_IF(tokens[24].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[24].line != 8);

        EXIT_IF(tokens[25].tag != TOKEN_RBP);
        EXIT_IF(tokens[25].line != 8);

        EXIT_IF(tokens[26].tag != TOKEN_PLUS);
        EXIT_IF(tokens[26].line != 8);

        EXIT_IF(tokens[27].tag != TOKEN_I32);
        EXIT_IF(tokens[27].i32 != 16);
        EXIT_IF(tokens[27].line != 8);

        EXIT_IF(tokens[28].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[28].line != 8);

        EXIT_IF(tokens[29].tag != TOKEN_SUB);
        EXIT_IF(tokens[29].line != 9);

        EXIT_IF(tokens[30].tag != TOKEN_EAX);
        EXIT_IF(tokens[30].line != 9);

        EXIT_IF(tokens[31].tag != TOKEN_COMMA);
        EXIT_IF(tokens[31].line != 9);

        EXIT_IF(tokens[32].tag != TOKEN_EBX);
        EXIT_IF(tokens[32].line != 9);

        EXIT_IF(tokens[33].tag != TOKEN_POP);
        EXIT_IF(tokens[33].line != 10);

        EXIT_IF(tokens[34].tag != TOKEN_RBX);
        EXIT_IF(tokens[34].line != 10);

        EXIT_IF(tokens[35].tag != TOKEN_MOV);
        EXIT_IF(tokens[35].line != 11);

        EXIT_IF(tokens[36].tag != TOKEN_RSP);
        EXIT_IF(tokens[36].line != 11);

        EXIT_IF(tokens[37].tag != TOKEN_COMMA);
        EXIT_IF(tokens[37].line != 11);

        EXIT_IF(tokens[38].tag != TOKEN_RBP);
        EXIT_IF(tokens[38].line != 11);

        EXIT_IF(tokens[39].tag != TOKEN_POP);
        EXIT_IF(tokens[39].line != 12);

        EXIT_IF(tokens[40].tag != TOKEN_RBP);
        EXIT_IF(tokens[40].line != 12);

        EXIT_IF(tokens[41].tag != TOKEN_RET);
        EXIT_IF(tokens[41].line != 13);
        {
            EXIT_IF(tokens[42].tag != TOKEN_STR);
            const char* string = tokens[42].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(tokens[42].line != 14);
        }
        EXIT_IF(tokens[43].tag != TOKEN_COLON);
        EXIT_IF(tokens[43].line != 14);

        EXIT_IF(tokens[44].tag != TOKEN_PUSH);
        EXIT_IF(tokens[44].line != 15);

        EXIT_IF(tokens[45].tag != TOKEN_RBP);
        EXIT_IF(tokens[45].line != 15);

        EXIT_IF(tokens[46].tag != TOKEN_MOV);
        EXIT_IF(tokens[46].line != 16);

        EXIT_IF(tokens[47].tag != TOKEN_RBP);
        EXIT_IF(tokens[47].line != 16);

        EXIT_IF(tokens[48].tag != TOKEN_COMMA);
        EXIT_IF(tokens[48].line != 16);

        EXIT_IF(tokens[49].tag != TOKEN_RSP);
        EXIT_IF(tokens[49].line != 16);

        EXIT_IF(tokens[50].tag != TOKEN_SUB);
        EXIT_IF(tokens[50].line != 17);

        EXIT_IF(tokens[51].tag != TOKEN_RSP);
        EXIT_IF(tokens[51].line != 17);

        EXIT_IF(tokens[52].tag != TOKEN_COMMA);
        EXIT_IF(tokens[52].line != 17);

        EXIT_IF(tokens[53].tag != TOKEN_I32);
        EXIT_IF(tokens[53].i32 != 8);
        EXIT_IF(tokens[53].line != 17);

        EXIT_IF(tokens[54].tag != TOKEN_MOV);
        EXIT_IF(tokens[54].line != 18);

        EXIT_IF(tokens[55].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[55].line != 18);

        EXIT_IF(tokens[56].tag != TOKEN_RSP);
        EXIT_IF(tokens[56].line != 18);

        EXIT_IF(tokens[57].tag != TOKEN_PLUS);
        EXIT_IF(tokens[57].line != 18);

        EXIT_IF(tokens[58].tag != TOKEN_I32);
        EXIT_IF(tokens[58].i32 != 4);
        EXIT_IF(tokens[58].line != 18);

        EXIT_IF(tokens[59].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[59].line != 18);

        EXIT_IF(tokens[60].tag != TOKEN_COMMA);
        EXIT_IF(tokens[60].line != 18);

        EXIT_IF(tokens[61].tag != TOKEN_I32);
        EXIT_IF(tokens[61].i32 != 45);
        EXIT_IF(tokens[61].line != 18);

        EXIT_IF(tokens[62].tag != TOKEN_MOV);
        EXIT_IF(tokens[62].line != 19);

        EXIT_IF(tokens[63].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[63].line != 19);

        EXIT_IF(tokens[64].tag != TOKEN_RSP);
        EXIT_IF(tokens[64].line != 19);

        EXIT_IF(tokens[65].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[65].line != 19);

        EXIT_IF(tokens[66].tag != TOKEN_COMMA);
        EXIT_IF(tokens[66].line != 19);

        EXIT_IF(tokens[67].tag != TOKEN_I32);
        EXIT_IF(tokens[67].i32 != 99);
        EXIT_IF(tokens[67].line != 19);

        EXIT_IF(tokens[68].tag != TOKEN_CALL);
        EXIT_IF(tokens[68].line != 20);
        {
            EXIT_IF(tokens[69].tag != TOKEN_STR);
            const char* string = tokens[69].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(tokens[69].line != 20);
        }
        EXIT_IF(tokens[70].tag != TOKEN_MOV);
        EXIT_IF(tokens[70].line != 21);

        EXIT_IF(tokens[71].tag != TOKEN_RSP);
        EXIT_IF(tokens[71].line != 21);

        EXIT_IF(tokens[72].tag != TOKEN_COMMA);
        EXIT_IF(tokens[72].line != 21);

        EXIT_IF(tokens[73].tag != TOKEN_RBP);
        EXIT_IF(tokens[73].line != 21);

        EXIT_IF(tokens[74].tag != TOKEN_POP);
        EXIT_IF(tokens[74].line != 22);

        EXIT_IF(tokens[75].tag != TOKEN_RBP);
        EXIT_IF(tokens[75].line != 22);

        EXIT_IF(tokens[76].tag != TOKEN_RET);
        EXIT_IF(tokens[76].line != 23);

        EXIT_IF(memory->tokens_index != 77);
        EXIT_IF(memory->buffer_index != 14);
    }
    {
        set_insts(memory);
        EXIT_IF(memory->labels_index != 2);
        {
            const char* string = memory->labels[0].string;
            EXIT_IF(memcmp(string, "f", strlen(string)));
            EXIT_IF(memory->labels[0].position != 6);
        }
        {
            const char* string = memory->labels[1].string;
            EXIT_IF(memcmp(string, "main", strlen(string)));
            EXIT_IF(memory->labels[1].position != 31);
        }
        resolve_insts(memory);
        Inst* insts = memory->insts;

        EXIT_IF(insts[0].tag != INST_CALL_REL_IMM_I32);
        EXIT_IF(insts[0].dst.imm_i32 != 26);
        EXIT_IF(insts[0].dst.line != 1);
        EXIT_IF(insts[0].position != 0);
        EXIT_IF(insts[0].size != 5);

        EXIT_IF(insts[1].tag != INST_RET);
        EXIT_IF(insts[1].position != 5);
        EXIT_IF(insts[1].size != 1);

        EXIT_IF(insts[2].tag != INST_PUSH_REG);
        EXIT_IF(insts[2].src.reg != REG_RBP);
        EXIT_IF(insts[2].src.line != 4);
        EXIT_IF(insts[2].position != 6);
        EXIT_IF(insts[2].size != 1);

        EXIT_IF(insts[3].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[3].dst.reg != REG_RBP);
        EXIT_IF(insts[3].dst.line != 5);
        EXIT_IF(insts[3].src.reg != REG_RSP);
        EXIT_IF(insts[3].src.line != 5);
        EXIT_IF(insts[3].position != 7);
        EXIT_IF(insts[3].size != 3);

        EXIT_IF(insts[4].tag != INST_PUSH_REG);
        EXIT_IF(insts[4].src.reg != REG_RBX);
        EXIT_IF(insts[4].src.line != 6);
        EXIT_IF(insts[4].position != 10);
        EXIT_IF(insts[4].size != 1);

        EXIT_IF(insts[5].tag != INST_MOV_REG_ADDR_OFFSET);
        EXIT_IF(insts[5].dst.reg != REG_EAX);
        EXIT_IF(insts[5].dst.line != 7);
        EXIT_IF(insts[5].src.reg != REG_RBP);
        EXIT_IF(insts[5].src.addr_offset != 20);
        EXIT_IF(insts[5].src.line != 7);
        EXIT_IF(insts[5].position != 11);
        EXIT_IF(insts[5].size != 6);

        EXIT_IF(insts[6].tag != INST_MOV_REG_ADDR_OFFSET);
        EXIT_IF(insts[6].dst.reg != REG_EBX);
        EXIT_IF(insts[6].dst.line != 8);
        EXIT_IF(insts[6].src.reg != REG_RBP);
        EXIT_IF(insts[6].src.addr_offset != 16);
        EXIT_IF(insts[6].src.line != 8);
        EXIT_IF(insts[6].position != 17);
        EXIT_IF(insts[6].size != 6);

        EXIT_IF(insts[7].tag != INST_SUB_REG_REG);
        EXIT_IF(insts[7].dst.reg != REG_EAX);
        EXIT_IF(insts[7].dst.line != 9);
        EXIT_IF(insts[7].src.reg != REG_EBX);
        EXIT_IF(insts[7].src.line != 9);
        EXIT_IF(insts[7].position != 23);
        EXIT_IF(insts[7].size != 2);

        EXIT_IF(insts[8].tag != INST_POP_REG);
        EXIT_IF(insts[8].dst.reg != REG_RBX);
        EXIT_IF(insts[8].dst.line != 10);
        EXIT_IF(insts[8].position != 25);
        EXIT_IF(insts[8].size != 1);

        EXIT_IF(insts[9].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[9].dst.reg != REG_RSP);
        EXIT_IF(insts[9].dst.line != 11);
        EXIT_IF(insts[9].src.reg != REG_RBP);
        EXIT_IF(insts[9].src.line != 11);
        EXIT_IF(insts[9].position != 26);
        EXIT_IF(insts[9].size != 3);

        EXIT_IF(insts[10].tag != INST_POP_REG);
        EXIT_IF(insts[10].dst.reg != REG_RBP);
        EXIT_IF(insts[10].dst.line != 12);
        EXIT_IF(insts[10].position != 29);
        EXIT_IF(insts[10].size != 1);

        EXIT_IF(insts[11].tag != INST_RET);
        EXIT_IF(insts[11].position != 30);
        EXIT_IF(insts[11].size != 1);

        EXIT_IF(insts[12].tag != INST_PUSH_REG);
        EXIT_IF(insts[12].src.reg != REG_RBP);
        EXIT_IF(insts[12].src.line != 15);
        EXIT_IF(insts[12].position != 31);
        EXIT_IF(insts[12].size != 1);

        EXIT_IF(insts[13].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[13].dst.reg != REG_RBP);
        EXIT_IF(insts[13].dst.line != 16);
        EXIT_IF(insts[13].src.reg != REG_RSP);
        EXIT_IF(insts[13].src.line != 16);
        EXIT_IF(insts[13].position != 32);
        EXIT_IF(insts[13].size != 3);

        EXIT_IF(insts[14].tag != INST_SUB_REG_IMM_I32);
        EXIT_IF(insts[14].dst.reg != REG_RSP);
        EXIT_IF(insts[14].dst.line != 17);
        EXIT_IF(insts[14].src.imm_i32 != 8);
        EXIT_IF(insts[14].src.line != 17);
        EXIT_IF(insts[14].position != 35);
        EXIT_IF(insts[14].size != 7);

        EXIT_IF(insts[15].tag != INST_MOV_ADDR_OFFSET_IMM_I32);
        EXIT_IF(insts[15].dst.reg != REG_RSP);
        EXIT_IF(insts[15].dst.addr_offset != 4);
        EXIT_IF(insts[15].dst.line != 18);
        EXIT_IF(insts[15].src.imm_i32 != 45);
        EXIT_IF(insts[15].src.line != 18);
        EXIT_IF(insts[15].position != 42);
        EXIT_IF(insts[15].size != 11);

        EXIT_IF(insts[16].tag != INST_MOV_ADDR_OFFSET_IMM_I32);
        EXIT_IF(insts[16].dst.reg != REG_RSP);
        EXIT_IF(insts[16].dst.addr_offset != 0);
        EXIT_IF(insts[16].dst.line != 19);
        EXIT_IF(insts[16].src.imm_i32 != 99);
        EXIT_IF(insts[16].src.line != 19);
        EXIT_IF(insts[16].position != 53);
        EXIT_IF(insts[16].size != 11);

        EXIT_IF(insts[17].tag != INST_CALL_REL_IMM_I32);
        EXIT_IF(insts[17].dst.imm_i32 != -63);
        EXIT_IF(insts[17].dst.line != 20);
        EXIT_IF(insts[17].position != 64);
        EXIT_IF(insts[17].size != 5);

        EXIT_IF(insts[18].tag != INST_MOV_REG_REG);
        EXIT_IF(insts[18].dst.reg != REG_RSP);
        EXIT_IF(insts[18].dst.line != 21);
        EXIT_IF(insts[18].src.reg != REG_RBP);
        EXIT_IF(insts[18].src.line != 21);
        EXIT_IF(insts[18].position != 69);
        EXIT_IF(insts[18].size != 3);

        EXIT_IF(insts[19].tag != INST_POP_REG);
        EXIT_IF(insts[19].dst.reg != REG_RBP);
        EXIT_IF(insts[19].dst.line != 22);
        EXIT_IF(insts[19].position != 72);
        EXIT_IF(insts[19].size != 1);

        EXIT_IF(insts[20].tag != INST_RET);
        EXIT_IF(insts[20].position != 73);
        EXIT_IF(insts[20].size != 1);

        EXIT_IF(memory->insts_index != 21);
    }
    {
        set_bytes(memory);
        EXIT_IF(memory->bytes_index != 74);
        Program program = transform(memory);
        EXIT_IF(-54 != (*((FnVoidI32*)&program.buffer))());
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

static void test_compile_4(Memory* memory) {
    reset(memory);
    {
        const char* source = "    sub     rsp, 8\n"
                             "    mov     [rsp], 1.1\n"
                             "    mov     eax, [rsp]\n"
                             "    add     rsp, 8\n"
                             "    ret\n";
        usize       size = strlen(source);
        memcpy(memory->file, source, size);
        memory->file_index = size;
    }
    {
        set_tokens(memory);
        Token* tokens = memory->tokens;

        EXIT_IF(tokens[0].tag != TOKEN_SUB);
        EXIT_IF(tokens[0].line != 1);

        EXIT_IF(tokens[1].tag != TOKEN_RSP);
        EXIT_IF(tokens[1].line != 1);

        EXIT_IF(tokens[2].tag != TOKEN_COMMA);
        EXIT_IF(tokens[2].line != 1);

        EXIT_IF(tokens[3].tag != TOKEN_I32);
        EXIT_IF(tokens[3].i32 != 8);
        EXIT_IF(tokens[3].line != 1);

        EXIT_IF(tokens[4].tag != TOKEN_MOV);
        EXIT_IF(tokens[4].line != 2);

        EXIT_IF(tokens[5].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[5].line != 2);

        EXIT_IF(tokens[6].tag != TOKEN_RSP);
        EXIT_IF(tokens[6].line != 2);

        EXIT_IF(tokens[7].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[7].line != 2);

        EXIT_IF(tokens[8].tag != TOKEN_COMMA);
        EXIT_IF(tokens[8].line != 2);

        EXIT_IF(tokens[9].tag != TOKEN_F32);
        EXIT_IF(tokens[9].i32 != 1066192077);
        EXIT_IF(tokens[9].line != 2);

        EXIT_IF(tokens[10].tag != TOKEN_MOV);
        EXIT_IF(tokens[10].line != 3);

        EXIT_IF(tokens[11].tag != TOKEN_EAX);
        EXIT_IF(tokens[11].line != 3);

        EXIT_IF(tokens[12].tag != TOKEN_COMMA);
        EXIT_IF(tokens[12].line != 3);

        EXIT_IF(tokens[13].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[13].line != 3);

        EXIT_IF(tokens[14].tag != TOKEN_RSP);
        EXIT_IF(tokens[14].line != 3);

        EXIT_IF(tokens[15].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[15].line != 3);

        EXIT_IF(tokens[16].tag != TOKEN_ADD);
        EXIT_IF(tokens[16].line != 4);

        EXIT_IF(tokens[17].tag != TOKEN_RSP);
        EXIT_IF(tokens[17].line != 4);

        EXIT_IF(tokens[18].tag != TOKEN_COMMA);
        EXIT_IF(tokens[18].line != 4);

        EXIT_IF(tokens[19].tag != TOKEN_I32);
        EXIT_IF(tokens[19].i32 != 8);
        EXIT_IF(tokens[19].line != 4);

        EXIT_IF(tokens[20].tag != TOKEN_RET);
        EXIT_IF(tokens[20].line != 5);

        EXIT_IF(memory->tokens_index != 21);
        EXIT_IF(memory->buffer_index != 0);
    }
    {
        set_insts(memory);
        EXIT_IF(memory->labels_index != 0);
        resolve_insts(memory);
        Inst* insts = memory->insts;

        EXIT_IF(insts[0].tag != INST_SUB_REG_IMM_I32);
        EXIT_IF(insts[0].dst.reg != REG_RSP);
        EXIT_IF(insts[0].dst.line != 1);
        EXIT_IF(insts[0].src.imm_i32 != 8);
        EXIT_IF(insts[0].src.line != 1);
        EXIT_IF(insts[0].position != 0);
        EXIT_IF(insts[0].size != 7);

        EXIT_IF(insts[1].tag != INST_MOV_ADDR_OFFSET_IMM_I32);
        EXIT_IF(insts[1].dst.reg != REG_RSP);
        EXIT_IF(insts[1].dst.addr_offset != 0);
        EXIT_IF(insts[1].dst.line != 2);
        EXIT_IF(insts[1].src.imm_i32 != 1066192077);
        EXIT_IF(insts[1].src.line != 2);
        EXIT_IF(insts[1].position != 7);
        EXIT_IF(insts[1].size != 11);

        EXIT_IF(insts[2].tag != INST_MOV_REG_ADDR_OFFSET);
        EXIT_IF(insts[2].dst.reg != REG_EAX);
        EXIT_IF(insts[2].dst.line != 3);
        EXIT_IF(insts[2].src.reg != REG_RSP);
        EXIT_IF(insts[2].src.addr_offset != 0);
        EXIT_IF(insts[2].src.line != 3);
        EXIT_IF(insts[2].position != 18);
        EXIT_IF(insts[2].size != 7);

        EXIT_IF(insts[3].tag != INST_ADD_REG_IMM_I32);
        EXIT_IF(insts[3].dst.reg != REG_RSP);
        EXIT_IF(insts[3].dst.line != 4);
        EXIT_IF(insts[3].src.imm_i32 != 8);
        EXIT_IF(insts[3].src.line != 4);
        EXIT_IF(insts[3].position != 25);
        EXIT_IF(insts[3].size != 7);

        EXIT_IF(insts[4].tag != INST_RET);
        EXIT_IF(insts[4].position != 32);
        EXIT_IF(insts[4].size != 1);

        EXIT_IF(memory->insts_index != 5);
    }
    {
        set_bytes(memory);
        EXIT_IF(memory->bytes_index != 33);
        Program program = transform(memory);
        EXIT_IF(1066192077 != (*((FnVoidI32*)&program.buffer))());
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

static void test_compile_5(Memory* memory) {
    reset(memory);
    {
        const char* source = "    sub     rsp, 8\n"
                             "    mov     [rsp + 4], 1.0 ; => `[rbp - 4]`\n"
                             "    mov     [rsp], -1.5    ; => `[rbp - 8]`\n"
                             "    xorps   xmm0, xmm0\n"
                             "    movss   xmm0, [rsp + 4]\n"
                             "    movss   xmm1, [rsp]\n"
                             "    addss   xmm0, xmm1\n"
                             "    addss   xmm0, [rsp]\n"
                             "    addss   xmm0, [rsp + 4]\n"
                             "    add     rsp, 8\n"
                             "    ret\n";
        usize       size = strlen(source);
        memcpy(memory->file, source, size);
        memory->file_index = size;
    }
    {
        set_tokens(memory);
        Token* tokens = memory->tokens;

        EXIT_IF(tokens[0].tag != TOKEN_SUB);
        EXIT_IF(tokens[0].line != 1);

        EXIT_IF(tokens[1].tag != TOKEN_RSP);
        EXIT_IF(tokens[1].line != 1);

        EXIT_IF(tokens[2].tag != TOKEN_COMMA);
        EXIT_IF(tokens[2].line != 1);

        EXIT_IF(tokens[3].tag != TOKEN_I32);
        EXIT_IF(tokens[3].i32 != 8);
        EXIT_IF(tokens[3].line != 1);

        EXIT_IF(tokens[4].tag != TOKEN_MOV);
        EXIT_IF(tokens[4].line != 2);

        EXIT_IF(tokens[5].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[5].line != 2);

        EXIT_IF(tokens[6].tag != TOKEN_RSP);
        EXIT_IF(tokens[6].line != 2);

        EXIT_IF(tokens[7].tag != TOKEN_PLUS);
        EXIT_IF(tokens[7].line != 2);

        EXIT_IF(tokens[8].tag != TOKEN_I32);
        EXIT_IF(tokens[8].i32 != 4);
        EXIT_IF(tokens[8].line != 2);

        EXIT_IF(tokens[9].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[9].line != 2);

        EXIT_IF(tokens[10].tag != TOKEN_COMMA);
        EXIT_IF(tokens[10].line != 2);

        EXIT_IF(tokens[11].tag != TOKEN_F32);
        EXIT_IF(tokens[11].i32 != 1065353216);
        EXIT_IF(tokens[11].line != 2);

        EXIT_IF(tokens[12].tag != TOKEN_MOV);
        EXIT_IF(tokens[12].line != 3);

        EXIT_IF(tokens[13].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[13].line != 3);

        EXIT_IF(tokens[14].tag != TOKEN_RSP);
        EXIT_IF(tokens[14].line != 3);

        EXIT_IF(tokens[15].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[15].line != 3);

        EXIT_IF(tokens[16].tag != TOKEN_COMMA);
        EXIT_IF(tokens[16].line != 3);

        EXIT_IF(tokens[17].tag != TOKEN_F32);
        EXIT_IF(tokens[17].i32 != -1077936128);
        EXIT_IF(tokens[17].line != 3);

        EXIT_IF(tokens[18].tag != TOKEN_XORPS);
        EXIT_IF(tokens[18].line != 4);

        EXIT_IF(tokens[19].tag != TOKEN_XMM0);
        EXIT_IF(tokens[19].line != 4);

        EXIT_IF(tokens[20].tag != TOKEN_COMMA);
        EXIT_IF(tokens[20].line != 4);

        EXIT_IF(tokens[21].tag != TOKEN_XMM0);
        EXIT_IF(tokens[21].line != 4);

        EXIT_IF(tokens[22].tag != TOKEN_MOVSS);
        EXIT_IF(tokens[22].line != 5);

        EXIT_IF(tokens[23].tag != TOKEN_XMM0);
        EXIT_IF(tokens[23].line != 5);

        EXIT_IF(tokens[24].tag != TOKEN_COMMA);
        EXIT_IF(tokens[24].line != 5);

        EXIT_IF(tokens[25].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[25].line != 5);

        EXIT_IF(tokens[26].tag != TOKEN_RSP);
        EXIT_IF(tokens[26].line != 5);

        EXIT_IF(tokens[27].tag != TOKEN_PLUS);
        EXIT_IF(tokens[27].line != 5);

        EXIT_IF(tokens[28].tag != TOKEN_I32);
        EXIT_IF(tokens[28].i32 != 4);
        EXIT_IF(tokens[28].line != 5);

        EXIT_IF(tokens[29].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[29].line != 5);

        EXIT_IF(tokens[30].tag != TOKEN_MOVSS);
        EXIT_IF(tokens[30].line != 6);

        EXIT_IF(tokens[31].tag != TOKEN_XMM1);
        EXIT_IF(tokens[31].line != 6);

        EXIT_IF(tokens[32].tag != TOKEN_COMMA);
        EXIT_IF(tokens[32].line != 6);

        EXIT_IF(tokens[33].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[33].line != 6);

        EXIT_IF(tokens[34].tag != TOKEN_RSP);
        EXIT_IF(tokens[34].line != 6);

        EXIT_IF(tokens[35].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[35].line != 6);

        EXIT_IF(tokens[36].tag != TOKEN_ADDSS);
        EXIT_IF(tokens[36].line != 7);

        EXIT_IF(tokens[37].tag != TOKEN_XMM0);
        EXIT_IF(tokens[37].line != 7);

        EXIT_IF(tokens[38].tag != TOKEN_COMMA);
        EXIT_IF(tokens[38].line != 7);

        EXIT_IF(tokens[39].tag != TOKEN_XMM1);
        EXIT_IF(tokens[39].line != 7);

        EXIT_IF(tokens[40].tag != TOKEN_ADDSS);
        EXIT_IF(tokens[40].line != 8);

        EXIT_IF(tokens[41].tag != TOKEN_XMM0);
        EXIT_IF(tokens[41].line != 8);

        EXIT_IF(tokens[42].tag != TOKEN_COMMA);
        EXIT_IF(tokens[42].line != 8);

        EXIT_IF(tokens[43].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[43].line != 8);

        EXIT_IF(tokens[44].tag != TOKEN_RSP);
        EXIT_IF(tokens[44].line != 8);

        EXIT_IF(tokens[45].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[45].line != 8);

        EXIT_IF(tokens[46].tag != TOKEN_ADDSS);
        EXIT_IF(tokens[46].line != 9);

        EXIT_IF(tokens[47].tag != TOKEN_XMM0);
        EXIT_IF(tokens[47].line != 9);

        EXIT_IF(tokens[48].tag != TOKEN_COMMA);
        EXIT_IF(tokens[48].line != 9);

        EXIT_IF(tokens[49].tag != TOKEN_LBRACKET);
        EXIT_IF(tokens[49].line != 9);

        EXIT_IF(tokens[50].tag != TOKEN_RSP);
        EXIT_IF(tokens[50].line != 9);

        EXIT_IF(tokens[51].tag != TOKEN_PLUS);
        EXIT_IF(tokens[51].line != 9);

        EXIT_IF(tokens[52].tag != TOKEN_I32);
        EXIT_IF(tokens[52].i32 != 4);
        EXIT_IF(tokens[52].line != 9);

        EXIT_IF(tokens[53].tag != TOKEN_RBRACKET);
        EXIT_IF(tokens[53].line != 9);

        EXIT_IF(tokens[54].tag != TOKEN_ADD);
        EXIT_IF(tokens[54].line != 10);

        EXIT_IF(tokens[55].tag != TOKEN_RSP);
        EXIT_IF(tokens[55].line != 10);

        EXIT_IF(tokens[56].tag != TOKEN_COMMA);
        EXIT_IF(tokens[56].line != 10);

        EXIT_IF(tokens[57].tag != TOKEN_I32);
        EXIT_IF(tokens[57].i32 != 8);
        EXIT_IF(tokens[57].line != 10);

        EXIT_IF(tokens[58].tag != TOKEN_RET);
        EXIT_IF(tokens[58].line != 11);

        EXIT_IF(memory->tokens_index != 59);
        EXIT_IF(memory->buffer_index != 0);
    }
    {
        set_insts(memory);
        EXIT_IF(memory->labels_index != 0);
        resolve_insts(memory);
        Inst* insts = memory->insts;

        EXIT_IF(insts[0].tag != INST_SUB_REG_IMM_I32);
        EXIT_IF(insts[0].dst.reg != REG_RSP);
        EXIT_IF(insts[0].dst.line != 1);
        EXIT_IF(insts[0].src.imm_i32 != 8);
        EXIT_IF(insts[0].src.line != 1);
        EXIT_IF(insts[0].position != 0);
        EXIT_IF(insts[0].size != 7);

        EXIT_IF(insts[1].tag != INST_MOV_ADDR_OFFSET_IMM_I32);
        EXIT_IF(insts[1].dst.reg != REG_RSP);
        EXIT_IF(insts[1].dst.addr_offset != 4);
        EXIT_IF(insts[1].dst.line != 2);
        EXIT_IF(insts[1].src.imm_i32 != 1065353216);
        EXIT_IF(insts[1].src.line != 2);
        EXIT_IF(insts[1].position != 7);
        EXIT_IF(insts[1].size != 11);

        EXIT_IF(insts[2].tag != INST_MOV_ADDR_OFFSET_IMM_I32);
        EXIT_IF(insts[2].dst.reg != REG_RSP);
        EXIT_IF(insts[2].dst.addr_offset != 0);
        EXIT_IF(insts[2].dst.line != 3);
        EXIT_IF(insts[2].src.imm_i32 != -1077936128);
        EXIT_IF(insts[2].src.line != 3);
        EXIT_IF(insts[2].position != 18);
        EXIT_IF(insts[2].size != 11);

        EXIT_IF(insts[3].tag != INST_XORPS_REG_REG);
        EXIT_IF(insts[3].dst.reg != REG_XMM0);
        EXIT_IF(insts[3].dst.line != 4);
        EXIT_IF(insts[3].src.reg != REG_XMM0);
        EXIT_IF(insts[3].src.line != 4);
        EXIT_IF(insts[3].position != 29);
        EXIT_IF(insts[3].size != 3);

        EXIT_IF(insts[4].tag != INST_MOVSS_REG_ADDR_OFFSET);
        EXIT_IF(insts[4].dst.reg != REG_XMM0);
        EXIT_IF(insts[4].dst.line != 5);
        EXIT_IF(insts[4].src.reg != REG_RSP);
        EXIT_IF(insts[4].src.addr_offset != 4);
        EXIT_IF(insts[4].src.line != 5);
        EXIT_IF(insts[4].position != 32);
        EXIT_IF(insts[4].size != 9);

        EXIT_IF(insts[5].tag != INST_MOVSS_REG_ADDR_OFFSET);
        EXIT_IF(insts[5].dst.reg != REG_XMM1);
        EXIT_IF(insts[5].dst.line != 6);
        EXIT_IF(insts[5].src.reg != REG_RSP);
        EXIT_IF(insts[5].src.addr_offset != 0);
        EXIT_IF(insts[5].src.line != 6);
        EXIT_IF(insts[5].position != 41);
        EXIT_IF(insts[5].size != 9);

        EXIT_IF(insts[6].tag != INST_ADDSS_REG_REG);
        EXIT_IF(insts[6].dst.reg != REG_XMM0);
        EXIT_IF(insts[6].dst.line != 7);
        EXIT_IF(insts[6].src.reg != REG_XMM1);
        EXIT_IF(insts[6].src.line != 7);
        EXIT_IF(insts[6].position != 50);
        EXIT_IF(insts[6].size != 4);

        EXIT_IF(insts[7].tag != INST_ADDSS_REG_ADDR_OFFSET);
        EXIT_IF(insts[7].dst.reg != REG_XMM0);
        EXIT_IF(insts[7].dst.line != 8);
        EXIT_IF(insts[7].src.reg != REG_RSP);
        EXIT_IF(insts[7].src.addr_offset != 0);
        EXIT_IF(insts[7].src.line != 8);
        EXIT_IF(insts[7].position != 54);
        EXIT_IF(insts[7].size != 9);

        EXIT_IF(insts[8].tag != INST_ADDSS_REG_ADDR_OFFSET);
        EXIT_IF(insts[8].dst.reg != REG_XMM0);
        EXIT_IF(insts[8].dst.line != 9);
        EXIT_IF(insts[8].src.reg != REG_RSP);
        EXIT_IF(insts[8].src.addr_offset != 4);
        EXIT_IF(insts[8].src.line != 9);
        EXIT_IF(insts[8].position != 63);
        EXIT_IF(insts[8].size != 9);

        EXIT_IF(insts[9].tag != INST_ADD_REG_IMM_I32);
        EXIT_IF(insts[9].dst.reg != REG_RSP);
        EXIT_IF(insts[9].dst.line != 10);
        EXIT_IF(insts[9].src.imm_i32 != 8);
        EXIT_IF(insts[9].src.line != 10);
        EXIT_IF(insts[9].position != 72);
        EXIT_IF(insts[9].size != 7);

        EXIT_IF(insts[10].tag != INST_RET);
        EXIT_IF(insts[10].position != 79);
        EXIT_IF(insts[10].size != 1);

        EXIT_IF(memory->insts_index != 11);
    }
    {
        set_bytes(memory);
        EXIT_IF(memory->bytes_index != 80);
        Program program = transform(memory);
        f32     x = (*((FnVoidF32*)&program.buffer))();
        EXIT_IF((x < (-1.0f - EPSILON)) || ((-1.0f + EPSILON) < x));
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

static void test_emit_transform(Memory* memory) {
    reset(memory);
    i32 x = 42;
    {
        /*     - 0-     mov     edi, `x`
         *     - 5-     call    label
         *     -10-     ret
         *     -11-     mov     eax, `-(x + 1)` ; NOTE: Never evaluated!
         */
        emit_mov_edi_imm_i32(memory);  //  0 + 1 ->  1
        emit_i32(memory, x);           //  1 + 4 ->  5
        emit_call_rel_imm_i32(memory); //  5 + 1 ->  6
        emit_i32(memory, 6);           //  6 + 4 -> 10
        emit_ret(memory);              // 10 + 1 -> 11
        emit_mov_eax_imm_i32(memory);  // 11 + 1 -> 12
        emit_i32(memory, -(x + 1));    // 12 + 4 -> 16
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
        EXIT_IF(x != (*((FnVoidI32*)&program.buffer))());
        EXIT_IF(munmap(program.buffer, memory->bytes_index));
    }
    reset(memory);
    PRINT_FN_OK();
}

i32 main(i32 n, const char** args) {
    Memory* memory = malloc(sizeof(Memory));
    EXIT_IF(!memory);
    printf("sizeof(Memory)         : %zu\n"
           "sizeof(memory->file)   : %zu\n"
           "sizeof(memory->tokens) : %zu\n"
           "sizeof(memory->buffer) : %zu\n"
           "sizeof(memory->insts)  : %zu\n"
           "sizeof(memory->labels) : %zu\n"
           "sizeof(memory->bytes)  : %zu\n"
           "sizeof(Token)          : %zu\n"
           "sizeof(Arg)            : %zu\n"
           "sizeof(Inst)           : %zu\n"
           "sizeof(Label)          : %zu\n"
           "sizeof(Program)        : %zu\n"
           "\n",
           sizeof(Memory),
           sizeof(memory->file),
           sizeof(memory->tokens),
           sizeof(memory->buffer),
           sizeof(memory->insts),
           sizeof(memory->labels),
           sizeof(memory->bytes),
           sizeof(Token),
           sizeof(Arg),
           sizeof(Inst),
           sizeof(Label),
           sizeof(Program));
    {
        test_compile_1(memory);
        test_compile_2(memory);
        test_compile_3(memory);
        test_compile_4(memory);
        test_compile_5(memory);
        test_emit_transform(memory);
    }
    EXIT_IF(n < 2);
    {
        set_file(memory, args[1]);
        printf("\n%s\n", memory->file);
        set_tokens(memory);
        set_insts(memory);
        resolve_insts(memory);
        set_bytes(memory);
        {
            Program program = transform(memory);
            printf("%f\n", (f64)(*((FnVoidF32*)&program.buffer))());
            EXIT_IF(munmap(program.buffer, memory->bytes_index));
        }
    }
    free(memory);
    return EXIT_SUCCESS;
}
