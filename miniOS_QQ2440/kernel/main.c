#include "s3c2440.h"
#include "serial.h"
#include "swi.h"
#include "buzzer.h"
#include "led.h"
#include "mmu.h"
#include "interrupt.h" 

extern void sched_init(void);
extern void key_init(void);

int xmain(void)
{
	uart_init();				// ���ڳ�ʼ��
	led_init(); 				// led�Ƴ�ʼ��	
	key_init();					// ������ʼ��
	init_buzzer();
	beep(0x1ffff);
	irq_init();					// �жϳ�ʼ��
	Timer0_init();				// ��ʱ��0��ʼ��
	
	PRINT_OS_INRO();			// ��ӡOS��Ϣ
	OS_ENTER_CRITICAL();		// �ر��жϣ�׼��������̳�ʼ������
	sched_init();				// ���̵��ȳ�ʼ��
	OS_EXIT_CRITICAL();			// �����ж�
	
	// ������Ϣ
	DPRINTK(KERNEL_DEBUG,"kernel:Enter user mode to run\r\n");	
	ENTER_USR_MODE();			// �����û�ģʽ
	// ����0ִ������
	while(1){
		DPRINTK(KERNEL_DEBUG,"kernel:process 0\r\n");
		printk("process 0, idle\r\n");
		wait(1000000);
	}
	return 0;
}

