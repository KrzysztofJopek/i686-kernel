global swtch
global setup_ctx
;void swtch(struct context** old, struct context* new);
swtch:
	mov eax, [esp+4]
	mov edx, [esp+8]

	push ebp
	push ebx
	push esi
	push edi

	mov [eax], esp
	mov esp, edx

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void swtch2(struct context* old, struct context* new);
setup_ctx:
	mov eax, [esp+4]

	mov [eax], edi
	mov [eax+4], esi
	mov [eax+8], ebx
	mov [eax+12], ebp
	ret
