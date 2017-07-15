#include "s3c2440.h"
#include "serial.h"
 
#define	TXD0READY	(1<<2)
#define	RXD0READY	(1)

/*************************************************************************
* 串口初始化代码
*************************************************************************/
void uart_init( )
{
	SFR_VA(GPHCON) |= 0xa0;		//GPH2,GPH3 used as TXD0,RXD0
	SFR_VA(GPHUP)	 = 0x0;		//GPH2,GPH3内部上拉

	SFR_VA(ULCON0)	 = 0x03;	//8N1	
	SFR_VA(UCON0)	 = 0x05;	//查询方式为轮询或中断;时钟选择为PCLK
	SFR_VA(UFCON0) = 0x00;		//不使用FIFO
	SFR_VA(UMCON0) = 0x00;		//不使用流控
	SFR_VA(UBRDIV0) = 26;		//波特率为115200,PCLK=50Mhz
//	UBRDIV0 = 53;		//波特率为57600,PCLK=50Mhz
//	UBRDIV0 = 6;		//波特率为115200,PCLK=12Mhz，会超出baudrate的误差容忍范围
//	UBRDIV0 = 12;		//波特率为57600,PCLK=12Mhz
	DPRINTK(KERNEL_DEBUG, "init Uart OK\r\n");
}

/*************************************************************************
* 从串口打印单个字符
*************************************************************************/
extern void putc(unsigned char c)
{
	while( ! (SFR_VA(UTRSTAT0) & TXD0READY) );
	SFR_VA(UTXH0) = c;
}

/*************************************************************************
* 从串口接收单个字符
*************************************************************************/
extern unsigned char getc(void)
{
	while( ! (SFR_VA(UTRSTAT0) & RXD0READY) );
	return SFR_VA(URXH0);
}

/*************************************************************************
* 打印字符串
*************************************************************************/
extern int printk(const char* str)
{
	int i = 0;
	while( str[i] ){
		putc( (unsigned char) str[i++] );
	}
	return i;
}
