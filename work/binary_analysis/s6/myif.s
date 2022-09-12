global main

main:
    mov eax, 0x3
    cmp eax, 0x3
    jz equal
    jmp neq

equal:
    mov eax, 0x1
    jmp exit

neq:
    mov eax, 0x0

exit: