	INCLUDE s3c2440_h.S

MAX_SWI_NUM		EQU		6	

	EXTERN 	swi_table
	AREA SWI, CODE, READONLY
	EXPORT HandleSWI
HandleSWI
	ldr	sp, =(SVC_STACK_BASE+VA_TO_PA_OFT)
	stmdb	sp!, {r0-r12, lr}	; ����ʹ�õ��ļĴ����ͷ��ص�ַ
	ldr	r4, [lr, #-4]			; lr-4Ϊָ��"swi n"�ĵ�ַ,��ָ���24λ����n
	bic	r4, r4, #0xff000000	
	ldr r6, =MAX_SWI_NUM
	cmp	r4, r6
	ldrls	r5, =swi_table		; swi��ת����ַ
	ldrls	lr, =swi_return		; ���ص�ַ
	ldrls	pc, [r5,r4,lsl #2]	; ��ת����Ӧ��ദ������
	mov	r0, #-1					; ����������-1
swi_return		
	ldmia	sp!,{ r0-r12,pc }^	; �жϷ���, ^��ʾ��spsr��ֵ���Ƶ�cpsr

	; �����˳���������,���紦��main�����˳�

	EXPORT sys_exit
	IMPORT do_exit
sys_exit
	; ������C����do_exit(int error_code)
	; r0=error_code
	ldr	pc, =do_exit		; do_exit���ص�swi_return


	EXPORT sys_sleep		
	IMPORT do_sleep
sys_sleep		
	; ������C����int sleep(int time)
	; r0=sleep time
	ldr	pc, =do_sleep		; do_sleep���ص�swi_return

	EXPORT sys_write		
	IMPORT do_write	
sys_write		
	; ������C����int write(char *str)
	; r0=�ַ����׵�ַ
	ldr	pc, =do_write		; do_write���ص�swi_return	
	
	EXPORT sys_led_on		
	IMPORT do_led_on	
sys_led_on		
	; ������C����void led_on(int led_no)
	; r0=led_no
	ldr	pc, =do_led_on		; do_led_on���ص�swi_return	
	
	EXPORT sys_all_led_off		
	IMPORT do_all_led_off
sys_all_led_off		
	; ������C����void all_led_off(void)
	ldr	pc, =do_all_led_off	; do_all_led_off���ص�swi_return
	
	EXPORT sys_all_led_on		
	IMPORT do_all_led_on	
sys_all_led_on		
	; ������C����void all_led_on(void)
	ldr	pc, =do_all_led_on	; do_all_led_on���ص�swi_return

	END