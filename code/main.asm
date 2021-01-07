    mov     edi, 100
    call    label
    ret
label:
    push    rbx
    mov     ebx, edi
    mov     eax, ebx
    add     eax, 25
    pop     rbx
    ret
