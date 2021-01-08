    mov     edi, 1
    call    main
    ret
f:
    push    rbx
    mov     ebx, edi
    mov     eax, ebx
    add     eax, -87
    pop     rbx
    ret
main:
    mov     edi, 10
    call    f
    ret
