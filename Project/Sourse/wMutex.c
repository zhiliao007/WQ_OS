/*
 * @file wMutex.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   互斥信号量
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "WQ_OS.h"

#if WQ_OS_ENABLE_MUTEX == 1
/*!
 * @brief  初始化互斥信号量函数
 * @param  mutex   互斥信号量结构指针
 * @retval 无
 */	
void wMutexInit(wMutex * mutex)
{
	wEventInit(&mutex->event, wEventTypeMutex);
	
	mutex->lockedCount = 0;
	mutex->owner = (wTask *)0;
	/* 配置为无效值，因为优先级为0~31 */
	mutex->ownerOriginalPrio = WQ_OS_PRO_COUNT;  //配置为无效值，因为优先级为0~31
}

/*!
 * @brief  等待互斥信号量函数
 * @param  mutex       互斥信号量结构指针
 * @param  waitTicks   等待的最大ticks数
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
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

/*!
 * @brief  获取互斥信号量函数
 * @param  mutex       互斥信号量结构指针
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
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

/*!
 * @brief  互斥信号量唤醒任务函数
 * @param  mutex       互斥信号量结构指针
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
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

/*!
 * @brief  删除互斥信号量函数
 * @param  mutex       互斥信号量结构指针
 * @retval 因销毁该信号量而唤醒的任务数量
 */	
uint32_t wMutexDestroy(wMutex * mutex)
{
	uint32_t count = 0;
	uint32_t status = wTaskEnterCritical();
	
	/* 信号量是否已经被锁定，未锁定时没有任务等待，不必处理 */
	if(mutex->lockedCount > 0)
	{
		/* 是否有发生优先级继承?如果发生，需要恢复拥有者的原优先级 */
		if(mutex->ownerOriginalPrio != mutex->owner->prio)
		{
			if(mutex->owner->stack == WQOS_TASK_STATE_RDY)
			{
				/* 如果任务处于就绪状态时，更改任务在就绪表中的位置 */
				wTaskSchedUnRdy(mutex->owner);
				mutex->owner->prio = mutex->ownerOriginalPrio;
				wTaskSchedRdy(mutex->owner);
			}
			else
			{
				/* 其它状态，只需要修改优先级 */
				mutex->owner->prio = mutex->ownerOriginalPrio;
			}
		}
		
		count = wEventRemoveAll(&mutex->event, (void *)0,wErrorDel);
		if(count > 0)
		{
			wTaskSched();
		}
	}
	
	wTaskExitCritical(status);
	return count;
}

/*!
 * @brief  互斥信号量状态查询函数
 * @param  mutex       互斥信号量结构指针
 * @param  info        状态查询结构指针
 * @retval 无
 */	
void wMutexGetInfo(wMutex * mutex, wMutexInfo * info)
{
	uint32_t status = wTaskEnterCritical();
	
	info->taskCount = wEventWaitCount(&mutex->event);
	info->ownerPrio = mutex->ownerOriginalPrio;
	if(mutex->owner != (wTask *)0)
	{
		info->inheritedPrio = mutex->owner->prio;
	}
	else
	{
		info->inheritedPrio = WQ_OS_PRO_COUNT;
	}
	info->owner = mutex->owner;
	info->lockedCount = mutex->lockedCount;
	
	wTaskExitCritical(status);
}

#endif
