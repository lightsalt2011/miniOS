	INCLUDE s3c2440_h.S
	
;++++++++++++++++++++++++++++++++++++++++++++++++++++
FAULT_DESC				EQU		(0x0<<0)
NONE_SEC_DESC			EQU		(FAULT_DESC)
;++++++++++++++++++++++++++++++++++++++++++++++++++++
VECTOR		EQU		(1<<13)	; 设置异常向量表位置，0 = 低地址0x0 1 = 高地址0xFFFF0000
ICACHE		EQU		(1<<12)	; 设置ICACHE，0 = 禁用 1 = 使用
R_S_BIT		EQU		(3<<8)	; 和页表项中描述符一起确定内存访问仅限
ENDIAN		EQU		(1<<7)	; 确定系统使用大，小端字节序，0 = 小端模式 1 = 大端模式
DCACHE		EQU		(1<<2)	; 设置DCACHE，0 = 禁用 1 = 使用
ALIGN		EQU		(1<<1)	; 设置地址对齐检查，0 = 禁用 1 = 使用
MMU_ON		EQU		(1<<0)	; 设置MMU，0 = 禁用 1 = 使用
;++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	AREA MMU_INIT, CODE, READONLY
		
	EXPORT __enable_mmu
__enable_mmu
	
	ldr r0, =TTB_BASE
	ldr r1, =VECTOR
	ldr r2, =ICACHE
	orr r3, r2, r1
	ldr r1, =DCACHE
	orr r3, r3, r1
	ldr r1, =ALIGN
	orr r3, r3, r1
	ldr r1, =MMU_ON
	orr r3, r3, r1
	ldr r1, =R_S_BIT
	orr r4, r3, r1
	ldr r1, =ENDIAN
	orr r4, r4, r1
	
	mov	r2, #0
	; 使ICaches和DCaches无效 
	mcr	p15, 0, r2, c7, c7, 0
	; 使能写入缓冲器 
	mcr	p15, 0, r2, c7, c10, 4
	; 使指令，数据TLB无效无效 
	mcr	p15, 0, r2, c8, c7, 0
	; 页表基址写入C2 
	mcr	p15, 0, r0, c2, c0, 0
	; 将0x2取反变成0xFFFFFFFD，Domain0 = 0b01为用户模式，其它域为0b11管理模式
	mvn	r2, #0x2
	; 写入域控制信息 
	mcr	p15, 0, r2, c3, c0, 0
	; 取出C1寄存器中值给reg0 
	mrc	p15, 0, r2, c1, c0, 0
	; 先清除不需要的功能，现开启
	bic	r2, r2, r4
	; 设置相关位并开启MMU
	orr	r2, r2, r3	
	mcr	p15, 0, r2, c1, c0, 0
	nop
	nop
	nop
	nop
	mov pc, lr
	
	END
	
	