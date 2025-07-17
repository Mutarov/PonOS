	;       Multiboot header
	section .multiboot
	align   4
	dd      0x1BADB002; MAGIC
	dd      0x03; FLAGS
	dd      -(0x1BADB002 + 0x03); CHECKSUM

	section .text
	global  start
	global  stack_top
	global  start
	extern  kernel_main

start:
	;   Установка стека
	mov esp, stack_top

	;    Вызов основной функции на C
	call kernel_main

	; Бесконечный цикл (на случай возврата)
	cli

.hang:
	hlt
	jmp .hang

section .bss
align   16

stack_bottom:
	resb 16384; 16 КБ стека

stack_top:
