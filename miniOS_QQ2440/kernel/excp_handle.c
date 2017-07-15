#include "serial.h"
#include "buzzer.h"
#include "libs.h"

/* print the kernel panic information */
#define kernel_panic()	 					\
    do{										\
		printk("\r\nKernel panic!!\r\n");	\
		buzzer_on();						\
		while(1); 							\
    } while(0)


void undef_excp(unsigned long * sp)
{
    printk("\r\nUndefine exception\r\n");
    stack_dump(sp);
    kernel_panic();
    // this will print the stack content
}

void prefetch_abt(unsigned long * sp)
{
    printk("\r\nPrefetch abort exception\r\n");
    stack_dump(sp);
    kernel_panic();
    // this will print the stack content
}

void data_abt(unsigned long * sp)
{
    printk("\r\nData abort exception\r\n");
    stack_dump(sp);
    kernel_panic();
    // this will print the stack content
}

