;******************************************************************************
; File��crt0.s
; ���ܣ�ͨ����ת��C����
;******************************************************************************       
	AREA    crt0, CODE, READONLY
	ENTRY 
	ldr	r0, [sp]		; main��������argc
	add	r1, sp, #4		; main��������argv
	IMPORT main
	bl	main			; ����main����
	IMPORT exit
	bl	exit			; �û�����main�����󣬵���exit,��lib.c��
halt_loop
	b	halt_loop
	END
