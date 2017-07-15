#include "sched.h"
#include "s3c2440.h"
#include "mmu.h"
#include "libs.h"
#include "interrupt.h"
#include "serial.h"
#define SYS_MODE_STACK_BASE (KERNEL_LIMIT - 4*KB)

PCB task[TASK_SZ];								// 进程队列(PCB数组)，最多支持62个进程
PCB*  current = NULL;							// 声明当前执行进程指针
long runningCount = 0;							// 正在运行进程个数
extern void __switch_to(PCB *pcur,PCB *pnext);	// 引入外部声明上下文切换函数

/*****************************************************************************
* 进程队列初始化函数
*****************************************************************************/
void sched_init(void)
{
	PCB *p = &task[0];					// 0号进程为内核进程
	int i;
	/* 循环为每个进程PCB初始化 */
	for(i = 0; i < TASK_SZ; i++, p++){
		p->pid = -1;					// pid = -1，表示未分配pid
		p->state = TASK_UNALLOCATE;		// 设置初始进程状态为未分配状态
		p->count = 0;					// 设置时间片计数为0，表示没有时间片
		p->priority = 0;				// 初始进程优先级为0
	}
	/* 初始化0号进程 */ 
	p = &task[0];						// p指向0号进程PCB
	p->pid = 0;							// 设置0号进程pid
	p->state = TASK_RUNNING;			// 设置其运行状态为就绪态
	p->count = 5;						// 设置其时间片为5
	p->priority = 5;					// 设置优先级为5
	p->content[0] = CPSR_USR_MODE;		// 保存状态寄存器cpsr值，用户模式，开启中断
	p->content[1] = USER_APP_STACK_BASE;// 设置当前进程栈指针
	p->content[2] = 0;   	
	p->content[16]= 0;					// 设置PC寄存器的值为0，该进程起始地址被MMU映射为0地址

	current = &task[0];					// 当前运行进程为0号进程
	DPRINTK(KERNEL_DEBUG,"kernel:sched_init, all task init OK\r\n");
}

/*****************************************************************************
* 杀死进程函数
* 参数：int pid：0：随机杀死一个进程，pid不为0：杀死指定进程id号为pid进程
*****************************************************************************/
void kill_task(int pid){
	int i;
	DPRINTK(KERNEL_DEBUG,"kernel:kill_task\r\n");
	for(i = 1; i < TASK_SZ; i++){
		if(task[i].state != TASK_UNALLOCATE){
			if(pid == 0 || pid == task[i].pid){
				task[i].pid = -1;
				task[i].state = TASK_UNALLOCATE;
				task[i].count = 0;
				task[i].priority = 0;
				runningCount--;
				break;
			} 
		}
	}
	// 杀死进程后，重新调度
	schedule();
}

/*****************************************************************************
* 进程调度函数
* 从当前进程就绪队列中挑选出优先级最高的进程执行，如果没有就绪进程，执行内核进程
* 如果当前进程还是最高优先级，则继续执行当前进程
* 如果有进程比当前进程优先级高，则进行上下文切换
*****************************************************************************/
void schedule(void)
{
	/* 
	* max用来保存当前进程队列里最高优先级进程count
	* p_tsk保存当前进程PID副本
	*/
	long max = -1;						// max初始值为-1，后面做判断
	long i = 0,  next = 0;				// next保存最高优先级PID
	PCB * p_tsk = NULL;					// 临时进程结构体指针
	
	// 如果只有一个进程且为0号进程,则赋予其时间片,返回
	if(runningCount == 1 && current->pid == 0){
		current->count = current->priority;
		return ;
	}
	// 进程调度循环

	DPRINTK(KERNEL_DEBUG,"kernel:schedule\r\n");	
	while(1){
		/* 
		*  循环找出进程队列里，就绪状态最高优先级进程，也就是count值最大进程，
		*  count越大说明其被执行时间越短，CPU需求越高，
		*  同时保存其PID（进程队列数组下标）到next里
		*  0号进程不参与比较
		*/
		for(i = 1; i < TASK_SZ; i++){
			if( (task[i].state == TASK_RUNNING) && (max < (long)task[i].count) ) {
				max = (long)task[i].count;
				next = i;
			}
		}
		
		// 如果max为非0，跳出循环，说明选出了调度进程
		// 如果max为0，说明count值最大进程count为0，说明全部进程分配时间片已执行完，
		// 需要重新分配，执行break后面for语句
		// 如果max为-1说明没有就绪状态进程可被调度，退出循环，继续执行0进程

		if(max) break;		// max = 0时，选出新进程，跳出循环
		// max = 0，即进程队列中count值最大为0，全部进程时间片用尽，需要重新分配
		for(i = 1; i < TASK_SZ; i++){
			if( task[i].state == TASK_RUNNING ) {
				// 时间片数为其默认优先级
				task[i].count = task[i].priority;
			}
		}
	}
	// 当前进程为选出进程，说明当前进程优先级还是最高，返回继续执行
	if(current == &task[next])
		return;
	// 无效PID
	if(task[next].pid < 0)
		return;
	// 保存当前进程副本到p_tsk，将选出进程设置为当前运行进程
	p_tsk = current;
	current = &task[next];
	if(KERNEL_DEBUG){
		printk("__switch_to\r\n");
		printk("\n\rold task id =");
		dtos(p_tsk->pid);
		printk("\n\rnew task id =");
		dtos(next);
		printk("\n\r");
	}
	// 调用上下文切换函数
	__switch_to(p_tsk, &task[next]);
}

/*****************************************************************************
* 检查当前CPU工作模式，如果是用户模式，返回1，如果不是返回0
*****************************************************************************/
int is_in_user_space(void) {
	unsigned long __rtn;
	__asm{
		mrs __rtn, SPSR
		and __rtn, __rtn, #0x1F
		cmp	__rtn,	#0x10
		moveq __rtn, #0x1
		movne __rtn, #0x0 
	}
	return __rtn;
}

/*****************************************************************************
* 定时器处理函数
* 主要用于进程时间片处理和睡眠时间处理，每次定时器中断产生后，调用该函数，对进程时间片进行
* 递减操作，如果时间片用完，则进行调度，如果用户进程主动进入睡眠状态，则
* 该函数在睡眠时间到达后，将其唤醒
*****************************************************************************/
void do_timer(void)
{
	int i = 0;
	// 没有当前进程，说明进程还未创建，返回
	if(!current){
		DPRINTK(KERNEL_DEBUG,"kernel:leaving do_timer,hasn't init task\r\n");
		return;
	}
	// 递减睡眠进程，睡眠时间到了，将其状态改为就绪态
	for(i = 1; i < TASK_SZ; i++){
		if(task[i].state == TASK_SLEEPING){			// 检查其睡眠时间
			if(!(--task[i].timer)){
				task[i].state = TASK_RUNNING;		// 如果睡眠时间为0，唤醒它
				runningCount++;
			}
		}
	}
	// 对当前执行进程时间片递减，每10ms递减一次
	if(current->count){
		current->count--;
	}
	// 如果当前进程时间片已经用完，或当前进程状态为非就绪态，则尝试调度新进程
	if((current->state != TASK_RUNNING) || current->count <= 0 ){
		// 保障内核空间执行进程不会被抢占打断
		if(is_in_user_space())
			schedule();
	}
}

/**********************************************************************************
* 进程代码拷贝函数，用于在创建新进程后，将程序代码拷贝到其对应执行空间
**********************************************************************************/
int CopyCode2Ram2(unsigned long * src, unsigned long * dest, long size)
{
    unsigned long i;
    /* copy code from NOR Flash to RAM */
    for (i = 0; i < size / 4; i++)
    {
	    dest[i] = src[i];
	}
	return 0;
}

unsigned long * getTaskMemSpace(unsigned long pid){
	unsigned long * p = NULL, i;
	/* 
	* 新进程执行空间首地址，为其对应Pid号对应MB处，比如pid = 4,则其进程空间为
	* 内存基址 + 4MB，使用了段映射每个进程对应1MB执行空间
	*/
	p = (unsigned long  *)(KERNEL_SPACE_BASE + pid*MB);				
	// 将用户执行空间清零
	for(i = 0; i < (MB >> 2); i++)
		p[i] = 0;
	return p;
}

/*******************************************************************************
* 创建新进程函数
* 参数：unsigned long start_addr 新进程的运行地址
* 		unsigned long len 新进程代码长度
*		char *parameters 程序执行参数，以空格格开
* 		long priority 进程指定优先级
**********************************************************************************/
int OSCreateProcess(unsigned long start_addr, unsigned long len, char *parameters, long priority)
{
	unsigned long i, j, pid = -1, argc = 0;
	unsigned long *pul_user_space, *pul_args_base;
	char *pch_args;
	
	DPRINTK(KERNEL_DEBUG,"kernel:Enter OSCreateProcess\r\n");
	/* 
	* 检查用户程序是否符合程序调用规格，所有用户程序第一条指令为：ldr	r0, [sp]
	* 其对应机器码为:0xe59d0000，如果加载的是非法程序，则出错退出
	*/
	if( *((unsigned long*)start_addr) != 0xe59d0000 ){
		printk("user program error!!\r\n");
		return -1;
	}
	// 为新进程挑选可用pid
	for(i = 0; i < TASK_SZ; i++){
	    if((task[i].state == TASK_UNALLOCATE) ) {
	        pid = i;
			break;
	    }
	}
	// 如果没有可用Pid，出错，退出
	if(pid == -1){
		printk("task has to max number!\r\n");
		return -1;
	}
	// 进入进程创建，此过程中不能被中断打断
	OS_ENTER_CRITICAL();
	
	// 获得新进程执行空间首地址
	pul_user_space = getTaskMemSpace(pid);
	
	// 从Norflash将程序代码拷贝到新进程执行空间
	CopyCode2Ram2((unsigned long *)start_addr, pul_user_space, len);
	
	// -----------------以下为程序执行时所带参数处理-----------------------------------
	// 程序参数保存在新进程空间最高1KB空间内，以下简称参数空间
	pul_args_base = (unsigned long *)((unsigned long)pul_user_space + USER_APP_STACK_BASE); 
	// 参数个数变量
	argc = 0;
	i = 0;

	/* 根据parameters，计算出程序参数个数，以空格格开 */
	if(parameters){
		while(parameters[i]){
			while(parameters[i] == ' ') i++;	// 吃掉空格
			if(!parameters[i])	break;			// 到达字符串结束，退出
			argc++;								// 参数个数++
			while(parameters[i] && ( parameters[i] != ' ')) i++;	// 如果是有效参数，跳过
		}
	}
	// 现在argc里面保存的是parameters传递过来的参数个数，以空格格开

	*pul_args_base++ = argc + 1;		/* 将参数个数保存到参数空间内第一个位置 */
	*pul_args_base++ = pid;				/* 将新进程pid号保存到参数空间第二个位置 */
	
	// pch_args用来拷贝每个参数字符串，首先跳过argc + 1个位置，每个位置用来放参数字符串指针
	pch_args = (char *)(pul_args_base + argc + 1);	
	// 循环拷贝每一个参数
	for(i = 0, j = 0; i < argc; i++){
		// 将每一个参数字符串地址保存到pul_args_base指向的参数空间
		// 由于参数要让用户程序来访问,所以,参数地址应该为相对地址
		*pul_args_base++ = (unsigned long)pch_args - KERNEL_SPACE_BASE - pid*MB;
		while(parameters[j] == ' ')j++;				// 吃掉空格
		if(!parameters[j])	break;					// 到达字符串结束，退出
		// 执行参数字符串拷贝
		while(parameters[j] && ( parameters[j] != ' ')){
 			*pch_args++ = parameters[j++];
		}
		// 最后加上字符串结束符
		*pch_args = '\0';
		// 保证每个参数开始位置是4个字节对齐
	    pch_args = (char *)(((unsigned long)pch_args + 4)&(~0x3));
		if(!parameters[j])
			break;
	}
	//mem_dump((unsigned long *)((unsigned long)pul_user_space + USER_APP_STACK_BASE), 20);
	// -----------------以上为程序执行时所带参数处理-----------------------------------
	runningCount++;
	task[pid].pid = pid;					// 新进程PID
	task[pid].state = TASK_RUNNING;			// 新进程执行状态
	task[pid].count = 5;					// 新进程时间片
	task[pid].priority = priority;			// 新进程优先级
	task[pid].content[0] = CPSR_USR_MODE;	// CPSR	
	task[pid].content[1] = USER_APP_STACK_BASE; // SP栈指针
	task[pid].content[2] = 0;				// LR返回地址
	task[pid].content[16]= 0;				// PC
	// 打开中断
	OS_EXIT_CRITICAL();					
	return pid;
}

