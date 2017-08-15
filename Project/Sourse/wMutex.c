#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ�������ź�������
  * @param  mutex   �����ź����ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
void wMutexInit(wMutex * mutex)
{
	wEventInit(&mutex->event, wEventTypeMutex);
	
	mutex->lockedCount = 0;
	mutex->owner = (wTask *)0;
	mutex->ownerOriginalPrio = WQ_OS_PRO_COUNT;  //����Ϊ��Чֵ����Ϊ���ȼ�Ϊ0~31
}

/*******************************************************************************************************************
  * @brief  �ȴ������ź�������
  * @param  mutex       �����ź����ṹָ��
			waitTicks   �ȴ������ticks��
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
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
  * @brief  ��ȡ�����ź�������
  * @param  mutex       �����ź����ṹָ��
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
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
  * @brief  �����ź�������������
  * @param  mutex       �����ź����ṹָ��
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
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
