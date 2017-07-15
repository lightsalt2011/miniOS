;******************************************************************************
; File：crt0.s
; 功能：通过它转入C程序
;******************************************************************************       
	AREA    crt0, CODE, READONLY
	ENTRY 
	ldr	r0, [sp]		; main函数参数argc
	add	r1, sp, #4		; main函数参数argv
	IMPORT main
	bl	main			; 调用main函数
	IMPORT exit
	bl	exit			; 用户程序main结束后，调用exit,在lib.c中
halt_loop
	b	halt_loop
	END
