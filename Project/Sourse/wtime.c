#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ������ʱ����
  * @param  delay�� ������ʱʱ��(x10ms)
  * @retval ��
  ******************************************************************************************************************/
void wTaskDelay(uint32_t delay)
{
	uint32_t status = wTaskEnterCritical();
	
	wTimeTaskWait(currentTask, delay);
	
	wTaskSchedUnRdy(currentTask);
	
	wTaskExitCritical(status);
	
	wTaskSched();
}
