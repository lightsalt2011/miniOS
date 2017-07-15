#define	__NR_exit			0
#define	__NR_sleep			1
#define	__NR_write			2
#define	__NR_led_on			3
#define	__NR_all_led_on		4
#define	__NR_all_led_off	5

int exit(int error_code);
int sleep(int time);
int write(char *str);
int led_on(int led_no);
void all_led_off(void);
void all_led_on(void);