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

section .multiboot
        ALIGN 4
        dd MB_MAGIC
        dd MB_FLAGS
        dd MB_CHECKSUM

section .bss
	align 16
	stack_bottom:
	resb 8196
	stack_top:

section .data
	mem_ptr dd 0


section .text
	_start:
		cli ; disable interrupts
		mov [mem_ptr], ebx

		;clear segments
		xor ax, ax
		mov ds, ax
		mov es, ax

		call set_gdt

	
		mov esp, stack_top
		mov ebx, [mem_ptr]
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
