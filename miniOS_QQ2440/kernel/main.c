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
	uart_init();				// 串口初始化
	led_init(); 				// led灯初始化	
	key_init();					// 按键初始化
	init_buzzer();
	beep(0x1ffff);
	irq_init();					// 中断初始化
	Timer0_init();				// 定时器0初始化
	
	PRINT_OS_INRO();			// 打印OS信息
	OS_ENTER_CRITICAL();		// 关闭中断，准备进入进程初始化函数
	sched_init();				// 进程调度初始化
	OS_EXIT_CRITICAL();			// 开启中断
	
	// 调试信息
	DPRINTK(KERNEL_DEBUG,"kernel:Enter user mode to run\r\n");	
	ENTER_USR_MODE();			// 进入用户模式
	// 进程0执行内容
	while(1){
		DPRINTK(KERNEL_DEBUG,"kernel:process 0\r\n");
		printk("process 0, idle\r\n");
		wait(1000000);
	}
	return 0;
}

