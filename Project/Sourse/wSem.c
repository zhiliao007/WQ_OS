#include "wSem.h"

/*******************************************************************************************************************
  * @brief  初始化信号量函数
  * @param  sem         信号量结果指针
			startCount  初始计数值
			maxCount    最大计数值，若为0则不限数量 
  * @retval 无
  ******************************************************************************************************************/	
void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount) 
{
	wEventInit(&sem->event, wEventTypeSem);

	sem->maxCount = maxCount;
	if (maxCount == 0)
	{
		sem->count = startCount;
	} 
	else 
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}
