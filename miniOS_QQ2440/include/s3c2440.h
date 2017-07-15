#ifndef S3C2440_H
#define	S3C2440_H	

#define KERNEL_DEBUG	1
#define DPRINTK(x,y)	if((x))printk(y)
#define DPUTC(x,y)		if((x))putc(y)

/* print the welcome information */
#define PRINT_OS_INRO()	 					\
    do{										\
		printk("\r\nWelcome to use miniOS v2.0 for learning and teach"); \
		printk("\r\n\t\t\tMichael Tang  2012.03.06"); \
		printk("\r\n  > 2010.05.02 miniOS v1.0 "); \
		printk("\r\n  > 2012.03.06 miniOS v2.0 Remap process memory space. "); \
		printk("\n\rminiOS is loading..."); \
    } while(0)
    
#define	KB					(1<<10)
#define	MB					(1<<20)
#define	GB					(1<<30)
#define	NULL				0

#define	TASK_SZ				64				// 支持最大进程个数
#define SDRAM_BASE			0x30000000
#define	SDRAM_SIZE			0x04000000		/* 64M */
#define KERNEL_OS_LIMIT		(SDRAM_BASE+MB)	

// MMU mapping 
#define KERNEL_SPACE_BASE	0x80000000	

#define VA_TO_PA_OFT 		(KERNEL_SPACE_BASE - SDRAM_BASE)
#define VA_TO_PA(add) 		(add-VA_TO_PA_OFT)
#define PA_TO_VA(add) 		(add+VA_TO_PA_OFT)
#define SFR_VA(SFR)   		(*(volatile unsigned long *)((unsigned long)(&SFR)+VA_TO_PA_OFT))

// Norflash mapping area
#define NOR_1M_VM			(0xC0000000)
#define NOR_2M_VM			(NOR_1M_VM+MB)

// User application
#define USER_APP_VM_SPACE   (32*MB)
#define USER_APP_STACK_BASE (MB-KB)
#define USER_APP_ARGS_SIZE  (KB)

#define CPSR_USR_MODE		0x50
#define CPSR_SYS_MODE		0x5F


/* SDRAM regisers */
#define MEM_CTL_BASE		0x48000000

// 特殊寄存器开始地址
#define SFR_PA_BASE			0x48000000
// 特殊寄存器空间大小
#define SFR_PA_LIMIT		0x60000000
#define SFR_VA_BASE			(KERNEL_SPACE_BASE + SDRAM_SIZE)
#define SFR_SPACE_BASE		SFR_VA_BASE
#define SFR_VA_PA_OFFSET	(SFR_VA_BASE - SFR_PA_BASE)		


__inline void wait(unsigned long dly)
{
    unsigned long i;
    for( i = dly; i > 0; i--);
}

/* WOTCH DOG register */
#define WTCON				(*(volatile unsigned long *)0x53000000)

/* NAND Flash registers */
#define NFCONF				(*(volatile unsigned long *)0x4e000000)
#define NFCONT				(*(volatile unsigned long *)0x4e000004)
#define NFCMD				(*(volatile unsigned char *)0x4e000008)
#define NFADDR				(*(volatile unsigned char *)0x4e00000c)
#define NFDATA				(*(volatile unsigned char *)0x4e000010)
#define NFSTAT				(*(volatile unsigned char *)0x4e000020)

/*GPIO registers*/
#define	GPBCON				(*(volatile unsigned long *)0x56000010)
#define	GPBDAT				(*(volatile unsigned long *)0x56000014)

#define	GPFCON				(*(volatile unsigned long *)0x56000050)
#define	GPFDAT				(*(volatile unsigned long *)0x56000054)
#define	GPFUP				(*(volatile unsigned long *)0x56000058)

#define	GPGCON				(*(volatile unsigned long *)0x56000060)
#define	GPGDAT				(*(volatile unsigned long *)0x56000064)
#define	GPGUP				(*(volatile unsigned long *)0x56000068)

#define	GPHCON				(*(volatile unsigned long *)0x56000070)
#define	GPHDAT				(*(volatile unsigned long *)0x56000074)
#define	GPHUP				(*(volatile unsigned long *)0x56000078)

/*UART registers*/
#define	ULCON0				(*(volatile unsigned long *)0x50000000)
#define	UCON0				(*(volatile unsigned long *)0x50000004)
#define	UFCON0			 	(*(volatile unsigned long *)0x50000008)
#define	UMCON0				(*(volatile unsigned long *)0x5000000c)
#define	UTRSTAT0			(*(volatile unsigned long *)0x50000010)
#define	UTXH0		 		(*(volatile unsigned char *)0x50000020)
#define	URXH0		 		(*(volatile unsigned char *)0x50000024)
#define	UBRDIV0				(*(volatile unsigned long *)0x50000028)

/*interrupt registes*/
#define	SRCPND				(*(volatile unsigned long *)0x4A000000)
#define	INTMOD				(*(volatile unsigned long *)0x4A000004)
#define	INTMSK				(*(volatile unsigned long *)0x4A000008)
#define	PRIORITY			(*(volatile unsigned long *)0x4A00000c)
#define	INTPND				(*(volatile unsigned long *)0x4A000010)
#define	INTOFFSET			(*(volatile unsigned long *)0x4A000014)
#define	SUBSRCPND			(*(volatile unsigned long *)0x4A000018)
#define	INTSUBMSK			(*(volatile unsigned long *)0x4A00001c)

/*external interrupt registers*/
#define	EINTMASK			(*(volatile unsigned long *)0x560000a4)
#define	EINTPEND			(*(volatile unsigned long *)0x560000a8)
#define EXTINT0				(*(volatile unsigned long *)0x56000088)
#define EXTINT1				(*(volatile unsigned long *)0x5600008C)
#define EXTINT2				(*(volatile unsigned long *)0x56000090)

/*clock registers*/
#define	LOCKTIME			(*(volatile unsigned long *)0x4c000000)
#define	MPLLCON				(*(volatile unsigned long *)0x4c000004)
#define	UPLLCON				(*(volatile unsigned long *)0x4c000008)
#define	CLKCON				(*(volatile unsigned long *)0x4c00000c)
#define	CLKSLOW				(*(volatile unsigned long *)0x4c000010)
#define	CLKDIVN				(*(volatile unsigned long *)0x4c000014)


/*PWM & Timer registers*/
#define	TCFG0				(*(volatile unsigned long *)0x51000000)
#define	TCFG1				(*(volatile unsigned long *)0x51000004)
#define	TCON				(*(volatile unsigned long *)0x51000008)
#define	TCNTB0				(*(volatile unsigned long *)0x5100000c)
#define	TCMPB0				(*(volatile unsigned long *)0x51000010)
#define	TCNTO0				(*(volatile unsigned long *)0x51000014)

#endif
