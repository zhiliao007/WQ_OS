#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  任务初始化函数
  * @param  task： 任务结构体地址
		    entry：任务入口函数名
		    param：传递给任务的参数
            stack：任务栈空间地址
  * @retval 无
  ******************************************************************************************************************/	
void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack)
{
	*(--stack) = (unsigned long)(1<<24);                // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stack) = (unsigned long)entry;                  // R15(PC) 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
														// R14内核寄存器用来保存函数的返回值地址
    *(--stack) = (unsigned long)0x12;                   // R12, 未用
    *(--stack) = (unsigned long)0x3;                    // R3,  未用
    *(--stack) = (unsigned long)0x2;                    // R2,  未用
    *(--stack) = (unsigned long)0x1;                    // R1,  未用
    *(--stack) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
	                                                    // 函数的第一个参数的地址保存在R0内核寄存器
	
    *(--stack) = (unsigned long)0x11;                   // R11, 未用
    *(--stack) = (unsigned long)0x10;                   // R10, 未用
    *(--stack) = (unsigned long)0x9;                    // R9,  未用
    *(--stack) = (unsigned long)0x8;                    // R8,  未用
    *(--stack) = (unsigned long)0x7;                    // R7,  未用
    *(--stack) = (unsigned long)0x6;                    // R6,  未用
    *(--stack) = (unsigned long)0x5;                    // R5,  未用
    *(--stack) = (unsigned long)0x4;                    // R4,  未用
	
	task->slice = WQ_OS_SLICE_MAX; 
	task->stack = stack;								// 保存最终的值，使任务栈指针指向缓冲区栈顶
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
  * @brief  任务挂起函数
  * @param  task   任务结构指针
  * @retval 无
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
  * @brief  恢复被挂起函数
  * @param  task   任务结构指针
  * @retval 无
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
  * @brief  任务被删除时调用的清理函数
  * @param  task   任务结构指针
			clean  清理函数入口地址
			param  传递给清理函数的参数
  * @retval 无
  ******************************************************************************************************************/
void wTaskSetCleanCallFunc(wTask * task, void(*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanparam = param;
}	

/*******************************************************************************************************************
  * @brief  任务强制删除函数
  * @param  task   任务结构指针
  * @retval 无
  ******************************************************************************************************************/
void wTaskForceDelete(wTask * task) 
{
    uint32_t status = wTaskEnterCritical();

    if (task->state & WQOS_TASK_STATE_DELAYED)            // 如果任务处于延时状态，则从延时队列中删除
    {
        wTimeTaskRemove(task);
    }
    else if (!(task->state & WQOS_TASK_STATE_SUSPEND))   // 如果任务不处于挂起状态，那么就是就绪态，从就绪表中删除
    {
        wTaskSchedRemove(task);
    }

    if (task->clean)               // 删除时，如果有设置清理函数，则调用清理函数
    {
        task->clean(task->cleanparam);
    }
    
    if (currentTask == task)      // 如果删除的是自己，那么需要切换至另一个任务，所以执行一次任务调度
    {
        wTaskSched();
    }
	
	currentTask->state |= WQOS_TASK_STATE_DESTORYED;

    wTaskExitCritical(status); 
}

/*******************************************************************************************************************
  * @brief  任务请求删除函数
  * @param  task   任务结构指针
  * @retval 无
  ******************************************************************************************************************/
void wTaskRequestDelete (wTask * task)
{
    uint32_t status = wTaskEnterCritical();

    task->requestDeleteFlag = 1;
	
    wTaskExitCritical(status); 
}
/*******************************************************************************************************************
  * @brief  检查任务是否请求删除函数
  * @param  无
  * @retval 请求删除标志 非0表示请求删除，0表示无请求
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
  * @brief  任务自删除函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void wTaskDeleteSelf (void)
{
    uint32_t status = wTaskEnterCritical();

    wTaskSchedRemove(currentTask);

    if (currentTask->clean)        // 删除时，如果有设置清理函数，则调用清理函数
    {
        currentTask->clean(currentTask->cleanparam);
    }

    wTaskSched();

	currentTask->state |= WQOS_TASK_STATE_DESTORYED;
	
    wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  任务状态查询函数
  * @param  task   任务结构指针
  * @retval info   状态查询结构指针
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
