/*
 * @file wtask.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   任务
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 
 
#include "WQ_OS.h"
#include <string.h>

/*!
 * @brief  任务初始化函数
 * @param  task： 任务结构体地址
 * @param  entry：任务入口函数名
 * @param  param：传递给任务的参数
 * @param  stack：任务栈空间地址
 * @retval 无
 */	
void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack, uint32_t size)
{
	uint32_t * stackTop;
	
	task->stackBase = stack;
	task->stackSize = size;
	memset(stack, 0, size);
	
	stackTop = stack + size / sizeof(wTaskStack);
	
	// XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行 
	*(--stackTop) = (unsigned long)(1<<24);                
	// R15(PC) 程序的入口地址 
    *(--stackTop) = (unsigned long)entry;
    // R14(LR), 任务不会通过return xxx结束自己，所以未用,R14内核寄存器用来保存函数的返回值地址	
    *(--stackTop) = (unsigned long)0x14;                   
														
	// R12, 未用
    *(--stackTop) = (unsigned long)0x12; 
	// R3,  未用
    *(--stackTop) = (unsigned long)0x3;
	// R2,  未用
    *(--stackTop) = (unsigned long)0x2;     
	// R1,  未用
    *(--stackTop) = (unsigned long)0x1;  
	// R0 = param, 传给任务的入口函数,函数的第一个参数的地址保存在R0内核寄存器
    *(--stackTop) = (unsigned long)param;                  
	                                                  
	// R11, 未用
	*(--stackTop) = (unsigned long)0x11;
	// R10, 未用
    *(--stackTop) = (unsigned long)0x10;
	// R9,  未用
    *(--stackTop) = (unsigned long)0x9; 
	// R8,  未用
    *(--stackTop) = (unsigned long)0x8; 
	// R7,  未用
    *(--stackTop) = (unsigned long)0x7;   
	// R6,  未用
    *(--stackTop) = (unsigned long)0x6;   
	// R5,  未用
    *(--stackTop) = (unsigned long)0x5; 
	// R4,  未用
    *(--stackTop) = (unsigned long)0x4;                    
	
	task->slice = WQ_OS_SLICE_MAX; 
	/* 保存最终的值，使任务栈指针指向缓冲区栈顶 */
	task->stack = stackTop;								
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
	
#if WQ_OS_ENABLE_HOOKS == 1
	wHooksTaskInit(task);
#endif
}

/*!
 * @brief  任务挂起函数
 * @param  task   任务结构指针
 * @retval 无
 */
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

/*!
 * @brief  恢复被挂起函数
 * @param  task   任务结构指针
 * @retval 无
 */
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

/*!
 * @brief  任务被删除时调用的清理函数
 * @param  task   任务结构指针
 * @param  clean  清理函数入口地址
 * @param  param  传递给清理函数的参数
 * @retval 无
 */
void wTaskSetCleanCallFunc(wTask * task, void(*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanparam = param;
}	

/*!
 * @brief  任务强制删除函数
 * @param  task   任务结构指针
 * @retval 无
 */
void wTaskForceDelete(wTask * task) 
{
    uint32_t status = wTaskEnterCritical();

	/* 如果任务处于延时状态，则从延时队列中删除 */
    if (task->state & WQOS_TASK_STATE_DELAYED)            
    {
        wTimeTaskRemove(task);
    }
	/* 如果任务不处于挂起状态，那么就是就绪态，从就绪表中删除 */
    else if (!(task->state & WQOS_TASK_STATE_SUSPEND))   
    {
        wTaskSchedRemove(task);
    }

	/* 删除时，如果有设置清理函数，则调用清理函数 */
    if (task->clean)               
    {
        task->clean(task->cleanparam);
    }
    
	/* 如果删除的是自己，那么需要切换至另一个任务，所以执行一次任务调度 */
    if (currentTask == task)      
    {
        wTaskSched();
    }
	
	currentTask->state |= WQOS_TASK_STATE_DESTORYED;

    wTaskExitCritical(status); 
}

/*!
 * @brief  任务请求删除函数
 * @param  task   任务结构指针
 * @retval 无
 */
void wTaskRequestDelete (wTask * task)
{
    uint32_t status = wTaskEnterCritical();

    task->requestDeleteFlag = 1;
	
    wTaskExitCritical(status); 
}

/*!
 * @brief  检查任务是否请求删除函数
 * @param  无
 * @retval 请求删除标志 非0表示请求删除，0表示无请求
 */
uint8_t wTaskIsRequestedDelete (void)
{
    uint8_t delete;

    uint32_t status = wTaskEnterCritical();

    delete = currentTask->requestDeleteFlag;

    wTaskExitCritical(status); 

    return delete;
}

/*!
 * @brief  任务自删除函数
 * @param  无
 * @retval 无
 */
void wTaskDeleteSelf (void)
{
    uint32_t status = wTaskEnterCritical();

    wTaskSchedRemove(currentTask);

	/* 删除时，如果有设置清理函数，则调用清理函数 */
    if (currentTask->clean)        
    {
        currentTask->clean(currentTask->cleanparam);
    }

    wTaskSched();

	currentTask->state |= WQOS_TASK_STATE_DESTORYED;
	
    wTaskExitCritical(status);
}

/*!
 * @brief  任务状态查询函数
 * @param  task   任务结构指针
 * @retval info   状态查询结构指针
 */
void wTaskGetInfo(wTask * task, wTaskInfo * info)
{
	uint32_t * stackEnd;
	uint32_t status = wTaskEnterCritical();
	
	info->delayTicks = task->delayTicks;
	info->prio = task->prio;
	info->state = task->state;
	info->slice = task->slice;
	info->suspendCount = task->suspendCount;
	
	info->stackSize = task->stackSize; 
	info->stackFree = 0;
	stackEnd = task->stackBase;
	while((*stackEnd++ == 0) && (stackEnd <= task->stackBase + task->stackSize / sizeof(wTaskStack)))
	{
		info->stackFree++;
	}
	info->stackFree *= sizeof(wTaskStack);
	
	wTaskExitCritical(status);
}
