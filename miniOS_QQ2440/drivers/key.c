#include "s3c2440.h"
#include "serial.h"
#include "key.h"

/*************************************************************************
* 按键初始化代码
*************************************************************************/
void key_init(void)
{
    SFR_VA(GPFCON) = (2 << 0) | (2 << 4);
    SFR_VA(GPGCON) = (2<<6) | (2<<22);
    SFR_VA(EXTINT0) = (3<<8) | (3<<0);
    SFR_VA(EXTINT1) = (3<<12);
    SFR_VA(EXTINT2) = (3<<12);
    DPRINTK(KERNEL_DEBUG, "key_init OK\r\n"); 
}
   
