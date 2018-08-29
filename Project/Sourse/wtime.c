/*
 * @file wtime.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   延时
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 
 
#include "WQ_OS.h"

/*!
 * @brief  任务延时函数
 * @param  delay： 任务延时时间(x10ms)
 * @retval 无
 */
void wTaskDelay(uint32_t delay)
{
	uint32_t status = wTaskEnterCritical();
	
	wTimeTaskWait(currentTask, delay);
	
	wTaskSchedUnRdy(currentTask);
	
	wTaskExitCritical(status);
	
	wTaskSched();
}
