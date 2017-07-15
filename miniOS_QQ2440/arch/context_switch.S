	INCLUDE s3c2440_h.S

PID_OFT			EQU			0		; ����PID���ƫ��
STATE_OFT		EQU			4		; ����״̬���ƫ��
CONTENT_OFT		EQU			20		; ����ִ���ֳ����������Ĵ�����ֵ��
SPSR_OFT		EQU			CONTENT_OFT 			; CPSR���ƫ��
USER_SP_LR_OFT 	EQU			CONTENT_OFT + 1			; �û�ģʽSP,LRƫ��
R0_R13_OFT		EQU			USER_SP_LR_OFT + 1		; �û����üĴ���ƫ��
; �������쳣������Ҫ���쳣����֮ǰ����ִ���ֳ�������ջ�Ĵ�����ֵ���α�������
; Ϊ��ȡ���ڽ����쳣ʱ��ִ���ֳ��������Ӧģʽ��ջָ������ƫ��14���õ�R0-R13�Ĵ�����ֵ
IRQ_SAVE_BASE	EQU			(IRQ_STACK_BASE + VA_TO_PA_OFT - 14*4)	; �ж�ģʽ��R0-R13�Ĵ��������ַ
SVR_SAVE_BASE	EQU			(SVC_STACK_BASE + VA_TO_PA_OFT - 14*4)	; ����ģʽ��R0-R13�Ĵ��������ַ
			
	AREA SWITCH, CODE, READONLY
	EXPORT __switch_to
__switch_to				; __switch_to�п��ܴ�irq��svcģʽ����
	mrs	r2, cpsr
	bic	r2, r2,#0xffffffe0
	cmp	r2, #0x12		; �ж��Ƿ�Ϊirqģʽ
	
	; �����ж�ģʽ�µ�SPջָ��
	ldreq	sp, =IRQ_SAVE_BASE

	; ���ù���ģʽ�µ�SPջָ��
	ldrne	sp, =SVR_SAVE_BASE

	add	r0, r0, #CONTENT_OFT		; r0ָ��ǰ����PCB��ƫ�Ƶ�PCB�ṹ��content[16]��
	mrs	r2, spsr	
	stmia	r0!, {r2}				; ����SPSR��ֵ��PCB�ṹ��content[0]��
	stmia	r0!, {sp}^				; ���������û��ռ����SP��LR��content[1-2]
	stmia	r0!, {lr}^				; ���ڽ����ָ��û�SP, LR�Ĵ������쳣����ʱû�б���
									; ����Ϊ��ÿ��ģʽ�¶����Լ���SP,LR�����������ִ��
									; ÿ�������������û�ģʽ�£���ͬ���̵�SP,LR��һ����
									; ���Ҫ���棬��Ȼ�����û����̻Ḳ�ǵ���ǰ����SP,LR��
	; ���жϴ���ʱ����ļĴ���(R0~R13)ת�Ƶ���ǰPCB��
	ldmia	sp!, {r2-r8}			; ����SP�б����R0~R6
	stmia	r0!, {r2-r8}			; ����r0ָ���PCB�ṹ����content[3~9]
	ldmia	sp!, {r2-r8}			; ����SP�б����R7~R13
	stmia	r0!, {r2-r8}			; ����r0ָ���PCB�ṹ����content[10~16]
	
	; ����ж�
	mov	r0, #SRCPND_VA			; ʹ�������ַ
	mov	r2, #0x400
	str	r2, [r0]
	add	r0, r0, #0x10
	ldr	r2, [r0]
	str	r2, [r0]

do_switch
	ldr	r0, [r1, #PID_OFT]		; Ҫ�л����Ľ��̵�PID
	mov	r0, r0, lsl #25			; PID����25λ�����ڼĴ��������7λ	
	mcr	p15, 0, r0, c13, c0, 0	; дnext_pid,�Ӵˣ�VA<32M��ȡַ���㹫ʽ�ͱ��ˣ��������ڵ�VA���Ǵ���32M��

	add	r1, r1, #CONTENT_OFT	; r1 = pnext->content

	ldmia	r1!, {r2-r4}		; CPSR ,SP, LR
	msr	spsr_cxsf, r2			; ǰ��������cpsr
	mrs	r2, cpsr
	; �л���ϵͳģʽ�£������û�ģʽ��SP,LR
	msr	cpsr_c, #0xdf
	mov	sp, r3
	mov	lr, r4
	; �л��ص�ǰģʽ
	msr	cpsr_cxsf, r2
	ldmia	r1, {r0-r12, pc}^	; ִ�н����л�
	END