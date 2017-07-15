#include "serial.h"

/*************************************************************************
* ��������������Ƿ��Ǵ�Norflash����
* ����������������Norflash��Nandflash���֣���������
* ѡ���Nand����ʱ������Nand�������н�Nandǰ4K���뿽����
* SRAM���͵�Stepping stone��SRAM�ǿɶ�д�洢��������ͨ��ָ��
* ʵ��д���������Norflash��ROM���ʹ洢����ֻ�ܶ�ȡ���ݣ�ִ��
* ִ��д�����ʱ��Ҫ�����ض�����ʵ�֣���˿���ͨ���������д������
* Ȼ���ٶ�ȡ���������Ƿ�һ�£������ж��Ƿ��Ǵ�Norflash������
* 0��ַ�����ڰ�װ���쳣��������"b	Reset"ָ���Ӧ�Ķ�������Ϊ��0xEA00000B
* ͨ����0��ַ��д������0x12345678��Ȼ���ٶ�ȡ���������Ƿ���0x12345678
* �����0x12345678��˵���Ǵ�Nand�����������0xEA00000B˵����û��д��ɹ�����
* �Ǵ�Norflash����
*************************************************************************/
int isBootFrmNORFlash(void)
{
    volatile unsigned long *pdw = (volatile unsigned long *)0;
    unsigned long dwVal;
	        
    dwVal = *pdw;       
    *pdw = 0x12345678;
    if (*pdw != 0x12345678)
    {
		return 1; 
    } else {
		*pdw = dwVal;
		return 0;
    }
}

/*************************************************************************
* ���뿽������
* ���ܣ��жϵ�ǰ������ʽ������Ǵ�Norflash�������򽫴��뿽����SDRAM�ڴ���
* 	Norflash��ROM��ֻ�ܶ�ȡ���ݲ���ִ��д�������������Norflash��ִ���ٶ�
*	��������˱���Ҫ���俽����SDRAM��
* ������
*	unsigned long start_addr��	����������ַ
*	unsigned char *buf��		Ŀ���ַ(�ڴ�����)
*	int size��					�������ݴ�С���ֽڣ�
*************************************************************************/
extern int CopyCode2Ram(unsigned long start_addr, unsigned char *buf, int size)
{
    unsigned long *pdwDest;
    unsigned long *pdwSrc;
    unsigned long i;

    if (isBootFrmNORFlash())
    {
		pdwDest = (unsigned long *)buf;
        pdwSrc  = (unsigned long *)start_addr;
        /* copy code from NOR Flash to RAM */
        for (i = 0; i < size / 4; i++)
        {
	    	pdwDest[i] = pdwSrc[i];
		}
		return 0;
    } else {
		return 0;
    }
}
