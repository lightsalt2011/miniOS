;
;  start.S����Ҫ��װ�쳣����������ʼ����Ҫ��Ӳ����
; �������Norflash�п�����SDRAM��
; Ȼ����ת��SDRAM��ȥִ�У�������main��������ʼminiOS�����ڶ��׶�
;
	INCLUDE s3c2440_h.S
	
; ����Ϊʱ����ؼĴ�����ַ
LOCKTIME			EQU     0x4c000000	
MPLLCON		    	EQU		0x4c000004	
CLKDIVN		    	EQU		0x4c000014

RUN_BASE			EQU     0x33ff0000	; OS�ڴ����е�ַ
MEM_REG_BASE		EQU     0x48000000
MEM_REG_END	   		EQU 	0x48000034
WTD_TIMER	   		EQU 	0x53000000
; �����쳣ջָ�룬MTOS���������е��쳣

	IMPORT HandleSWI			; �������������ļ�������������
	IMPORT CopyCode2Ram
	IMPORT xmain
	IMPORT handle_irq
	IMPORT undef_excp
	IMPORT prefetch_abt
	IMPORT data_abt
	
	AREA    Start, CODE, READONLY

	ENTRY							; ����ο�ʼ
    b   Reset						; �쳣�������������е�ַΪ0��pc�Զ���Ӳ������
    								; �õ�ַ��ָ��Ϊһ��תָ�����reset�쳣����
    b	HandleUndef 				; δ�����쳣������תָ�����_HandleUndef��
    b	HandleSWI					; �����ж��쳣������תָ�����_HandleSWI��
    b   HandlePrefetchAbort			; Ԥȡָ����ֹ�쳣������תָ�����_HandlePrefetchAbort��
    b   HandleDataAbort				; ������ֹ�쳣������תָ�����_HandleDataAbort��
HandleNotUsed
    b	HandleNotUsed				; δʹ���쳣������תָ�û�д���
    b 	HandleIRQ					; һ���ж��쳣������תָ�������Դ�ļ���
    								;HandleIRQ���Ŵ�
HandleFIQ
    b	HandleFIQ					; �����ж��쳣������תָ�û�д���


Reset 								; Reset�쳣��������
    bl	clock_init					; ����ʱ�ӳ�ʼ������
    bl	mem_init					; �����ڴ��ʼ������
    ldr	sp, =SVC_STACK_BASE			; ���ù���ģʽջָ��
    bl	disable_watchdog			; �رտ��Ź�
; ���뿽���������뿽�����ڴ���ȥ���У������Nandflash							
;�������У���RAM steppingstoneֻ��4k������������ȫ����							
;�룬�����Norflash��������Ӳ�����Ծ����������ٶȽ�����						
;��ˣ������뿽�����ڴ���ȥ����
copy_code 							; ���뿽����ʼ���� 
    mov r0, #0x0					; R0��Ϊ���ݿ�ʼ��ַ (ROM���ݱ�����0��ַ��ʼ��)
    ldr	r1, =|Image$$RO$$Base|		; R1�д��RO��������е�ַ��
									; ��ֵ�ɷ��ű���Image$$EXEC_RO$$Baseȡ��
	ldr r2, =|Image$$ZI$$Limit|
    								; ��ֵ�ɷ��ű���Image$$EXEC_ZI$$Limitȡ��
    sub r2, r2, r1					; R2 = R2 - R1���ó����������ݳ���
    ldr r1, =KERNEL_RUN_PA
    bl	CopyCode2Ram				; ��R0,R1,R2�����������ݸ�CopyCode2Ram����ִ�п���
    
    ldr	r0, =|Image$$ZI$$Base|
    ldr r1, =|Image$$ZI$$Limit|
    sub r0, r0, #VA_TO_PA_OFT
    sub r1, r1, #VA_TO_PA_OFT
	bl	clear_bss_region
	
	; mmu
	IMPORT __create_page_tables_early
	IMPORT __enable_mmu
	IMPORT __create_page_tables_post
	
	bl delay
	
	bl __create_page_tables_early
	
	ldr lr, =RUN_VM					; RUN_VM�����е�ַ��ƫ�Ƶ�ַ��0x80000000+OFT��
	ldr r0, =__enable_mmu			; r0Ҳ�����е�ַ��ƫ�Ƶ�ַ��0x80000000+__enable_mmu��
	sub r0, r0, #VA_TO_PA_OFT		; ��ΪMMU��û��ʹ�ܣ����Ի��������ڴ���ִ��,���Ҫ��VA_TO_PA_OFT
	mov pc, r0						; һ��������MMU�������ַ0x80000000��0x30000000������ʹ��
RUN_VM			
	bl __create_page_tables_post	; ����Ѿ�������MMU��Ҫ�ж�ԭ�ȵ�0X300000000��ַӳ�䣬���Ա�֤����������0x800000000��
	
    ; init all mode stack
    bl stack_init					; ����ջ��ʼ�����봦

    msr	cpsr_c,	#0x5f    			; ����ϵͳ�жϣ�����ϵͳģʽ
    ldr	lr, =halt_loop				; ���÷��ص�ַ
    ldr	pc, =xmain					; ����main����������OS��������
halt_loop 							; OS���ص�ַ����ʵ�����Զ�����ܱ�ִ�е�����ΪֻҪOS��
									; �������ͻ����е�����ĩ��
    b	halt_loop					; ��ѭ��

delay
	mov r0, #0x100000
	mov r1, #0
loop
	cmp r1, r0
	sub r0, r0, #1
	bne loop
	mov pc, lr
	

HandleIRQ 							; ϵͳ�жϴ���
    sub	lr, lr, #4					; �������ص�ַ
    ldr	sp, =(IRQ_STACK_BASE + VA_TO_PA_OFT) ; �����ж�ģʽ��ջָ��
    stmdb   sp!, {r0-r12,lr}  		; �����ֳ�
    ldr	lr, =int_return				; �����жϴ�������ķ��ص�ַ
    ldr	pc, =handle_irq				; �����жϴ�������
int_return 							; ���ص�ַ
    ldmia   sp!, {r0-r12,pc}^		; �ָ����жϴ���ֳ�

clock_init 							; ʱ�ӳ�ʼ�����룬��ϸ��ʱ�ӳ�ʼ���½�
    ; Set lock time
    ldr r0, =LOCKTIME
    ldr r1, =0x00ffffff
    str r1, [r0]

    ; Set clock divison
    ldr r0, =CLKDIVN
    mov r1, #0x05
    str r1, [r0]

    ; Set system clock to asynchronous mode 
    mrc    p15, 0, r1, c1, c0, 0
    orr    r1, r1, #0xc0000000
    mcr    p15, 0, r1, c1, c0, 0

    ldr r0, =MPLLCON
    ldr r1, =0x5c011				; MPLL is 400MHz
    str r1, [r0]
    mov pc, lr

mem_init 							; �ڴ��ʼ�����룬��ϸ���ڴ��ʼ���½�
    ldr r0, =MEM_REG_BASE
    ldr r1, =MEM_REG_END
    adr r2, memdata
mem_init_loop 
    ldr r3, [r2], #4
    str r3, [r0], #4
    teq r0, r1
    bne mem_init_loop
    mov pc,lr

memdata 
    DCD 0x22111110	;BWSCON
    DCD	0x00000700	;BANKCON0	
    DCD 0x00000700	;BANKCON1   
    DCD 0x00000700	;BANKCON2	
    DCD 0x00000700	;BANKCON3	
    DCD 0x00000700	;BANKCON4	
    DCD	0x00000700	;BANKCON5   
    DCD 0x00018005	;BANKCON6	
    DCD 0x00018005	;BANKCON7   
    DCD 0x008e04f4	;REFRESH	
    DCD 0x000000b1	;BANKSIZE   
    DCD 0x00000030	;MRSRB6 
    DCD 0x00000030	;MRSRB7

clear_bss_region
	mov r2, #0
clear_loop
	cmp r0, r1
	beq quit_loop
	str r2, [r0], #4
	b clear_loop
quit_loop
	mov pc, lr
	
disable_watchdog
	ldr r0, =WTD_TIMER
	mov r1, #0
	str r1, [r0]
	mov pc, lr
	
stack_init 							; ջָ���ʼ��
    ; undefine_stack				; δ�����쳣
    msr cpsr_c, #0xdb
    ldr	sp, =(UND_STACK_BASE + VA_TO_PA_OFT)
    ; abort_stack					; δ�����쳣ģʽ
    msr cpsr_c, #0xd7
    ldr	sp, =(ABT_STACK_BASE + VA_TO_PA_OFT)
    ; irq_stack						; �ж�ģʽ
    msr	cpsr_c,	#0xd2
    ldr	sp, =(IRQ_STACK_BASE + VA_TO_PA_OFT)

    ; sys_stack						; ϵͳģʽ
    msr	cpsr_c,	#0xdf	
    ldr	sp, =(SYS_STACK_BASE + VA_TO_PA_OFT)
    
    ; svr_stack						; �л��ع���ģʽ
    msr	cpsr_c,	#0xd3	
    mov pc, lr
    
HandleUndef 						; δ�����쳣����
    add lr, lr, #4					; �������ص�ַ
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; �����ֳ�
    mrs r0, spsr					; �����쳣ʱ����״̬�Ĵ���������ݱ�����ջ��
    stmdb   sp!, {r0}
    mov r0, sp						; �����쳣ʱ������ջָ�봫�ݸ��쳣�����������ڴ�ӡ�쳣�ֳ���Ϣ			
    ldr pc, =undef_excp
    b	halt_loop

HandlePrefetchAbort 				; δ�����쳣����
    sub lr, lr, #4					; �������ص�ַ
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; �����ֳ�
    mrs r0, spsr					; �����쳣ʱ����״̬�Ĵ���������ݱ�����ջ��
    stmdb   sp!, {r0}
    mov r0, sp						; �����쳣ʱ������ջָ�봫�ݸ��쳣�����������ڴ�ӡ�쳣�ֳ���Ϣ	
    ldr pc, =prefetch_abt
    b	halt_loop

HandleDataAbort 					; δ�����쳣����
    sub lr, lr, #8					; �������ص�ַ
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; �����ֳ�
    mrs r0, spsr					; �����쳣ʱ����״̬�Ĵ���������ݱ�����ջ��
    stmdb   sp!, {r0}
    mov r0, sp						; �����쳣ʱ������ջָ�봫�ݸ��쳣�����������ڴ�ӡ�쳣�ֳ���Ϣ	
    ldr pc, =data_abt
    b	halt_loop
    
	END