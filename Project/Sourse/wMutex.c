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

/*******************************************************************************************************************
  * @brief  ɾ�������ź�������
  * @param  mutex       �����ź����ṹָ��
  * @retval �����ٸ��ź��������ѵ���������
  ******************************************************************************************************************/	
uint32_t wMutexDestroy(wMutex * mutex)
{
	uint32_t count = 0;
	uint32_t status = wTaskEnterCritical();
	
	// �ź����Ƿ��Ѿ���������δ����ʱû������ȴ������ش���
	if(mutex->lockedCount > 0)
	{
		//�Ƿ��з������ȼ��̳�?�����������Ҫ�ָ�ӵ���ߵ�ԭ���ȼ�
		if(mutex->ownerOriginalPrio != mutex->owner->prio)
		{
			if(mutex->owner->stack == WQOS_TASK_STATE_RDY)
			{
				// ��������ھ���״̬ʱ�����������ھ������е�λ��
				wTaskSchedUnRdy(mutex->owner);
				mutex->owner->prio = mutex->ownerOriginalPrio;
				wTaskSchedRdy(mutex->owner);
			}
			else
			{
				// ����״̬��ֻ��Ҫ�޸����ȼ�
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

/*******************************************************************************************************************
  * @brief  �����ź���״̬��ѯ����
  * @param  mutex       �����ź����ṹָ��
			info        ״̬��ѯ�ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
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
