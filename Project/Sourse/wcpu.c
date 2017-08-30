#include "WQ_OS.h"
#include <ARMCM3.h>

/*******************************************************************************************************************
  * @brief  SysTick初始化函数
  * @param  ms： 定时时间
  * @retval 无
  ******************************************************************************************************************/
void wSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}

/*******************************************************************************************************************
  * @brief  SysTick中断服务函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void SysTick_Handler()
{
	wTaskSystemTickHandler();
}
