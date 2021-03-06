    IMPORT |Image$$RO$$Base|	; 引入链接器自动生成符号变量Image$$EXEC_RO$$Base
    IMPORT |Image$$RO$$Limit|	; 引入链接器自动生成符号变量Image$$EXEC_RO$$Base
    IMPORT |Image$$RW$$Base|	; 引入链接器自动生成符号变量Image$$EXEC_RO$$Base
    IMPORT |Image$$RW$$Limit|
    IMPORT |Image$$ZI$$Base|
    IMPORT |Image$$ZI$$Limit|	; 引入链接器自动生成符号变量Image$$EXEC_ZI$$Limit

SDRAM_BASE				EQU     0x30000000
SDRAM_SIZE				EQU     0x4000000

SFR_BASE				EQU     0x48000000
SFR_LIMIT				EQU     0x60000000

; 内核所用物理内存空间大小为1M
KERNLE_LIMIT			EQU   	0x100000
; 页表空间大小为16K
PAGE_TABLE_SIZE			EQU   	0x4000

TTB_BASE				EQU		SDRAM_BASE
HIGTH_VERTOR			EQU		(0xffff0000)

USER_SPACE_BASE			EQU    	0x0	
KERNEL_SPACE_BASE		EQU    	0x80000000
KERNEL_SPACE_SIZE		EQU    	(0xFFFFFFFF - KERNEL_SPACE_BASE + 1)
KERNEL_RUN_PA			EQU		(0x300F0000)
KERNEL_RUN_VA			EQU		(VA_TO_PA_OFT+KERNEL_RUN_PA)
USER_SPACE_SIZE			EQU    	(KERNEL_SPACE_BASE - USER_SPACE_BASE)
KERNEL_PAGE_TABLE_BASE	EQU    	(SDRAM_BASE)

; 不同模式栈空间
; 管理模式：		0x30100000~0x300FE000
; 系统模式：		0x300FE000~0x300FC000
; 中止模式：		0x300FC000~0x300FA000
; 未定义模式：		0x300FA000~0x300F8000
; 中断模式：		0x300F8000~0x300F6000
; 快中断模式：		0x300F6000~0x300F4000

; 栈空间大小都为8K,共计48K
STACK_SIZE				EQU    	(0x2000)			
SVC_STACK_BASE			EQU    	(SDRAM_BASE+KERNLE_LIMIT)
SYS_STACK_BASE			EQU    	(SVC_STACK_BASE - STACK_SIZE)
ABT_STACK_BASE			EQU    	(SYS_STACK_BASE - STACK_SIZE)
UND_STACK_BASE			EQU    	(ABT_STACK_BASE - STACK_SIZE)
IRQ_STACK_BASE			EQU    	(UND_STACK_BASE - STACK_SIZE)
FIQ_STACK_BASE			EQU    	(IRQ_STACK_BASE - STACK_SIZE)

VA_TO_PA_OFT 			EQU 	(KERNEL_SPACE_BASE - SDRAM_BASE)
SRCPND_VA				EQU		(0x9A000000)


	END