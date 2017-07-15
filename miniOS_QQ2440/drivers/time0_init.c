#include "serial.h"
#include "s3c2440.h"

/*************************************************************************
* 定时器0初始化代码
* 定时器0输入时钟频率 = PCLK / (预分频器值 +1) / (分频因子)
* 	其中：	预分频器值 = 0~255
* 			分频因子 = 2, 4, 8, 16
*			输入时钟频率 = 50MHz/(49+1)/(16) = 62500Hz
* 			设置定时器计数器TCNTB0[15:0] = 625，则每10ms产生一次定时器中断
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
* 开启定时器
*************************************************************************/
void Timer0_start(void)
{
	SFR_VA(TCON) |= 0x1;						
}

/*************************************************************************
* 关闭定时器
*************************************************************************/
void Timer0_stop(void)
{
	SFR_VA(TCON) &= ~0x1;						
}