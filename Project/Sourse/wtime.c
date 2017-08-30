#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  任务延时函数
  * @param  delay： 任务延时时间(x10ms)
  * @retval 无
  ******************************************************************************************************************/
void wTaskDelay(uint32_t delay)
{
	uint32_t status = wTaskEnterCritical();
	
	wTimeTaskWait(currentTask, delay);
	
	wTaskSchedUnRdy(currentTask);
	
	wTaskExitCritical(status);
	
	wTaskSched();
}
