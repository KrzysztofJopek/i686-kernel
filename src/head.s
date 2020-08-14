bits 32
extern kmain
extern keyboard_handler_main
extern uart_handler_main

global _start
global keyboard_handler
global uart_handler

MB_MAGIC equ 0x1BADB002
MB_FLAGS equ (1<<0)|(1<<1)
MB_CHECKSUM equ (0 - (MB_MAGIC + MB_FLAGS))

section .multiboot.data
        ALIGN 4
        dd MB_MAGIC
        dd MB_FLAGS
        dd MB_CHECKSUM

PG_KERN 	equ 0xC0000000
PG_KERN_POS 	equ (PG_KERN >> 22)
section .bss
	align 4096
	stack_bottom:
	resb 8192
	stack_top:
	pgdir:
	resb 4096
	pgtab:
	resb 4096

;	mem_ptr dd 0


PG_TAB_NEXT_VAL equ (1 << 12)
section .multiboot.text
	_start:
		cli ; disable interrupts
		mov edx, ebx

		;clear segments
		xor ax, ax
		mov ds, ax
		mov es, ax

		;setup pgdir
		mov eax, (pgtab - PG_KERN)
		mov ebx, (pgdir - PG_KERN)
		;map entry for 0xC00
		mov [ebx + PG_KERN_POS*4], eax
		or  [ebx + PG_KERN_POS*4], dword 0x3
		;map entry for 0x0
		mov [ebx], eax
		or  [ebx], dword 0x3

		;setup page table
		mov ecx, eax
		add ecx, dword 4096
		mov ebx, dword 0x3
		next_page:
		mov [eax], ebx
		add ebx, PG_TAB_NEXT_VAL
		add eax, dword 0x4
		cmp eax, ecx
		jl next_page

		;enable paging
		mov eax, (pgdir - PG_KERN)
		mov cr3, eax
		mov eax, cr0
		or eax, dword 0x80000001
		mov cr0, eax
		nop
		lea ecx, call_main
		jmp ecx

section .text
		call_main:
		call set_gdt

		mov esp, stack_top
		mov ebx, edx
		push ebx
		call kmain
		hlt

		;enable a20
		;not used -- multiboot
	enable_a20:
		set_20.1:
		in al, 0x64
		test al, 0x2
		jnz set_20.1
		mov al, 0xd1
		out 0x64, al

		set_20.2:
		in al, 0x64
		test al, 0x2
		jnz set_20.2
		mov al, 0xdf
		out 0x60, al

	set_gdt:
		lgdt [gdtdesc]
		mov eax, cr0
		or eax, 1
		mov cr0, eax

		jmp 0x8:en32

		en32:
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov ss, ax

		xor ax, ax
		mov fs, ax
		mov gs, ax
		ret

	keyboard_handler:
		call    keyboard_handler_main
		iretd

	uart_handler:
		call    uart_handler_main
		iretd



gdt:
	gdt_null:
	dq 0
	gdt_code:
	dw 0xffff
	dw 0
	db 0
	db 0x9a
	db 11001111b
	db 0
	gdt_data:
	dw 0xffff
	dw 0
	db 0
	db 0x92
	db 11001111b
	db 0

gdtdesc:
	dw (gdtdesc - gdt - 1)
	dd gdt
