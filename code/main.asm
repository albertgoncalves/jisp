    mov     edi, 1
    call    main
    ret
f:
    push    rbx
    mov     [rsp - 4], edi
    mov     ebx, [rsp - 4]
    mov     eax, ebx
    add     eax, -87
    pop     rbx
    ret
main:
    mov     edi, 10
    call    f
    ret
