/* 条件编译 */
#ifndef SCHED
#define SCHED

#include "s3c2440.h"

#define TASK_UNALLOCATE			-1	// PCB未分配
#define TASK_RUNNING			0	// 进程正在进行或已经准备就绪
#define TASK_INTERRUPTIBLE		1	// 进程处于可中断等待状态
#define TASK_UNINTERRUPTIBLE	2	// 进程处于不可中断等待状态
#define TASK_ZOMBIE				3	// 进程处于僵死状态，未用到
#define TASK_STOPPED			4	// 进程已经停止
#define TASK_SLEEPING			5	// 进程进入睡眠状态

#define PID_OFT				0
#define	STATE_OFT			4
#define	COUNT_OFT			8
#define	PRIORITY_OFT		16
#define	CONTENT_OFT			20

#define	NULL			0

typedef struct task_struct
{
	long pid;						// 进程ID
	long state;						// 进程状态
	long count;						// 进程时间片数
	long timer;						// 进程休眠时间
	unsigned long priority;			// 进程优先级
	unsigned long content[17];		// 进程执行现场保存区（寄存器的值）
	/**
	* content[0]:用户进程状态寄存器CPSR的值
	* content[1]:保存用户进程SP栈指针寄存器的值
	* content[2]:保存用户进程LR返回地址寄存器的值
	* content[3~15]:保存R0~R12寄存器的值
	* content[16]:保存PC程序计数器寄存器的值
	*/
} PCB;

// 进程队列数组
extern PCB task[TASK_SZ];
// 当前进程结构体指针
extern PCB* current ;
// 正在运行进程个数
extern long runningCount;
// 检查当前CPU工作模式，如果是用户模式返回非0
int is_in_user_space(void);
// 定时器处理
void do_timer(void);
// 进程调度
extern void schedule(void);
// 杀死进程
extern void kill_task(int);
// 进程调度初始化
extern void sched_init(void);
// 创建进程
extern int OSCreateProcess(unsigned long start_addr, unsigned long len, char *parameters, long priority);
#endif
