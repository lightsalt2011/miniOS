/* �������� */
#ifndef SCHED
#define SCHED

#include "s3c2440.h"

#define TASK_UNALLOCATE			-1	// PCBδ����
#define TASK_RUNNING			0	// �������ڽ��л��Ѿ�׼������
#define TASK_INTERRUPTIBLE		1	// ���̴��ڿ��жϵȴ�״̬
#define TASK_UNINTERRUPTIBLE	2	// ���̴��ڲ����жϵȴ�״̬
#define TASK_ZOMBIE				3	// ���̴��ڽ���״̬��δ�õ�
#define TASK_STOPPED			4	// �����Ѿ�ֹͣ
#define TASK_SLEEPING			5	// ���̽���˯��״̬

#define PID_OFT				0
#define	STATE_OFT			4
#define	COUNT_OFT			8
#define	PRIORITY_OFT		16
#define	CONTENT_OFT			20

#define	NULL			0

typedef struct task_struct
{
	long pid;						// ����ID
	long state;						// ����״̬
	long count;						// ����ʱ��Ƭ��
	long timer;						// ��������ʱ��
	unsigned long priority;			// �������ȼ�
	unsigned long content[17];		// ����ִ���ֳ����������Ĵ�����ֵ��
	/**
	* content[0]:�û�����״̬�Ĵ���CPSR��ֵ
	* content[1]:�����û�����SPջָ��Ĵ�����ֵ
	* content[2]:�����û�����LR���ص�ַ�Ĵ�����ֵ
	* content[3~15]:����R0~R12�Ĵ�����ֵ
	* content[16]:����PC����������Ĵ�����ֵ
	*/
} PCB;

// ���̶�������
extern PCB task[TASK_SZ];
// ��ǰ���̽ṹ��ָ��
extern PCB* current ;
// �������н��̸���
extern long runningCount;
// ��鵱ǰCPU����ģʽ��������û�ģʽ���ط�0
int is_in_user_space(void);
// ��ʱ������
void do_timer(void);
// ���̵���
extern void schedule(void);
// ɱ������
extern void kill_task(int);
// ���̵��ȳ�ʼ��
extern void sched_init(void);
// ��������
extern int OSCreateProcess(unsigned long start_addr, unsigned long len, char *parameters, long priority);
#endif
