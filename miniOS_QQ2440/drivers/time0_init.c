#include "serial.h"
#include "s3c2440.h"

/*************************************************************************
* ��ʱ��0��ʼ������
* ��ʱ��0����ʱ��Ƶ�� = PCLK / (Ԥ��Ƶ��ֵ +1) / (��Ƶ����)
* 	���У�	Ԥ��Ƶ��ֵ = 0~255
* 			��Ƶ���� = 2, 4, 8, 16
*			����ʱ��Ƶ�� = 50MHz/(49+1)/(16) = 62500Hz
* 			���ö�ʱ��������TCNTB0[15:0] = 625����ÿ10ms����һ�ζ�ʱ���ж�
*************************************************************************/
void Timer0_init(void)
{
	SFR_VA(TCFG0) = 49;				// Prescaler0 = 49	  	 
	SFR_VA(TCFG1) = 0x03;			// Select MUX input for PWM Timer0:divider=16
	//TCNTB0 = 62; 					// Request PWM Interrput per 1ms
	SFR_VA(TCNTB0) = 625;			// Request PWM Interrput per 10ms
	//TCNTB0 = 6250;				// Request PWM Interrput per 100ms
	//TCNTB0 = 62500;				// Request PWM Interrput per 1s used to Debug
	SFR_VA(TCON) |=  (1<<1);		// Timer 0 manual update
	/* Timer 0 auto reload on Timer 0 output inverter off */
	SFR_VA(TCON) = 0x09;			
    DPRINTK(KERNEL_DEBUG, "Timer0_init OK\r\n");			
}

/*************************************************************************
* ������ʱ��
*************************************************************************/
void Timer0_start(void)
{
	SFR_VA(TCON) |= 0x1;						
}

/*************************************************************************
* �رն�ʱ��
*************************************************************************/
void Timer0_stop(void)
{
	SFR_VA(TCON) &= ~0x1;						
}