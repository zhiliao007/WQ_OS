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
