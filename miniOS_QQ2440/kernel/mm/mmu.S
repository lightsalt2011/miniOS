	INCLUDE s3c2440_h.S
	
;++++++++++++++++++++++++++++++++++++++++++++++++++++
FAULT_DESC				EQU		(0x0<<0)
NONE_SEC_DESC			EQU		(FAULT_DESC)
;++++++++++++++++++++++++++++++++++++++++++++++++++++
VECTOR		EQU		(1<<13)	; �����쳣������λ�ã�0 = �͵�ַ0x0 1 = �ߵ�ַ0xFFFF0000
ICACHE		EQU		(1<<12)	; ����ICACHE��0 = ���� 1 = ʹ��
R_S_BIT		EQU		(3<<8)	; ��ҳ������������һ��ȷ���ڴ���ʽ���
ENDIAN		EQU		(1<<7)	; ȷ��ϵͳʹ�ô�С���ֽ���0 = С��ģʽ 1 = ���ģʽ
DCACHE		EQU		(1<<2)	; ����DCACHE��0 = ���� 1 = ʹ��
ALIGN		EQU		(1<<1)	; ���õ�ַ�����飬0 = ���� 1 = ʹ��
MMU_ON		EQU		(1<<0)	; ����MMU��0 = ���� 1 = ʹ��
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
	; ʹICaches��DCaches��Ч 
	mcr	p15, 0, r2, c7, c7, 0
	; ʹ��д�뻺���� 
	mcr	p15, 0, r2, c7, c10, 4
	; ʹָ�����TLB��Ч��Ч 
	mcr	p15, 0, r2, c8, c7, 0
	; ҳ����ַд��C2 
	mcr	p15, 0, r0, c2, c0, 0
	; ��0x2ȡ�����0xFFFFFFFD��Domain0 = 0b01Ϊ�û�ģʽ��������Ϊ0b11����ģʽ
	mvn	r2, #0x2
	; д���������Ϣ 
	mcr	p15, 0, r2, c3, c0, 0
	; ȡ��C1�Ĵ�����ֵ��reg0 
	mrc	p15, 0, r2, c1, c0, 0
	; ���������Ҫ�Ĺ��ܣ��ֿ���
	bic	r2, r2, r4
	; �������λ������MMU
	orr	r2, r2, r3	
	mcr	p15, 0, r2, c1, c0, 0
	nop
	nop
	nop
	nop
	mov pc, lr
	
	END
	
	