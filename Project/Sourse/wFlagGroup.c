#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化事件标志组函数
  * @param  flagGroup   事件标志组结构指针
			flags       事件标志
  * @retval 无
  ******************************************************************************************************************/	
void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags)
{
	wEventInit(&flagGroup->event, wEventTypeFlagGroup);
	flagGroup->flag = flags;
}
