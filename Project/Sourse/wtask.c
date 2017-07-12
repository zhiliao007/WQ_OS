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
