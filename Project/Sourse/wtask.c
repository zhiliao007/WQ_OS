#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  �����ʼ������
  * @param  task�� ����ṹ���ַ
		    entry��������ں�����
		    param�����ݸ�����Ĳ���
            stack������ջ�ռ��ַ
  * @retval ��
  ******************************************************************************************************************/	
void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack)
{
	*(--stack) = (unsigned long)(1<<24);                // XPSR, ������Thumbģʽ���ָ���Thumb״̬����ARM״̬����
    *(--stack) = (unsigned long)entry;                  // R15(PC) �������ڵ�ַ
    *(--stack) = (unsigned long)0x14;                   // R14(LR), ���񲻻�ͨ��return xxx�����Լ�������δ��
														// R14�ں˼Ĵ����������溯���ķ���ֵ��ַ
    *(--stack) = (unsigned long)0x12;                   // R12, δ��
    *(--stack) = (unsigned long)0x3;                    // R3,  δ��
    *(--stack) = (unsigned long)0x2;                    // R2,  δ��
    *(--stack) = (unsigned long)0x1;                    // R1,  δ��
    *(--stack) = (unsigned long)param;                  // R0 = param, �����������ں���
	                                                    // �����ĵ�һ�������ĵ�ַ������R0�ں˼Ĵ���
	
    *(--stack) = (unsigned long)0x11;                   // R11, δ��
    *(--stack) = (unsigned long)0x10;                   // R10, δ��
    *(--stack) = (unsigned long)0x9;                    // R9,  δ��
    *(--stack) = (unsigned long)0x8;                    // R8,  δ��
    *(--stack) = (unsigned long)0x7;                    // R7,  δ��
    *(--stack) = (unsigned long)0x6;                    // R6,  δ��
    *(--stack) = (unsigned long)0x5;                    // R5,  δ��
    *(--stack) = (unsigned long)0x4;                    // R4,  δ��
	
	task->slice = WQ_OS_SLICE_MAX; 
	task->stack = stack;								// �������յ�ֵ��ʹ����ջָ��ָ�򻺳���ջ��
	task->delayTicks = 0;
	task->prio = prio;
	task->state = WQOS_TASK_STATE_RDY;
	task->suspendCount = 0;
	task->clean = (void(*)(void *))0;
	task->cleanparam = (void *)0;
	task->requestDeleteFlag = 0;
	
	wNodeInit(&(task->delayNode));
	wNodeInit(&(task->linkNode));
	wTaskSchedRdy(task);
}

/*******************************************************************************************************************
  * @brief  ���������
  * @param  task   ����ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wTaskSuspend(wTask * task)
{
	uint32_t status = wTaskEnterCritical();
	
	if(!(task->state & WQOS_TASK_STATE_DELAYED))
	{
		if(++task->suspendCount <= 1)
		{
			task->state |= WQOS_TASK_STATE_SUSPEND;
			wTaskSchedUnRdy(task);
			if(task == currentTask)
			{
				wTaskSched();
			}
		}
	}
	wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  �ָ���������
  * @param  task   ����ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wTaskWakeUp(wTask * task)
{
	uint32_t status = wTaskEnterCritical();
	
	  if (task->state & WQOS_TASK_STATE_SUSPEND)
      {
		if(--task->suspendCount == 0)
		{
			task->state &= ~WQOS_TASK_STATE_SUSPEND;
			wTaskSchedRdy(task);
			wTaskSched();
	    } 
}
	wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  ����ɾ��ʱ���õ�������
  * @param  task   ����ṹָ��
			clean  ��������ڵ�ַ
			param  ���ݸ��������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void wTaskSetCleanCallFunc(wTask * task, void(*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanparam = param;
}	

/*******************************************************************************************************************
  * @brief  ����ǿ��ɾ������
  * @param  task   ����ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wTaskForceDelete(wTask * task) 
{
    uint32_t status = wTaskEnterCritical();

    if (task->state & WQOS_TASK_STATE_DELAYED)            // �����������ʱ״̬�������ʱ������ɾ��
    {
        wTimeTaskRemove(task);
    }
    else if (!(task->state & WQOS_TASK_STATE_SUSPEND))   // ������񲻴��ڹ���״̬����ô���Ǿ���̬���Ӿ�������ɾ��
    {
        wTaskSchedRemove(task);
    }

    if (task->clean)               // ɾ��ʱ������������������������������
    {
        task->clean(task->cleanparam);
    }
    
    if (currentTask == task)      // ���ɾ�������Լ�����ô��Ҫ�л�����һ����������ִ��һ���������
    {
        wTaskSched();
    }
	
	currentTask->state |= WQOS_TASK_STATE_DESTORYED;

    wTaskExitCritical(status); 
}

/*******************************************************************************************************************
  * @brief  ��������ɾ������
  * @param  task   ����ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wTaskRequestDelete (wTask * task)
{
    uint32_t status = wTaskEnterCritical();

    task->requestDeleteFlag = 1;
	
    wTaskExitCritical(status); 
}
/*******************************************************************************************************************
  * @brief  ��������Ƿ�����ɾ������
  * @param  ��
  * @retval ����ɾ����־ ��0��ʾ����ɾ����0��ʾ������
  ******************************************************************************************************************/
uint8_t wTaskIsRequestedDelete (void)
{
    uint8_t delete;

    uint32_t status = wTaskEnterCritical();

    delete = currentTask->requestDeleteFlag;

    wTaskExitCritical(status); 

    return delete;
}

/*******************************************************************************************************************
  * @brief  ������ɾ������
  * @param  ��
  * @retval ��
  ******************************************************************************************************************/
void wTaskDeleteSelf (void)
{
    uint32_t status = wTaskEnterCritical();

    wTaskSchedRemove(currentTask);

    if (currentTask->clean)        // ɾ��ʱ������������������������������
    {
        currentTask->clean(currentTask->cleanparam);
    }

    wTaskSched();

	currentTask->state |= WQOS_TASK_STATE_DESTORYED;
	
    wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  ����״̬��ѯ����
  * @param  task   ����ṹָ��
  * @retval info   ״̬��ѯ�ṹָ��
  ******************************************************************************************************************/
void wTaskGetInfo(wTask * task, wTaskInfo * info)
{
	uint32_t status = wTaskEnterCritical();
	
	info->delayTicks = task->delayTicks;
	info->prio = task->prio;
	info->state = task->state;
	info->slice = task->slice;
	info->suspendCount = task->suspendCount;
	  
	wTaskExitCritical(status);
}
