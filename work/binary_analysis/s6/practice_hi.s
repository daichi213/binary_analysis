global main

main:
	push 0x6F6C6C6548  
	mov eax, 0x4
	mov ebx, 0x1
	mov ecx, esp
	mov edx, 0x4
	int 0x80
