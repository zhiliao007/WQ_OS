/*
 * @file switch.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   实现PendSV异常处理
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 

#include "WQ_OS.h"
#include "ARMCM3.h"

/*******************************************************************************
 * 宏定义
 ******************************************************************************/
/*! 相关寄存器 */
#define NVIC_INT_CTRL          0xE000ED04
#define NVIC_PENDSVSET         0x10000000
#define NVIC_SYSPRI2           0xE000ED22
#define NVIC_PENDSV_PRT        0x000000FF

#define MEM32(addr)            *(volatile unsigned long *)(addr)
#define MEM8(addr)             *(volatile unsigned char *)(addr)

/*!
 * @brief  进入临界区函数
 * @param  无
 * @retval 0 中断屏蔽
 * @retval 1 中断开启
  */
uint32_t wTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

/*!
 * @brief  退出临界区函数
 * @param  status  中断的状态，由进入临界区函数返回
 * @retval 无
 */
void wTaskExitCritical(uint32_t status)
{
	__set_PRIMASK(status);
}

/*!
 * @brief  PendSV异常处理函数
 * @param  无
 * @retval 无
 */
__asm void PendSV_Handler(void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP
	CBZ R0, PendSVHandler_nosave
	
	STMDB R0!, {R4-R11}
	
	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]
	
PendSVHandler_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}
	
	MSR PSP, R0
	ORR LR, LR, #0x04
	BX  LR
}

/*!
 * @brief  系统开始运行函数
 * @param  无
 * @retval 无
 */	
void wTaskRunFirst()
{
	/* PSP标志置0 */
	__set_PSP(0);                               
	
	/* 设置PendSV优先级 */
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT;
    /*触发PendSV异常 */	
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;	  
}

/*!
 * @brief  任务切换函数
 * @param  无
 * @retval 无
 */	
void wTaskSwitch()
{
	/* 触发PendSV异常 */
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;	 
}
