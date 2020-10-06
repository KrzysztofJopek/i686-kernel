global trapent
global trapret

extern trap


trapent:
	push ds
	push es
	push fs
	push gs
	pushad

	mov ax, 0x10
	mov ds, ax
	mov es, ax

	push esp
	call trap
	add esp, dword 0x4

	trapret:
	popad
	pop gs
	pop fs
	pop es
	pop ds
	add esp, dword 0x8
	iret


global init_start
global init_end
init_start:
	;open
	mov eax, 0
	mov ebx, path
	int 0x80
	mov ebx, eax

	;write
	init_loop:
	mov eax, 1
	mov ecx, buff
	mov edx, 7
	int 0x80
	mov eax, 0
	busy_loop:
	inc eax
	cmp eax, 100000000
	jne busy_loop
	jmp init_loop
	buff: db "Hello", 10, 0
	path: db "C1", 0
init_end:
