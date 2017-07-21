#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ���¼����ƿ麯��
  * @param  event �¼����ƿ�ָ��
			type �¼����ƿ������
  * @retval ��
  ******************************************************************************************************************/
void wEventInit(wEvent * event,wEventType type)
{
	event->type = wEventTypeUnknown;
	wListInit(&event->waitList);
}

/*******************************************************************************************************************
  * @brief  ���ȴ��¼���������������¼����ƿ麯��
  * @param  event   �¼����ƿ�ָ��
			task    �ȴ��¼�����������
			msg     �¼���Ϣ�洢�ľ���λ��
            state   �¼�״̬
            timeout �ȴ�ʱ��
  * @retval ��
  ******************************************************************************************************************/
void wEventWait (wEvent * event, wTask * task, void * msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = wTaskEnterCritical();

  task->state |= state;        
  task->waitEvent = event;        
  task->eventMsg = msg;         
  task->waitEventResult = wErrorNoError;  

    wTaskSchedUnRdy(task);

  wListAddLast(&event->waitList, &task->linkNode);

  if (timeout) 
  {
    wTimeTaskWait(task, timeout);
  }

    wTaskExitCritical(status); 
}

/*******************************************************************************************************************
  * @brief  ���׸�������¼����ƿ��л��Ѻ���
  * @param  event �¼����ƿ�ָ��
			msg   �¼���Ϣ
			result �¼��ȴ����
  * @retval �׸��ȴ����������������򷵻�0
  ******************************************************************************************************************/
wTask * wEventWakeUp (wEvent * event, void * msg, uint32_t result)
{
    wNode  * node;
    wTask  * task = (wTask * )0;
    
    uint32_t status = wTaskEnterCritical();

    if((node = wListRemoveFirst(&event->waitList)) != (wNode *)0)
    {                                                          
        task = (wTask *)wNodeParent(node, wTask, linkNode);   // ת��Ϊ��Ӧ������ṹ

        task->waitEvent = (wEvent *)0;
        task->eventMsg = msg;
        task->waitEventResult = result;
        task->state &= ~WQOS_TASK_WAIT_MASK;

        if (task->delayTicks != 0)
        { 
            wTimeTaskWakeUp(task);
        }

        wTaskSchedRdy(task);        
    }  

    wTaskExitCritical(status); 
   
    return task;         
}

/*******************************************************************************************************************
  * @brief  ������ǿ�ƴӵȴ�������ɾ������
  * @param  task ��Ҫ�Ƴ�������
			msg  �¼���Ϣ
			result �¼��ȴ����
  * @retval ��
  ******************************************************************************************************************/
void wEventRemoveTask (wTask * task, void * msg, uint32_t result)
{     
    uint32_t status = wTaskEnterCritical();

	wListRemove(&task->waitEvent->waitList, &task->linkNode);

    task->waitEvent = (wEvent *)0;
    task->eventMsg = msg;
   	task->waitEventResult = result;
	 task->state &= ~WQOS_TASK_WAIT_MASK;

    wTaskExitCritical(status); 
}

