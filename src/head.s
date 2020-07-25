bits 32

section .text
        align 4
        dd 0x1BADB002
        dd 0x00
        dd - (0x1BADB002 + 0x00)

global _start
global keyboard_handler
global uart_handler

extern kmain
extern keyboard_handler_main
extern uart_handler_main

_start:
	cli
	mov esp, stack
	call kmain
	hlt

keyboard_handler:
	call    keyboard_handler_main
	iretd

uart_handler:
	call    uart_handler_main
	iretd

section .bss
resb 8192
stack:
