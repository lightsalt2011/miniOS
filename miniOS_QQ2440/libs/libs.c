#include "serial.h"
#include "s3c2440.h"
    
void xtos(unsigned long n){
	unsigned long i;
	if((i=n/16)!=0)
		xtos(i);
	if(n%16 > 9)
		putc(n%16 - 10 +'A');
	else
		putc(n%16+'0');
}

void dtos(unsigned long n){
	unsigned long i;
	if((i=n/10)!=0)
		xtos(i);
	putc(n%10+'0');
}
    
void memset(char * dest, long len, int value){
	if(dest == NULL || len <= 0)
		return;
	while(len--)
		*dest++ = value;
	return ;
}
    
// not safe copy 
char * memcpy(char * dest, const char * src, long len){
	char * temp = dest;
	if(dest == NULL || src == NULL || len <= 0)
		return NULL;
	
	while((dest - temp) != len)
		*dest++ = *src++;
	return temp;
}

void mem_dump(const unsigned long * src, long len){
	const unsigned long * temp = src;
	if(src == NULL || len <= 0)
		return;
	while(src-temp != len){
		printk("\r\n 0x");
		xtos(*src++);
		if((src-temp)%5 == 0)
			printk("\r\n");
	}
	printk("\r\n");
}


void stack_dump(unsigned long * sp){
	printk("\r\n CPSR:0x");
	xtos(*sp++);
	printk("\r\n   R0:0x");
	xtos(*sp++);
	printk("\r\n   R1:0x");
	xtos(*sp++);
	printk("\r\n   R2:0x");
	xtos(*sp++);
	printk("\r\n   R3:0x");
	xtos(*sp++);
	printk("\r\n   R4:0x");
	xtos(*sp++);
	printk("\r\n   R5:0x");
	xtos(*sp++);
	printk("\r\n   R6:0x");
	xtos(*sp++);
	printk("\r\n   R7:0x");
	xtos(*sp++);
	printk("\r\n   R8:0x");
	xtos(*sp++);
	printk("\r\n   R9:0x");
	xtos(*sp++);
	printk("\r\n  R10:0x");
	xtos(*sp++);
	printk("\r\n  R11:0x");
	xtos(*sp++);
	printk("\r\n  R12:0x");
	xtos(*sp++);
	printk("\r\n   SP:0x");
	xtos(*sp++);
	printk("\r\n   LR:0x");
	xtos(*sp++);
	printk("\r\n   PC:0x");
	xtos(*sp++);
}