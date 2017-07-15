	INCLUDE s3c2440_h.S

MAX_SWI_NUM		EQU		6	

	EXTERN 	swi_table
	AREA SWI, CODE, READONLY
	EXPORT HandleSWI
HandleSWI
	ldr	sp, =(SVC_STACK_BASE+VA_TO_PA_OFT)
	stmdb	sp!, {r0-r12, lr}	; 保存使用到的寄存器和返回地址
	ldr	r4, [lr, #-4]			; lr-4为指令"swi n"的地址,此指令低24位就是n
	bic	r4, r4, #0xff000000	
	ldr r6, =MAX_SWI_NUM
	cmp	r4, r6
	ldrls	r5, =swi_table		; swi跳转表基址
	ldrls	lr, =swi_return		; 返回地址
	ldrls	pc, [r5,r4,lsl #2]	; 跳转到相应汇编处理函数
	mov	r0, #-1					; 出错，返回-1
swi_return		
	ldmia	sp!,{ r0-r12,pc }^	; 中断返回, ^表示将spsr的值复制到cpsr

	; 进程退出处理函数,比如处理main函数退出

	EXPORT sys_exit
	IMPORT do_exit
sys_exit
	; 将调用C函数do_exit(int error_code)
	; r0=error_code
	ldr	pc, =do_exit		; do_exit返回到swi_return


	EXPORT sys_sleep		
	IMPORT do_sleep
sys_sleep		
	; 将调用C函数int sleep(int time)
	; r0=sleep time
	ldr	pc, =do_sleep		; do_sleep返回到swi_return

	EXPORT sys_write		
	IMPORT do_write	
sys_write		
	; 将调用C函数int write(char *str)
	; r0=字符串首地址
	ldr	pc, =do_write		; do_write返回到swi_return	
	
	EXPORT sys_led_on		
	IMPORT do_led_on	
sys_led_on		
	; 将调用C函数void led_on(int led_no)
	; r0=led_no
	ldr	pc, =do_led_on		; do_led_on返回到swi_return	
	
	EXPORT sys_all_led_off		
	IMPORT do_all_led_off
sys_all_led_off		
	; 将调用C函数void all_led_off(void)
	ldr	pc, =do_all_led_off	; do_all_led_off返回到swi_return
	
	EXPORT sys_all_led_on		
	IMPORT do_all_led_on	
sys_all_led_on		
	; 将调用C函数void all_led_on(void)
	ldr	pc, =do_all_led_on	; do_all_led_on返回到swi_return

	END