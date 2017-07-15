#include "sched.h"
#include "s3c2440.h"
#include "mmu.h"
#include "libs.h"
#include "interrupt.h"
#include "serial.h"
#define SYS_MODE_STACK_BASE (KERNEL_LIMIT - 4*KB)

PCB task[TASK_SZ];								// ���̶���(PCB����)�����֧��62������
PCB*  current = NULL;							// ������ǰִ�н���ָ��
long runningCount = 0;							// �������н��̸���
extern void __switch_to(PCB *pcur,PCB *pnext);	// �����ⲿ�����������л�����

/*****************************************************************************
* ���̶��г�ʼ������
*****************************************************************************/
void sched_init(void)
{
	PCB *p = &task[0];					// 0�Ž���Ϊ�ں˽���
	int i;
	/* ѭ��Ϊÿ������PCB��ʼ�� */
	for(i = 0; i < TASK_SZ; i++, p++){
		p->pid = -1;					// pid = -1����ʾδ����pid
		p->state = TASK_UNALLOCATE;		// ���ó�ʼ����״̬Ϊδ����״̬
		p->count = 0;					// ����ʱ��Ƭ����Ϊ0����ʾû��ʱ��Ƭ
		p->priority = 0;				// ��ʼ�������ȼ�Ϊ0
	}
	/* ��ʼ��0�Ž��� */ 
	p = &task[0];						// pָ��0�Ž���PCB
	p->pid = 0;							// ����0�Ž���pid
	p->state = TASK_RUNNING;			// ����������״̬Ϊ����̬
	p->count = 5;						// ������ʱ��ƬΪ5
	p->priority = 5;					// �������ȼ�Ϊ5
	p->content[0] = CPSR_USR_MODE;		// ����״̬�Ĵ���cpsrֵ���û�ģʽ�������ж�
	p->content[1] = USER_APP_STACK_BASE;// ���õ�ǰ����ջָ��
	p->content[2] = 0;   	
	p->content[16]= 0;					// ����PC�Ĵ�����ֵΪ0���ý�����ʼ��ַ��MMUӳ��Ϊ0��ַ

	current = &task[0];					// ��ǰ���н���Ϊ0�Ž���
	DPRINTK(KERNEL_DEBUG,"kernel:sched_init, all task init OK\r\n");
}

/*****************************************************************************
* ɱ�����̺���
* ������int pid��0�����ɱ��һ�����̣�pid��Ϊ0��ɱ��ָ������id��Ϊpid����
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
	// ɱ�����̺����µ���
	schedule();
}

/*****************************************************************************
* ���̵��Ⱥ���
* �ӵ�ǰ���̾�����������ѡ�����ȼ���ߵĽ���ִ�У����û�о������̣�ִ���ں˽���
* �����ǰ���̻���������ȼ��������ִ�е�ǰ����
* ����н��̱ȵ�ǰ�������ȼ��ߣ�������������л�
*****************************************************************************/
void schedule(void)
{
	/* 
	* max�������浱ǰ���̶�����������ȼ�����count
	* p_tsk���浱ǰ����PID����
	*/
	long max = -1;						// max��ʼֵΪ-1���������ж�
	long i = 0,  next = 0;				// next����������ȼ�PID
	PCB * p_tsk = NULL;					// ��ʱ���̽ṹ��ָ��
	
	// ���ֻ��һ��������Ϊ0�Ž���,������ʱ��Ƭ,����
	if(runningCount == 1 && current->pid == 0){
		current->count = current->priority;
		return ;
	}
	// ���̵���ѭ��

	DPRINTK(KERNEL_DEBUG,"kernel:schedule\r\n");	
	while(1){
		/* 
		*  ѭ���ҳ����̶��������״̬������ȼ����̣�Ҳ����countֵ�����̣�
		*  countԽ��˵���䱻ִ��ʱ��Խ�̣�CPU����Խ�ߣ�
		*  ͬʱ������PID�����̶��������±꣩��next��
		*  0�Ž��̲�����Ƚ�
		*/
		for(i = 1; i < TASK_SZ; i++){
			if( (task[i].state == TASK_RUNNING) && (max < (long)task[i].count) ) {
				max = (long)task[i].count;
				next = i;
			}
		}
		
		// ���maxΪ��0������ѭ����˵��ѡ���˵��Ƚ���
		// ���maxΪ0��˵��countֵ������countΪ0��˵��ȫ�����̷���ʱ��Ƭ��ִ���꣬
		// ��Ҫ���·��䣬ִ��break����for���
		// ���maxΪ-1˵��û�о���״̬���̿ɱ����ȣ��˳�ѭ��������ִ��0����

		if(max) break;		// max = 0ʱ��ѡ���½��̣�����ѭ��
		// max = 0�������̶�����countֵ���Ϊ0��ȫ������ʱ��Ƭ�þ�����Ҫ���·���
		for(i = 1; i < TASK_SZ; i++){
			if( task[i].state == TASK_RUNNING ) {
				// ʱ��Ƭ��Ϊ��Ĭ�����ȼ�
				task[i].count = task[i].priority;
			}
		}
	}
	// ��ǰ����Ϊѡ�����̣�˵����ǰ�������ȼ�������ߣ����ؼ���ִ��
	if(current == &task[next])
		return;
	// ��ЧPID
	if(task[next].pid < 0)
		return;
	// ���浱ǰ���̸�����p_tsk����ѡ����������Ϊ��ǰ���н���
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
	// �����������л�����
	__switch_to(p_tsk, &task[next]);
}

/*****************************************************************************
* ��鵱ǰCPU����ģʽ��������û�ģʽ������1��������Ƿ���0
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
* ��ʱ��������
* ��Ҫ���ڽ���ʱ��Ƭ�����˯��ʱ�䴦��ÿ�ζ�ʱ���жϲ����󣬵��øú������Խ���ʱ��Ƭ����
* �ݼ����������ʱ��Ƭ���꣬����е��ȣ�����û�������������˯��״̬����
* �ú�����˯��ʱ�䵽��󣬽��份��
*****************************************************************************/
void do_timer(void)
{
	int i = 0;
	// û�е�ǰ���̣�˵�����̻�δ����������
	if(!current){
		DPRINTK(KERNEL_DEBUG,"kernel:leaving do_timer,hasn't init task\r\n");
		return;
	}
	// �ݼ�˯�߽��̣�˯��ʱ�䵽�ˣ�����״̬��Ϊ����̬
	for(i = 1; i < TASK_SZ; i++){
		if(task[i].state == TASK_SLEEPING){			// �����˯��ʱ��
			if(!(--task[i].timer)){
				task[i].state = TASK_RUNNING;		// ���˯��ʱ��Ϊ0��������
				runningCount++;
			}
		}
	}
	// �Ե�ǰִ�н���ʱ��Ƭ�ݼ���ÿ10ms�ݼ�һ��
	if(current->count){
		current->count--;
	}
	// �����ǰ����ʱ��Ƭ�Ѿ����꣬��ǰ����״̬Ϊ�Ǿ���̬�����Ե����½���
	if((current->state != TASK_RUNNING) || current->count <= 0 ){
		// �����ں˿ռ�ִ�н��̲��ᱻ��ռ���
		if(is_in_user_space())
			schedule();
	}
}

/**********************************************************************************
* ���̴��뿽�������������ڴ����½��̺󣬽�������뿽�������Ӧִ�пռ�
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
	* �½���ִ�пռ��׵�ַ��Ϊ���ӦPid�Ŷ�ӦMB��������pid = 4,������̿ռ�Ϊ
	* �ڴ��ַ + 4MB��ʹ���˶�ӳ��ÿ�����̶�Ӧ1MBִ�пռ�
	*/
	p = (unsigned long  *)(KERNEL_SPACE_BASE + pid*MB);				
	// ���û�ִ�пռ�����
	for(i = 0; i < (MB >> 2); i++)
		p[i] = 0;
	return p;
}

/*******************************************************************************
* �����½��̺���
* ������unsigned long start_addr �½��̵����е�ַ
* 		unsigned long len �½��̴��볤��
*		char *parameters ����ִ�в������Կո��
* 		long priority ����ָ�����ȼ�
**********************************************************************************/
int OSCreateProcess(unsigned long start_addr, unsigned long len, char *parameters, long priority)
{
	unsigned long i, j, pid = -1, argc = 0;
	unsigned long *pul_user_space, *pul_args_base;
	char *pch_args;
	
	DPRINTK(KERNEL_DEBUG,"kernel:Enter OSCreateProcess\r\n");
	/* 
	* ����û������Ƿ���ϳ�����ù�������û������һ��ָ��Ϊ��ldr	r0, [sp]
	* ���Ӧ������Ϊ:0xe59d0000��������ص��ǷǷ�����������˳�
	*/
	if( *((unsigned long*)start_addr) != 0xe59d0000 ){
		printk("user program error!!\r\n");
		return -1;
	}
	// Ϊ�½�����ѡ����pid
	for(i = 0; i < TASK_SZ; i++){
	    if((task[i].state == TASK_UNALLOCATE) ) {
	        pid = i;
			break;
	    }
	}
	// ���û�п���Pid�������˳�
	if(pid == -1){
		printk("task has to max number!\r\n");
		return -1;
	}
	// ������̴������˹����в��ܱ��жϴ��
	OS_ENTER_CRITICAL();
	
	// ����½���ִ�пռ��׵�ַ
	pul_user_space = getTaskMemSpace(pid);
	
	// ��Norflash��������뿽�����½���ִ�пռ�
	CopyCode2Ram2((unsigned long *)start_addr, pul_user_space, len);
	
	// -----------------����Ϊ����ִ��ʱ������������-----------------------------------
	// ��������������½��̿ռ����1KB�ռ��ڣ����¼�Ʋ����ռ�
	pul_args_base = (unsigned long *)((unsigned long)pul_user_space + USER_APP_STACK_BASE); 
	// ������������
	argc = 0;
	i = 0;

	/* ����parameters���������������������Կո�� */
	if(parameters){
		while(parameters[i]){
			while(parameters[i] == ' ') i++;	// �Ե��ո�
			if(!parameters[i])	break;			// �����ַ����������˳�
			argc++;								// ��������++
			while(parameters[i] && ( parameters[i] != ' ')) i++;	// �������Ч����������
		}
	}
	// ����argc���汣�����parameters���ݹ����Ĳ����������Կո��

	*pul_args_base++ = argc + 1;		/* �������������浽�����ռ��ڵ�һ��λ�� */
	*pul_args_base++ = pid;				/* ���½���pid�ű��浽�����ռ�ڶ���λ�� */
	
	// pch_args��������ÿ�������ַ�������������argc + 1��λ�ã�ÿ��λ�������Ų����ַ���ָ��
	pch_args = (char *)(pul_args_base + argc + 1);	
	// ѭ������ÿһ������
	for(i = 0, j = 0; i < argc; i++){
		// ��ÿһ�������ַ�����ַ���浽pul_args_baseָ��Ĳ����ռ�
		// ���ڲ���Ҫ���û�����������,����,������ַӦ��Ϊ��Ե�ַ
		*pul_args_base++ = (unsigned long)pch_args - KERNEL_SPACE_BASE - pid*MB;
		while(parameters[j] == ' ')j++;				// �Ե��ո�
		if(!parameters[j])	break;					// �����ַ����������˳�
		// ִ�в����ַ�������
		while(parameters[j] && ( parameters[j] != ' ')){
 			*pch_args++ = parameters[j++];
		}
		// �������ַ���������
		*pch_args = '\0';
		// ��֤ÿ��������ʼλ����4���ֽڶ���
	    pch_args = (char *)(((unsigned long)pch_args + 4)&(~0x3));
		if(!parameters[j])
			break;
	}
	//mem_dump((unsigned long *)((unsigned long)pul_user_space + USER_APP_STACK_BASE), 20);
	// -----------------����Ϊ����ִ��ʱ������������-----------------------------------
	runningCount++;
	task[pid].pid = pid;					// �½���PID
	task[pid].state = TASK_RUNNING;			// �½���ִ��״̬
	task[pid].count = 5;					// �½���ʱ��Ƭ
	task[pid].priority = priority;			// �½������ȼ�
	task[pid].content[0] = CPSR_USR_MODE;	// CPSR	
	task[pid].content[1] = USER_APP_STACK_BASE; // SPջָ��
	task[pid].content[2] = 0;				// LR���ص�ַ
	task[pid].content[16]= 0;				// PC
	// ���ж�
	OS_EXIT_CRITICAL();					
	return pid;
}

