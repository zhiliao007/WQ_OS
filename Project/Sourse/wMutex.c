#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化互斥信号量函数
  * @param  mutex   互斥信号量结构指针
  * @retval 无
  ******************************************************************************************************************/	
void wMutexInit(wMutex * mutex)
{
	wEventInit(&mutex->event, wEventTypeMutex);
	
	mutex->lockedCount = 0;
	mutex->owner = (wTask *)0;
	mutex->ownerOriginalPrio = WQ_OS_PRO_COUNT;  //配置为无效值，因为优先级为0~31
}
