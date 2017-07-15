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
* ҳ��������
*____________________________________________________________________________
* ��ҳ����entry:[31:20]�λ�ַ��[11:10]ΪAP(���Ʒ���Ȩ��)��[8:5]��
*   [3:2]=CB(decide cached  &buffered)��[1:0]=0b10-->ҳ����Ϊ��������
*
* 0~32MB:Ϊ�ں˽��̿ռ�
* 32MB~0x80000000:Ϊ�û����̿ռ�
* 0x80000000~0x84000000:Ϊ�����ڴ���ȫӳ����
* 0x98000000~0xB0000000:Ϊ����Ĵ�����ȫӳ������
*____________________________________________________________________________
****************************************************************************/

void __create_page_tables_early() 
{
	unsigned long phyaddr, viraddr;

	/* ������Norflash��2MB�ĵ�ַ�ռ��ӳ�� */
	/* 0x90000000 ӳ�䵽0��ʼ��1MB��ַ�ռ� */
    *( TTB_BASE + (NOR_1M_VM >> 20) ) = 0x0 | SEC_DESC_WB;
    /* 0x90100000 ӳ�䵽0x100000~0x1FFFFF��1MB��ַ�ռ� */
    *( TTB_BASE + (NOR_2M_VM >> 20) ) = MB | SEC_DESC_WB;

	/* ��0x30000000~0x34000000��64MB�����ַ���������ַ�ռ䣬����miniOS�ڲ����̹��� */
    for(phyaddr = SDRAM_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }

	/* ��0x80000000~0x84000000��64MB�����ַӳ�䵽0x30000000~0x34000000 */
	for(phyaddr = SDRAM_BASE, viraddr = KERNEL_SPACE_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB, viraddr+= MB) {
		*(TTB_BASE + (viraddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }

	/* ��0x48000000~0x60000000�������ַ���������ַ�ռ䣬����miniOS�ڲ�������� */
    for(phyaddr = SFR_PA_BASE; phyaddr < SFR_PA_LIMIT; phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }
    
	/* ��0x98000000~0xB0000000�������ַӳ�䵽0x48000000~0x60000000 */
    for(phyaddr = SFR_PA_BASE, viraddr = phyaddr + VA_TO_PA_OFT; phyaddr < SFR_PA_LIMIT; 
    	phyaddr += MB, viraddr+= MB) {
		*(TTB_BASE + (viraddr >> 20) ) = phyaddr | SEC_DESC_WB;
    }


	/* 
	* �쳣������ 
	* 0xFFFF0000Ϊ�ߵ�ַ�쳣����������ͨ������CP15��C1�Ĵ���Vλ�����쳣����ʱ����Ӳ���Զ�ȥ0xFFFF0000
	* ��ַ��ִ���쳣��תִ�У�������֮ǰ��0��ַ���쳣��������ת�����ǽ��������ַӳ�䵽0x33F00000��1MB��ַ
	* �ռ䣬ͬ������ȫ��miniOS���뿽������1MB��ַ�ռ�����
	*/			
	*(TTB_BASE + (0xffff0000>>20)) = (VERTOR_NEW_BASE | SEC_DESC_WB);
}


void IDCaches_Restart(void)
{
	__asm{
		mov	r0, #0
		/* ʹICaches��DCaches��Ч */
		mcr	p15, 0, r0, c7, c7, 0
		/* ʹ��д�뻺���� */
		mcr	p15, 0, r0, c7, c10, 4
		/* ʹָ�����TLB��Ч */
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
	
	// �ж�ԭ�ȵ�0x30000000~0x34000000��ӳ���ϵ��ʹ��Ϊ��Ч������
    for(phyaddr = SDRAM_BASE; phyaddr < SDRAM_BASE + SDRAM_SIZE; 
    	phyaddr += MB) {
		*(TTB_BASE + (phyaddr >> 20) ) = NONE_SEC_DESC;
    }
    
    // Ϊÿ�����̿ռ���е�ַӳ�䣬ӳ���ϵΪ taskID -> taskID*32M~taskID*32M + 0x1fffff
	for(taskId = 1; taskId < TASK_SZ; taskId++) {
		*(TTB_BASE + ((taskId*32*MB) >> 20)) = (SDRAM_BASE + taskId*MB) | SEC_DESC_WB;
    }
    
    IDCaches_Restart();
}
