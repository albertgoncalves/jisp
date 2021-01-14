    call    main
    ret

f:
    push    rbp
    mov     rbp, rsp

    push    rbx

    mov     eax, [rbp + 20]
    mov     ebx, [rbp + 16]
    sub     eax, ebx

    pop     rbx

    mov     rsp, rbp
    pop     rbp
    ret

main:
    push    rbp
    mov     rbp, rsp

    sub     rsp, 8
    mov     [rsp + 4], 1.1 ; => `[rbp - 4]`
    mov     [rsp], -1.1    ; => `[rbp - 8]`
    call    f

    mov     rsp, rbp
    pop     rbp
    ret
