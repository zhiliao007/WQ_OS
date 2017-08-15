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

/*******************************************************************************************************************
  * @brief  等待互斥信号量函数
  * @param  mutex       互斥信号量结构指针
			waitTicks   等待的最大ticks数
  * @retval 等待结果    wErrorResourceUnavaliable    资源不可用
                        wErrorNoError                没有错误
                        wErrorTimeout                等待超时
  ******************************************************************************************************************/	
uint32_t wMutexWait(wMutex * mutex, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mutex->lockedCount <= 0)
	{
		mutex->owner = currentTask;
		mutex->ownerOriginalPrio = currentTask->prio;
		mutex->lockedCount++;
		
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else
	{
		if(mutex->owner == currentTask)
		{
			mutex->lockedCount++;
			
			wTaskExitCritical(status);
			return wErrorNoError;
		}
		else
		{
			if(currentTask->prio < mutex->owner->prio)
			{
				wTask * owner = mutex->owner;
				if(owner->state == WQOS_TASK_STATE_RDY)
				{
					wTaskSchedUnRdy(owner);
					owner->prio = currentTask->prio;
					wTaskSchedRdy(owner);
				}
				else
				{
					owner->prio = currentTask->prio;
				}
			}
			wEventWait(&mutex->event, currentTask, (void *)0, wEventTypeMutex, waitTicks);
			wTaskExitCritical(status);
			
			wTaskSched();
			return currentTask->waitEventResult;
		}
	}
}

/*******************************************************************************************************************
  * @brief  获取互斥信号量函数
  * @param  mutex       互斥信号量结构指针
  * @retval 等待结果    wErrorResourceUnavaliable    资源不可用
                        wErrorNoError                没有错误
                        wErrorTimeout                等待超时
  ******************************************************************************************************************/	
uint32_t wMutexNoWaitGet(wMutex * mutex)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mutex->lockedCount <= 0)
	{
		mutex->owner = currentTask;
		mutex->ownerOriginalPrio = currentTask->prio;
		mutex->lockedCount++;
		
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else
	{
		if(mutex->owner == currentTask)
		{
			mutex->lockedCount++;
			wTaskExitCritical(status);
			return wErrorNoError;
		}
		
		wTaskExitCritical(status);
		return wErrorResourceUnavaliable;
	}
}

/*******************************************************************************************************************
  * @brief  互斥信号量唤醒任务函数
  * @param  mutex       互斥信号量结构指针
  * @retval 等待结果    wErrorResourceUnavaliable    资源不可用
                        wErrorNoError                没有错误
                        wErrorTimeout                等待超时
  ******************************************************************************************************************/	
uint32_t wMutexNotify(wMutex * mutex)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mutex->lockedCount <= 0)
	{
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	
	if(mutex->owner != currentTask)
	{
		wTaskExitCritical(status);
		return wErrorOwner;
	}
	
	if(--mutex->lockedCount > 0)
	{
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	
	if(mutex->ownerOriginalPrio != mutex->owner->prio)
	{
		if(mutex->owner->state == WQOS_TASK_STATE_RDY)
		{
			wTaskSchedUnRdy(mutex->owner);
			currentTask->prio = mutex->ownerOriginalPrio;
			wTaskSchedRdy(mutex->owner);
		}
		else
		{
			currentTask->prio = mutex->ownerOriginalPrio;
		}
	}
	
	if(wEventWaitCount(&mutex->event) > 0)
	{
		wTask * task = wEventWakeUp(&mutex->event, (void *)0, wErrorNoError);
		
		mutex->owner = task;
		mutex->ownerOriginalPrio = task->prio;
		mutex->lockedCount++;
		
		if(task->prio < currentTask->prio)
		{
			wTaskSched();
		}
	}
	wTaskExitCritical(status);
	return wErrorNoError;
}
