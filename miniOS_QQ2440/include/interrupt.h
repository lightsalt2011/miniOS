#ifndef INTERRUPT_H
#define INTERRUPT_H

#define TIMER_OFT       10

void handle_irq(void);
extern __inline void OS_ENTER_CRITICAL(){
	__asm{					
		mrs		r0,	CPSR
        orr		r0, r0,	#0x80	
        msr		CPSR_c,	r0
     }
}

extern __inline void OS_EXIT_CRITICAL(){
	__asm{
		mrs		r0,	CPSR	
		tst		r0, #0x80
		biceq	r0, r0,	#0x80	
		msr		CPSR_c, r0
     }
}

extern __inline void ENTER_USR_MODE(){
	__asm{
	    mov r0,	#0x50
	    msr CPSR_c, r0
	}
}

#endif