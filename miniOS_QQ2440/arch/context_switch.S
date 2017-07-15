	INCLUDE s3c2440_h.S

PID_OFT			EQU			0		; 进程PID相对偏移
STATE_OFT		EQU			4		; 进程状态相对偏移
CONTENT_OFT		EQU			20		; 进程执行现场保存区（寄存器的值）
SPSR_OFT		EQU			CONTENT_OFT 			; CPSR相对偏移
USER_SP_LR_OFT 	EQU			CONTENT_OFT + 1			; 用户模式SP,LR偏移
R0_R13_OFT		EQU			USER_SP_LR_OFT + 1		; 用户能用寄存器偏移
; 由于在异常发生后，要求异常处理之前保存执行现场，其入栈寄存器的值依次保存起来
; 为了取得在进入异常时的执行现场，将其对应模式下栈指针向下偏移14，得到R0-R13寄存器的值
IRQ_SAVE_BASE	EQU			(IRQ_STACK_BASE + VA_TO_PA_OFT - 14*4)	; 中断模式下R0-R13寄存器保存地址
SVR_SAVE_BASE	EQU			(SVC_STACK_BASE + VA_TO_PA_OFT - 14*4)	; 管理模式下R0-R13寄存器保存地址
			
	AREA SWITCH, CODE, READONLY
	EXPORT __switch_to
__switch_to				; __switch_to有可能从irq和svc模式调用
	mrs	r2, cpsr
	bic	r2, r2,#0xffffffe0
	cmp	r2, #0x12		; 判断是否为irq模式
	
	; 设置中断模式下的SP栈指针
	ldreq	sp, =IRQ_SAVE_BASE

	; 设置管理模式下的SP栈指针
	ldrne	sp, =SVR_SAVE_BASE

	add	r0, r0, #CONTENT_OFT		; r0指向当前进程PCB，偏移到PCB结构体content[16]处
	mrs	r2, spsr	
	stmia	r0!, {r2}				; 保存SPSR中值到PCB结构体content[0]中
	stmia	r0!, {sp}^				; 保存挂起的用户空间程序SP，LR到content[1-2]
	stmia	r0!, {lr}^				; 用于将来恢复用户SP, LR寄存器（异常处理时没有保存
									; 是因为，每种模式下都有自己的SP,LR，而多进程在执行
									; 每个程序都运行在用户模式下，不同进程的SP,LR不一样，
									; 因此要保存，不然其它用户进程会覆盖掉当前进程SP,LR）
	; 把中断处理时保存的寄存器(R0~R13)转移到当前PCB中
	ldmia	sp!, {r2-r8}			; 加载SP中保存的R0~R6
	stmia	r0!, {r2-r8}			; 存入r0指向的PCB结构体中content[3~9]
	ldmia	sp!, {r2-r8}			; 加载SP中保存的R7~R13
	stmia	r0!, {r2-r8}			; 存入r0指向的PCB结构体中content[10~16]
	
	; 清除中断
	mov	r0, #SRCPND_VA			; 使用虚拟地址
	mov	r2, #0x400
	str	r2, [r0]
	add	r0, r0, #0x10
	ldr	r2, [r0]
	str	r2, [r0]

do_switch
	ldr	r0, [r1, #PID_OFT]		; 要切换到的进程的PID
	mov	r0, r0, lsl #25			; PID左移25位，存在寄存器的最高7位	
	mcr	p15, 0, r0, c13, c0, 0	; 写next_pid,从此，VA<32M的取址计算公式就变了，不过现在的VA总是大于32M的

	add	r1, r1, #CONTENT_OFT	; r1 = pnext->content

	ldmia	r1!, {r2-r4}		; CPSR ,SP, LR
	msr	spsr_cxsf, r2			; 前挂起程序的cpsr
	mrs	r2, cpsr
	; 切换到系统模式下，加载用户模式下SP,LR
	msr	cpsr_c, #0xdf
	mov	sp, r3
	mov	lr, r4
	; 切换回当前模式
	msr	cpsr_cxsf, r2
	ldmia	r1, {r0-r12, pc}^	; 执行进程切换
	END