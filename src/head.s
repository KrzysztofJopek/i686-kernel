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

section .text
	_start:
		cli
		mov esp, stack_top
		call kmain
		hlt

	keyboard_handler:
		call    keyboard_handler_main
		iretd

	uart_handler:
		call    uart_handler_main
		iretd

