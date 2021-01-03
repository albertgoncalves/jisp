    mov     edi, 42
    call    label
    ret
    mov     eax, 43 ; NOTE: Never evaluated!
label:
    push    rbx
    mov     ebx, edi
    mov     eax, ebx
    pop     rbx
    ret
