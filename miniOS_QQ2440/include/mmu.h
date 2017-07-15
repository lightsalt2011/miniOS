#include 	"s3c2440.h"

/*以下为虚拟地址*/ 
#define	MMU_TABLE_BASE	    (KERNEL_LIMIT - 0x10000)
#define	PROCESS0_BASE	    (SDRAM_BASE+0x4000)
#define	VECTORS_BASE	    0xffff0000
//#define	VECTORS_PHY_BASE    (SDRAM_BASE+SDRAM_SIZE - 0x100000 - 0xf0000)
#define	VECTORS_PHY_BASE    (SDRAM_BASE+SDRAM_SIZE - 0x100000)
void pgtb_init(void);
void mmu_init(void);
