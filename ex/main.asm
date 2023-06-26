    sub     rsp, 8
    mov     [rsp + 4], 1.0 ; => `[rbp - 4]`
    mov     [rsp], -1.5    ; => `[rbp - 8]`

    xorps   xmm0, xmm0

    movss   xmm0, [rsp + 4]
    movss   xmm1, [rsp]

    addss   xmm0, xmm1      ; => `-0.5`
    addss   xmm0, [rsp]     ; => `-2.0`
    addss   xmm0, [rsp + 4] ; => `-1.0`

    add     rsp, 8
    ret
