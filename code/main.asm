    mov     edi, 100
    call    label
    ret
label:
    push    rbx
    mov     ebx, edi
    mov     eax, ebx
    pop     rbx
    ret
