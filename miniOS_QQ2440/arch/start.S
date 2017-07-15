;
;  start.S：主要安装异常向量表，初始化必要的硬件，
; 将代码从Norflash中拷贝到SDRAM，
; 然后跳转到SDRAM中去执行，最后调用main函数，开始miniOS启动第二阶段
;
	INCLUDE s3c2440_h.S
	
; 以下为时钟相关寄存器地址
LOCKTIME			EQU     0x4c000000	
MPLLCON		    	EQU		0x4c000004	
CLKDIVN		    	EQU		0x4c000014

RUN_BASE			EQU     0x33ff0000	; OS内存运行地址
MEM_REG_BASE		EQU     0x48000000
MEM_REG_END	   		EQU 	0x48000034
WTD_TIMER	   		EQU 	0x53000000
; 其它异常栈指针，MTOS处理了所有的异常

	IMPORT HandleSWI			; 以下引入其它文件中声明函数名
	IMPORT CopyCode2Ram
	IMPORT xmain
	IMPORT handle_irq
	IMPORT undef_excp
	IMPORT prefetch_abt
	IMPORT data_abt
	
	AREA    Start, CODE, READONLY

	ENTRY							; 代码段开始
    b   Reset						; 异常向量表，其运行地址为0，pc自动由硬件设置
    								; 该地址处指令为一跳转指令，跳往reset异常处理
    b	HandleUndef 				; 未定义异常处理跳转指令，跳往_HandleUndef处
    b	HandleSWI					; 软件中断异常处理跳转指令，跳往_HandleSWI处
    b   HandlePrefetchAbort			; 预取指令中止异常处理跳转指令，跳往_HandlePrefetchAbort处
    b   HandleDataAbort				; 数据中止异常处理跳转指令，跳往_HandleDataAbort处
HandleNotUsed
    b	HandleNotUsed				; 未使用异常处理跳转指令，没有处理
    b 	HandleIRQ					; 一般中断异常处理跳转指令，跳往本源文件中
    								;HandleIRQ符号处
HandleFIQ
    b	HandleFIQ					; 快速中断异常处理跳转指令，没有处理


Reset 								; Reset异常处理符号
    bl	clock_init					; 跳往时钟初始化处理
    bl	mem_init					; 跳往内存初始化处理
    ldr	sp, =SVC_STACK_BASE			; 设置管理模式栈指针
    bl	disable_watchdog			; 关闭看门狗
; 代码拷贝，将代码拷贝到内存里去运行，如果从Nandflash							
;启动运行，其RAM steppingstone只有4k，不足已运行全部代							
;码，如果从Norflash启动，其硬件特性决定其运行速度较慢，						
;因此，将代码拷贝到内存里去运行
copy_code 							; 代码拷贝开始符号 
    mov r0, #0x0					; R0中为数据开始地址 (ROM数据保存在0地址开始处)
    ldr	r1, =|Image$$RO$$Base|		; R1中存放RO输出域运行地址，
									; 该值由符号变量Image$$EXEC_RO$$Base取得
	ldr r2, =|Image$$ZI$$Limit|
    								; 该值由符号变量Image$$EXEC_ZI$$Limit取得
    sub r2, r2, r1					; R2 = R2 - R1，得出待拷贝数据长度
    ldr r1, =KERNEL_RUN_PA
    bl	CopyCode2Ram				; 将R0,R1,R2三个参数传递给CopyCode2Ram函数执行拷贝
    
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
	
	ldr lr, =RUN_VM					; RUN_VM是运行地址的偏移地址（0x80000000+OFT）
	ldr r0, =__enable_mmu			; r0也是运行地址的偏移地址（0x80000000+__enable_mmu）
	sub r0, r0, #VA_TO_PA_OFT		; 因为MMU还没有使能，所以还在物理内存里执行,因此要减VA_TO_PA_OFT
	mov pc, r0						; 一旦开启了MMU，虚拟地址0x80000000和0x30000000都可以使用
RUN_VM			
	bl __create_page_tables_post	; 这儿已经开启了MMU，要切断原先的0X300000000地址映射，所以保证代码运行在0x800000000中
	
    ; init all mode stack
    bl stack_init					; 跳往栈初始化代码处

    msr	cpsr_c,	#0x5f    			; 开启系统中断，进入系统模式
    ldr	lr, =halt_loop				; 设置返回地址
    ldr	pc, =xmain					; 跳往main函数，进入OS启动处理
halt_loop 							; OS返回地址，其实这儿永远不可能被执行到，因为只要OS工
									; 作，它就会运行到世界末日
    b	halt_loop					; 死循环

delay
	mov r0, #0x100000
	mov r1, #0
loop
	cmp r1, r0
	sub r0, r0, #1
	bne loop
	mov pc, lr
	

HandleIRQ 							; 系统中断处理
    sub	lr, lr, #4					; 修正返回地址
    ldr	sp, =(IRQ_STACK_BASE + VA_TO_PA_OFT) ; 设置中断模式下栈指针
    stmdb   sp!, {r0-r12,lr}  		; 保存现场
    ldr	lr, =int_return				; 设置中断处理程序的返回地址
    ldr	pc, =handle_irq				; 跳往中断处理程序
int_return 							; 返回地址
    ldmia   sp!, {r0-r12,pc}^		; 恢复被中断打断现场

clock_init 							; 时钟初始化代码，详细见时钟初始化章节
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

mem_init 							; 内存初始化代码，详细见内存初始化章节
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
	
stack_init 							; 栈指针初始化
    ; undefine_stack				; 未定义异常
    msr cpsr_c, #0xdb
    ldr	sp, =(UND_STACK_BASE + VA_TO_PA_OFT)
    ; abort_stack					; 未定义异常模式
    msr cpsr_c, #0xd7
    ldr	sp, =(ABT_STACK_BASE + VA_TO_PA_OFT)
    ; irq_stack						; 中断模式
    msr	cpsr_c,	#0xd2
    ldr	sp, =(IRQ_STACK_BASE + VA_TO_PA_OFT)

    ; sys_stack						; 系统模式
    msr	cpsr_c,	#0xdf	
    ldr	sp, =(SYS_STACK_BASE + VA_TO_PA_OFT)
    
    ; svr_stack						; 切换回管理模式
    msr	cpsr_c,	#0xd3	
    mov pc, lr
    
HandleUndef 						; 未定义异常处理
    add lr, lr, #4					; 修正返回地址
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; 保存现场
    mrs r0, spsr					; 发生异常时，将状态寄存器里的数据保存在栈里
    stmdb   sp!, {r0}
    mov r0, sp						; 发生异常时，将将栈指针传递给异常处理函数用于打印异常现场信息			
    ldr pc, =undef_excp
    b	halt_loop

HandlePrefetchAbort 				; 未定义异常处理
    sub lr, lr, #4					; 修正返回地址
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; 保存现场
    mrs r0, spsr					; 发生异常时，将状态寄存器里的数据保存在栈里
    stmdb   sp!, {r0}
    mov r0, sp						; 发生异常时，将将栈指针传递给异常处理函数用于打印异常现场信息	
    ldr pc, =prefetch_abt
    b	halt_loop

HandleDataAbort 					; 未定义异常处理
    sub lr, lr, #8					; 修正返回地址
    stmdb   sp!, {lr}				; pc
    stmdb   sp!, {lr}^				; lr
    stmdb   sp!, {sp}^				; sp
    stmdb   sp!, {r0-r12}			; 保存现场
    mrs r0, spsr					; 发生异常时，将状态寄存器里的数据保存在栈里
    stmdb   sp!, {r0}
    mov r0, sp						; 发生异常时，将将栈指针传递给异常处理函数用于打印异常现场信息	
    ldr pc, =data_abt
    b	halt_loop
    
	END
