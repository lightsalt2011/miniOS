#include   	"s3c2440.h"
#include   	"serial.h"
#include	"mmu.h"

#define MANAGER_AP	    	(0x3<<10)
#define USER_AP    			(0x1<<10)
#define MANAGER_DOMAIN		(0x1<<5)
#define USER_DOMAIN			(0x0<<5)
#define FAULT_DOMAIN		(0x0<<5)
#define CB	    			(0x1<<2)
#define DESC_FLAG   		(0x2<<0)
#define FAULT_DESC   		(0x0<<0)
#define SEC_DESC_WB			(MANAGER_AP | MANAGER_DOMAIN | CB | DESC_FLAG)
#define NONE_SEC_DESC 		(FAULT_DESC)

#define TTB_BASE	    	((unsigned long *)SDRAM_BASE)
#define VERTOR_NEW_BASE		(SDRAM_BASE)
/****************************************************************************
* 页表建立函数
*____________________________________________________________________________
* 段页表项entry:[31:20]段基址，[11:10]为AP(控制访问权限)，[8:5]域，
*   [3:2]=CB(decide cached  &buffered)，[1:0]=0b10-->页表项为段描述符
*
* 0~32MB:为内核进程空间
* 32MB~0x80000000:为用户进程空间
* 0x80000000~0x84000000:为物理内存完全映射区
* 0x98000000~0xB0000000:为外设寄存器完全映射区间
*____________________________________________________________________________
****************************************************************************/

void __create_page_tables_early() 
{
	unsigned long phyaddr, viraddr;

	/* 建立到Norflash的2MB的地址空间的映射 */
	/* 0x90000000 映射到0开始的1MB地址空间 */
    *( TTB_BASE + (NOR_1M_VM >> 20) ) = 0x0 | SEC_DESC_WB;
    /* 0x90100000 映射到0x100000~0x1FFFFF的1MB地址空间 */
    *( TTB_BASE + (NOR_2M_VM >> 20) ) = MB | SEC_DESC_WB;

	/* 令0x30000000~0x34000000的64MB虚拟地址等于物理地址空间，方便miniOS内部进程管理 */
    for(phyaddr = SDRAM_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }

	/* 令0x80000000~0x84000000的64MB虚拟地址映射到0x30000000~0x34000000 */
	for(phyaddr = SDRAM_BASE, viraddr = KERNEL_SPACE_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB, viraddr+= MB) {
		*(TTB_BASE + (viraddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }

	/* 令0x48000000~0x60000000的虚拟地址等于物理地址空间，方便miniOS内部外设管理 */
    for(phyaddr = SFR_PA_BASE; phyaddr < SFR_PA_LIMIT; phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }
    
	/* 令0x98000000~0xB0000000的虚拟地址映射到0x48000000~0x60000000 */
    for(phyaddr = SFR_PA_BASE, viraddr = phyaddr + VA_TO_PA_OFT; phyaddr < SFR_PA_LIMIT; 
    	phyaddr += MB, viraddr+= MB) {
		*(TTB_BASE + (viraddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }


	/* 
	* 异常向量表 
	* 0xFFFF0000为高地址异常向量表，可以通常设置CP15，C1寄存器V位，当异常产生时，由硬件自动去0xFFFF0000
	* 地址处执行异常跳转执行，而不是之前的0地址处异常向量表跳转，我们将该虚拟地址映射到0x33F00000这1MB地址
	* 空间，同样，将全部miniOS代码拷贝到这1MB地址空间来。
	*/			
	*(TTB_BASE + (0xffff0000>>20)) = (VERTOR_NEW_BASE | SEC_DESC_WB);
}


void IDCaches_Restart(void)
{
	__asm{
		mov	r0, #0
		/* 使ICaches和DCaches无效 */
		mcr	p15, 0, r0, c7, c7, 0
		/* 使能写入缓冲器 */
		mcr	p15, 0, r0, c7, c10, 4
		/* 使指令，数据TLB无效 */
		mcr	p15, 0, r0, c8, c7, 0
		nop
		nop
		nop
		nop
	}
}

void __create_page_tables_post(void) 
{
    unsigned long taskId = 1;
	unsigned long phyaddr;
	
	// 切断原先的0x30000000~0x34000000的映射关系，使其为无效描述符
    for(phyaddr = SDRAM_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = NONE_SEC_DESC;
    }
    
    // 为每个进程空间进行地址映射，映射关系为 taskID -> taskID*32M~taskID*32M + 0x1fffff
	for(taskId = 1; taskId < TASK_SZ; taskId++) {
		*(TTB_BASE + ((taskId*32*MB) >> 20)) = (SDRAM_BASE + taskId*MB) | SEC_DESC_WB;
    }
    
    IDCaches_Restart();
}
