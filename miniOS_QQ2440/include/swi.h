#define	__NR_exit			0
#define	__NR_sleep			1
#define	__NR_write			2
#define	__NR_led_on			3
#define	__NR_all_led_on		4
#define	__NR_all_led_off	5

typedef int (*fn_ptr)(void);
extern fn_ptr swi_table[];

/****************************************************************
* 在swi.S中定义，用于swi_table中实现函数跳转
****************************************************************/
extern int sys_exit(void);
extern int sys_sleep(void);
extern int sys_write(void);
extern int sys_led_on(void);
extern int sys_all_led_off(void);
extern int sys_all_led_on(void);


/****************************************************************
* 在swi.c中定义，sys_xxxx的C处理函数
*****************************************************************/
void do_exit(int error_code);
void do_sleep(unsigned long time);
int do_write(char *str);
int do_led_on(int led_no);
void do_all_led_off(void);
void do_all_led_on(void);
