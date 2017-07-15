#include "serial.h"
#include "s3c2440.h"

void irq_init(void)
{
    SFR_VA(INTMSK) = 0xffffffff;
    SFR_VA(EINTMASK) = 0xffffffff;
    SFR_VA(INTMSK) &= ~((1<<10) | (1<< 5) | (1 << 2) | (1 << 0));
    SFR_VA(EINTMASK) &= ~((1 << 11) | (1 << 19));
    DPRINTK(KERNEL_DEBUG, "Interrupt init OK\r\n");
}