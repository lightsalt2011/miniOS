#include "swi.h"
#include "serial.h"
#include "sched.h"
#include "led.h"

/*************************************************************************
* ����ϵͳ���ú���ָ�����飬ÿ�������Ա����һ��ָ������ָ��
*************************************************************************/
fn_ptr swi_table[] = {sys_exit, sys_sleep, sys_write,
		sys_led_on, sys_all_led_off, sys_all_led_on};

/*************************************************************************
* exitϵͳ�����ں˿ռ�ʵ�ִ���
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
* sleepϵͳ�����ں˿ռ�ʵ�ִ���
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
* wirteϵͳ�����ں˿ռ�ʵ�ִ���
*************************************************************************/
int do_write(char *str)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_write\n\r");
	// �û��ռ��ַӳ�䴦��
	return printk(str);
}

/*************************************************************************
* led_onϵͳ�����ں˿ռ�ʵ�ִ���
*************************************************************************/
int do_led_on(int led_no)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_led_on\n\r");	
	led_on(led_no);
	return led_no;
}

/*************************************************************************
* all_led_offϵͳ�����ں˿ռ�ʵ�ִ���
*************************************************************************/
void do_all_led_off(void)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_all_led_off\n\r");	
	all_led_off();
}

/*************************************************************************
* all_led_onϵͳ�����ں˿ռ�ʵ�ִ���
*************************************************************************/
void do_all_led_on(void)
{
	DPRINTK(KERNEL_DEBUG,"kernel:do_all_led_on\n\r");	
	all_led_on();
}