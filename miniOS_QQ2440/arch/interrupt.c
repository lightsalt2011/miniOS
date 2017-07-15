#include "s3c2440.h"
#include "serial.h"
#include "interrupt.h"
#include "sched.h"
#include "key.h"

void handle_irq()
{
    unsigned long irqOffSet = SFR_VA(INTOFFSET);
    switch(irqOffSet)
    {
		case K1_IRQ_OFT:
		    printk("KEY 1 to create new Task\r\n");
		    if(OSCreateProcess(NOR_1M_VM+0x10000, 4*KB, "AAAAAAAAAAAAAAAAA", 5) == -1)
				DPRINTK(KERNEL_DEBUG,"Process Create fault!!\r\n");
		    break;

		case K2_IRQ_OFT:
		    printk("KEY 2 to kill a random Task\r\n");
		    kill_task(0);
		    break;

		case K3_K4_IRQ_OFT:
		    if(K3_EINT_BIT & SFR_VA(EINTPEND)) {
				SFR_VA(EINTPEND) &= K3_EINT_BIT;
				printk("KEY 3 pressed\r\n");
		    } else {
				SFR_VA(EINTPEND) &= K4_EINT_BIT;
				printk("KEY 4 pressed\r\n");
				if(OSCreateProcess(NOR_1M_VM+0x20000, 4*KB, NULL, 5) == -1)
					DPRINTK(KERNEL_DEBUG,"Process Create fault!!\r\n");
		    }
		    break;
		case TIMER_OFT:
		    do_timer();
		    break;
		default:
			DPRINTK(KERNEL_DEBUG,"Unknown Interrupt.\r\n");
    }
    
    SFR_VA(SRCPND) &= (1<<irqOffSet);	
    SFR_VA(INTPND) = SFR_VA(INTPND);		 
}
