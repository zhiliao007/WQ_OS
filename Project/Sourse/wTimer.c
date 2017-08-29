#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化软件定时器函数
  * @param  timer 等待初始化的定时器结构指针
			delayTicks 定时器初始启动的延时ticks数。
			durationTicks 给周期性定时器用的周期tick数，一次性定时器无效
			timerFunc 定时器回调函数
			arg 传递给定时器回调函数的参数
			timerFunc 定时器回调函数
			config 定时器的初始配置
  * @retval 无
  ******************************************************************************************************************/	
void wTimerInit(wTimer * timer, uint32_t delayTicks, uint32_t durationTicks, void(*timerFunc) (void * arg), void * arg, uint32_t config)
{
	wNodeInit(&timer->linkNode);
	timer->startDelayTicks = delayTicks;
	timer->durationTicks = durationTicks;
	timer->timerFunc = timerFunc;
	timer->arg = arg;
	timer->config = config;
	
	if(delayTicks == 0)
	{
		timer->delayTicks = durationTicks;
	}
	else
	{
		timer->delayTicks = timer->startDelayTicks;
	}
	
	timer->state = timerCreated;
}
