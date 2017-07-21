#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化事件控制块函数
  * @param  event 事件控制块指针
			type 事件控制块的类型
  * @retval 无
  ******************************************************************************************************************/
void wEventInit(wEvent * event,wEventType type)
{
	event->type = wEventTypeUnknown;
	wListInit(&event->waitList);
}

/*******************************************************************************************************************
  * @brief  将等待事件发生的任务插入事件控制块函数
  * @param  event   事件控制块指针
			task    等待事件发生的任务
			msg     事件消息存储的具体位置
            state   事件状态
            timeout 等待时间
  * @retval 无
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
  * @brief  将首个任务从事件控制块中唤醒函数
  * @param  event 事件控制块指针
			msg   事件消息
			result 事件等待结果
  * @retval 首个等待的任务，若无任务则返回0
  ******************************************************************************************************************/
wTask * wEventWakeUp (wEvent * event, void * msg, uint32_t result)
{
    wNode  * node;
    wTask  * task = (wTask * )0;
    
    uint32_t status = wTaskEnterCritical();

    if((node = wListRemoveFirst(&event->waitList)) != (wNode *)0)
    {                                                          
        task = (wTask *)wNodeParent(node, wTask, linkNode);   // 转换为相应的任务结构

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
  * @brief  将任务强制从等待队列中删除函数
  * @param  task 将要移除的任务
			msg  事件消息
			result 事件等待结果
  * @retval 无
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

