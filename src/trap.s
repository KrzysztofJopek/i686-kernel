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
	mov eax, 1
	int 0x80
	jmp init_start
init_end:
	nop
