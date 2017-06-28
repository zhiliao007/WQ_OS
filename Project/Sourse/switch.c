#include "WQ_OS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL          0xE000ED04
#define NVIC_PENDSVSET         0x10000000
#define NVIC_SYSPRI2           0xE000ED22
#define NVIC_PENDSV_PRT        0x000000FF

#define MEM32(addr)            *(volatile unsigned long *)(addr)
#define MEM8(addr)             *(volatile unsigned char *)(addr)

__asm void PendSV_Handler(void)
{
	
}

void wTaskRunFirst()
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void wTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
