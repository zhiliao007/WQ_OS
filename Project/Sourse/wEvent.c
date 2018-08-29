/*
 * @file wEvent.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   事件控制块
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
 * @brief  初始化事件控制块函数
 * @param  event 事件控制块指针
 * @param  type  事件控制块的类型
 * @retval 无
 */
void wEventInit(wEvent * event,wEventType type)
{
	event->type = wEventTypeUnknown;
	wListInit(&event->waitList);
}

/*!
 * @brief  将等待事件发生的任务插入事件控制块函数
 * @param  event   事件控制块指针
 * @param  task    等待事件发生的任务
 * @param  msg     事件消息存储的具体位置
 * @param  state   事件状态
 * @param  timeout 等待时间
 * @retval 无
 */
void wEventWait(wEvent * event, wTask * task, void * msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = wTaskEnterCritical();

  task->state |= state << 16;        
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

/*!
 * @brief  将首个任务从事件控制块中唤醒函数
 * @param  event 事件控制块指针
 * @param  msg   事件消息
 * @param  result 事件等待结果
 * @retval 首个等待的任务，若无任务则返回0
 */
wTask * wEventWakeUp(wEvent * event, void * msg, uint32_t result)
{
    wNode  * node;
    wTask  * task = (wTask * )0;
    
    uint32_t status = wTaskEnterCritical();

    if((node = wListRemoveFirst(&event->waitList)) != (wNode *)0)
    {    
		/* 转换为相应的任务结构 */
        task = (wTask *)wNodeParent(node, wTask, linkNode);   

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

/*!
 * @brief  将特定任务从事件控制块中唤醒函数
 * @param  event 事件控制块指针
 * @param  task  指定的任务结构指针
 * @param  msg   事件消息
 * @param  result  事件等待结果
 * @retval 首个等待的任务，若无任务则返回0
 */
wTask * wEventWakeUpTask(wEvent * event, wTask * task, void * msg, uint32_t result)
{
    uint32_t status = wTaskEnterCritical();
	
	wListRemove(&event->waitList, &task->linkNode);
	task->waitEvent = (wEvent *)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~WQOS_TASK_WAIT_MASK;

    if(task->delayTicks != 0)
    { 
		wTimeTaskWakeUp(task);
    }

    wTaskSchedRdy(task);         

    wTaskExitCritical(status); 
   
    return task;         
}

/*!
 * @brief  将任务强制从等待队列中删除函数
 * @param  task 将要移除的任务
 * @param  msg  事件消息
 * @param  result 事件等待结果
 * @retval 无
 */
void wEventRemoveTask(wTask * task, void * msg, uint32_t result)
{     
    uint32_t status = wTaskEnterCritical();

	wListRemove(&task->waitEvent->waitList, &task->linkNode);

    task->waitEvent = (wEvent *)0;
    task->eventMsg = msg;
   	task->waitEventResult = result;
	 task->state &= ~WQOS_TASK_WAIT_MASK;

    wTaskExitCritical(status); 
}

/*!
 * @brief  初始化事件控制块函数
 * @param  event 事件控制块指针
 * @param  msg 事件消息
 * @param  result 事件等待结果
 * @retval 唤醒的任务数量
 */
uint32_t wEventRemoveAll(wEvent * event, void * msg, uint32_t result)
{
    wNode  * node;
    uint32_t count;
    
    uint32_t status = wTaskEnterCritical();

    count = wListCount(&event->waitList);

    while ((node = wListRemoveFirst(&event->waitList)) != (wNode *)0)
    {    
		/* 转换为相应的任务结构 */
        wTask * task = (wTask *)wNodeParent(node, wTask, linkNode);    
   
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

    return  count;
}

/*!
 * @brief  查询事件控制块中等待的任务数量函数
 * @param  event 事件控制块指针
 * @retval 唤醒的任务数量
 */
uint32_t wEventWaitCount(wEvent * event)
{  
    uint32_t count = 0;

    uint32_t status = wTaskEnterCritical();

    count = wListCount(&event->waitList);  

    wTaskExitCritical(status);     

    return count;
}  
