#include 	"s3c2440.h"
#include   	"serial.h"
#include   	"led.h"

#define	ALL_LED_ON	(1<<5 | 1<<6 | 1<<7 | 1<<8)
#define	ALL_LED_OFF	(0)
#define LED_BIT 	(1<<5 | 1<<6 | 1<<7 | 1<<8)
#define	LED1_ON		(1<<6 | 1<<7 | 1<<8)
#define	LED2_ON		(1<<5 | 1<<7 | 1<<8)
#define	LED3_ON		(1<<5 | 1<<6 | 1<<8)
#define	LED4_ON		(1<<5 | 1<<6 | 1<<7)

/* led��ʼ�� */
void led_init(void){
	SFR_VA(GPBCON) = 0x15400;
	all_led_on();
	DPRINTK(KERNEL_DEBUG, "led_init OK\r\n"); 
}

/* ���ݲ���led_no������Ӧ�� */
void led_on(int led_no){
	switch(led_no){
		case 1:
			SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) & ~LED_BIT) | LED1_ON;
			break;
		case 2:
			SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) & ~LED_BIT) | LED2_ON;
			break;
		case 3:
			SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) & ~LED_BIT) | LED3_ON;
			break;
		case 4:
			SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) & ~LED_BIT) | LED4_ON;
			break;
		default:
			printk("error led number!!");
	}
}

/* ����ȫ��led�� */
void all_led_on(void){
	SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) & ~ALL_LED_ON);
}


/* �ر�ȫ��led�� */
void all_led_off(void){
	SFR_VA(GPBDAT)  = (SFR_VA(GPBDAT) | ~ALL_LED_OFF);
}
