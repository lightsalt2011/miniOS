#include "s3c2440.h"
#include "serial.h"
 
#define	TXD0READY	(1<<2)
#define	RXD0READY	(1)

/*************************************************************************
* ���ڳ�ʼ������
*************************************************************************/
void uart_init( )
{
	SFR_VA(GPHCON) |= 0xa0;		//GPH2,GPH3 used as TXD0,RXD0
	SFR_VA(GPHUP)	 = 0x0;		//GPH2,GPH3�ڲ�����

	SFR_VA(ULCON0)	 = 0x03;	//8N1	
	SFR_VA(UCON0)	 = 0x05;	//��ѯ��ʽΪ��ѯ���ж�;ʱ��ѡ��ΪPCLK
	SFR_VA(UFCON0) = 0x00;		//��ʹ��FIFO
	SFR_VA(UMCON0) = 0x00;		//��ʹ������
	SFR_VA(UBRDIV0) = 26;		//������Ϊ115200,PCLK=50Mhz
//	UBRDIV0 = 53;		//������Ϊ57600,PCLK=50Mhz
//	UBRDIV0 = 6;		//������Ϊ115200,PCLK=12Mhz���ᳬ��baudrate��������̷�Χ
//	UBRDIV0 = 12;		//������Ϊ57600,PCLK=12Mhz
	DPRINTK(KERNEL_DEBUG, "init Uart OK\r\n");
}

/*************************************************************************
* �Ӵ��ڴ�ӡ�����ַ�
*************************************************************************/
extern void putc(unsigned char c)
{
	while( ! (SFR_VA(UTRSTAT0) & TXD0READY) );
	SFR_VA(UTXH0) = c;
}

/*************************************************************************
* �Ӵ��ڽ��յ����ַ�
*************************************************************************/
extern unsigned char getc(void)
{
	while( ! (SFR_VA(UTRSTAT0) & RXD0READY) );
	return SFR_VA(URXH0);
}

/*************************************************************************
* ��ӡ�ַ���
*************************************************************************/
extern int printk(const char* str)
{
	int i = 0;
	while( str[i] ){
		putc( (unsigned char) str[i++] );
	}
	return i;
}
