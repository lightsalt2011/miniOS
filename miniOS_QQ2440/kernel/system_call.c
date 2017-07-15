#include "swi.h"
#include "serial.h"
#include "sched.h"
#include "led.h"

/*************************************************************************
* 所有系统调用函数指针数组，每个数组成员都是一个指向函数的指针
*************************************************************************/
fn_ptr swi_table[] = {sys_exit, sys_sleep, sys_write,
		sys_led_on, sys_all_led_off, sys_all_led_on};

/*************************************************************************
* exit系统调用内核空间实现代码
*************************************************************************/
void do_exit(int error_code)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_exit\n\r");			
	current->state = TASK_UNALLOCATE;
	current->pid = -1;
	memset(current->content, sizeof(current->content), 0);
	runningCount--;
	schedule();
}

/*************************************************************************
* sleep系统调用内核空间实现代码
*************************************************************************/
void do_sleep(unsigned long time)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_sleep\n\r");			
	current->state = TASK_SLEEPING;
	current->timer = time;
	runningCount--;
	schedule();
} 

/*************************************************************************
* wirte系统调用内核空间实现代码
*************************************************************************/
int do_write(char *str)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_write\n\r");
	// 用户空间地址映射处理
	return printk(str);
}

/*************************************************************************
* led_on系统调用内核空间实现代码
*************************************************************************/
int do_led_on(int led_no)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_led_on\n\r");	
	led_on(led_no);
	return led_no;
}

/*************************************************************************
* all_led_off系统调用内核空间实现代码
*************************************************************************/
void do_all_led_off(void)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_all_led_off\n\r");	
	all_led_off();
}

/*************************************************************************
* all_led_on系统调用内核空间实现代码
*************************************************************************/
void do_all_led_on(void)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_all_led_on\n\r");	
	all_led_on();
}