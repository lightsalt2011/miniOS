#include "serial.h"

/*************************************************************************
* 本函数用来检测是否是从Norflash启动
* 常见的启动介质有Norflash和Nandflash两种，当开发板
* 选择从Nand启动时，由于Nand控制器中将Nand前4K代码拷贝到
* SRAM类型的Stepping stone，SRAM是可读写存储器，可以通过指令
* 实现写入操作，而Norflash是ROM类型存储器，只能读取数据，执行
* 执行写入操作时，要借助特定程序实现，因此可以通过向介质中写入数据
* 然后再读取出来，看是否一致，即可判断是否是从Norflash启动。
* 0地址处由于安装有异常向量表，而"b	Reset"指令对应的二进行码为：0xEA00000B
* 通过向0地址处写入数据0x12345678，然后再读取出来，看是否是0x12345678
* 如果是0x12345678则说明是从Nand启动，如果是0xEA00000B说明，没有写入成功，则
* 是从Norflash启动
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
* 代码拷贝函数
* 功能：判断当前启动方式，如果是从Norflash启动，则将代码拷贝到SDRAM内存中
* 	Norflash是ROM，只能读取数据不能执行写入操作，并且在Norflash中执行速度
*	较慢，因此必须要将其拷贝到SDRAM中
* 参数：
*	unsigned long start_addr：	开启拷贝地址
*	unsigned char *buf：		目标地址(内存区域)
*	int size：					拷贝数据大小（字节）
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
