bits 32
extern kmain
extern keyboard_handler_main
extern uart_handler_main
extern trapent

global _start
global keyboard_handler
global uart_handler
global syscall_handler

global stack_top
global stack_bottom

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
	resb 4096
	stack_bottom:
	resb 8192
	stack_top:
	pgdir:
	resb 4096
	pgtab:
	resb 4096
	;;self_tab is last page table, poining to kernel page tables
	pgtab_self:
	resb 4096

;	mem_ptr dd 0


PG_TAB_NEXT_VAL equ (1 << 12)
section .multiboot.text
	_start:
		cli ; disable interrupts
		mov edx, ebx

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

		;setup pgdir self_tab entry
		;map last entry
		mov eax, (pgtab_self - PG_KERN)
		mov ebx, (pgdir - PG_KERN)
		mov [ebx + 1023*4], eax
		or [ebx + 1023*4], dword 0x3
		;setup table
		mov ebx, (pgtab - PG_KERN)
		mov [eax], ebx
		or [eax], dword 0x3
		mov [eax+PG_KERN_POS*4], ebx
		or [eax+PG_KERN_POS*4], dword 0x3
		mov ebx, (pgtab_self - PG_KERN)
		mov [eax+1023*4], ebx
		or [eax+1023*4], dword 0x3
		

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

	syscall_handler:
		push 0x01
		push 0x80
		jmp trapent
		hlt;should return here



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
	gdt_ucode:
	dw 0xffff
	dw 0
	db 0
	db 0xfa
	db 11001111b
	db 0
	gdt_udata:
	dw 0xffff
	dw 0
	db 0
	db 0xf2
	db 11001111b
	db 0
global gdt_tss
	gdt_tss:
	dq 0 

gdtdesc:
	dw (gdtdesc - gdt - 1)
	dd gdt
