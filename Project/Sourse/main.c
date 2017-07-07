#include "WQ_OS.h"
#include <ARMCM3.h>

wTask * currentTask;       //指向当前任务的指针
wTask * nextTask;          //指向将要运行任务的指针
wTask * idleTask;          //指向空闲任务的指针

wBitmap taskPrioBitmap;    //任务优先级位图
wList taskTable[WQ_OS_PRO_COUNT];      //任务就绪表

uint8_t schedlockCount;     //调度锁计数器

wList wTaskDelayList;       //延时队列

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
	wListAddFirst(&taskTable[prio], &(task->linkNode));
	
	wBitmapSet(&taskPrioBitmap, prio);                  //并将该位置1
}

/*******************************************************************************************************************
  * @brief  获取当前最高优先级且可运行的任务函数
  * @param  无
  * @retval 优先级最高的且可运行的任务
  ******************************************************************************************************************/	
wTask * wTaskHighestReady(void)
{
	uint32_t highestPrio = wBitmapGetFirstSet(&taskPrioBitmap);
	wNode * node = wListFirst(&taskTable[highestPrio]);
	return wNodeParent(node, wTask, linkNode);
}

/*******************************************************************************************************************
  * @brief  内核初始化函数（初始化调度器）
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedInit(void)
{
	int i;
	
	wBitmapInit(&taskPrioBitmap);          //初始化位图
	for(i = 0; i < WQ_OS_PRO_COUNT; i++)   //初始化优先级数组的各个列表项
	{
		wListInit(&taskTable[i]);
	}
	schedlockCount = 0;
}

/*******************************************************************************************************************
  * @brief  调度锁上锁函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedDisable(void)
{
	uint32_t status = wTaskEnterCritical();      //临界区保护,上锁过程不允许被中断打断
	
	if(schedlockCount < 255)
	{
		schedlockCount++;
	}
	
	wTaskExitCritical(status);                   //退出临界区保护
}

/*******************************************************************************************************************
  * @brief  任务调度函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedEnable(void)
{
	uint32_t status = wTaskEnterCritical();      //临界区保护,上锁过程不允许被中断打断
	if(schedlockCount > 0)
	{
		if(--schedlockCount == 0)
		{
			wTaskSched();
		}
	}
	wTaskExitCritical(status);                   //退出临界区保护
}

/*******************************************************************************************************************
  * @brief  将任务插入就绪列表（将任务从延时队列中移除时）函数
  * @param  task   任务结构指针
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedRdy(wTask * task)
{
	wListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	wBitmapSet(&taskPrioBitmap,task->prio);
}

/*******************************************************************************************************************
  * @brief  将任务从就绪列表中删除函数
  * @param  task   任务结构指针
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedUnRdy(wTask * task)
{
	wListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(wListCount(&taskTable[task->prio]) == 0)
	{
		wBitmapClear(&taskPrioBitmap,task->prio);
	}
}

/*******************************************************************************************************************
  * @brief  任务调度函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSched(void)
{
	wTask * tempTask;
	uint32_t status = wTaskEnterCritical();        
	
	if(schedlockCount > 0)
	{
		wTaskExitCritical(status);
		return;
	}
	
	tempTask = wTaskHighestReady();
	if(tempTask != currentTask)
	{
		nextTask = tempTask;
		wTaskSwitch();
	}
	
	wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  任务延时初始化函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void wTaskDelayInit(void)
{
	wListInit(&wTaskDelayList);             //初始化延时队列
}

/*******************************************************************************************************************
  * @brief  将任务插入延时队列函数
  * @param  task   任务结构指针
            ticks  延时的ticks数
  * @retval 无
  ******************************************************************************************************************/
void wTimeTaskWait(wTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	wListAddLast(&wTaskDelayList, &(task->delayNode));
	task->state |= WQOS_TASK_STATE_DELAYED;
}

/*******************************************************************************************************************
  * @brief  将任务从延时队列删除函数
  * @param  task   延时结构指针
  * @retval 无
  ******************************************************************************************************************/
void wTimeTaskWakeUp(wTask * task)
{
	wListRemove(&wTaskDelayList,&(task->delayNode));
	task->state &= ~WQOS_TASK_STATE_DELAYED;
}
	
/*******************************************************************************************************************
  * @brief  任务SystemTick中断服务函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void wTaskSystemTickHandler(void)
{	
	wNode * node;
	
	uint32_t status = wTaskEnterCritical();
	for(node = wTaskDelayList.headNode.nextNode; node != &(wTaskDelayList.headNode); node = node->nextNode)
	{
		wTask * task = wNodeParent(node, wTask, delayNode);
		if(--task->delayTicks == 0)
		{
			wTimeTaskWakeUp(task);       //从延时队列中删除
			
			wTaskSchedRdy(task);         //插入就绪列表
		}
	}
	
	if(--currentTask->slice == 0)
	{
		if(wListCount(&taskTable[currentTask->prio]) > 0)
		{
			wListRemoveFirst(&taskTable[currentTask->prio]);
			wListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			
			currentTask->slice = WQ_OS_SLICE_MAX;
		}
	}
	wTaskExitCritical(status);
	
	wTaskSched();
}

/*******************************************************************************************************************
  * @brief  任务延时函数
  * @param  delay： 任务延时时间(x10ms)
  * @retval 无
  ******************************************************************************************************************/
void wTaskDelay(uint32_t delay)
{
	uint32_t status = wTaskEnterCritical();
	
	wTimeTaskWait(currentTask, delay);
	
	wTaskSchedUnRdy(currentTask);
	
	wTaskExitCritical(status);
	
	wTaskSched();
}

/*******************************************************************************************************************
  * @brief  SysTick初始化函数
  * @param  ms： 定时时间
  * @retval 无
  ******************************************************************************************************************/
void wSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}

/*******************************************************************************************************************
  * @brief  SysTick中断服务函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void SysTick_Handler()
{
	wTaskSystemTickHandler();
}
/*******************************************************************************************************************
  * @brief  粗暴延时函数
  * @param  count： 延时时间
  * @retval 无
  ******************************************************************************************************************/
void delay(int count)
{
     while(--count>0);
}

int task1Flag;
int task2Flag;
int task3Flag;

/*******************************************************************************************************************
  * @brief  任务1入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task1Entry(void * param)
{
	wSetSysTickPeriod(10);
	for(;;)
	{
		task1Flag = 0;
		wTaskDelay(1);
		task1Flag = 1;
		wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  任务2入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(0xff);
		task2Flag = 1;
		delay(0xff);
	}
}

void task3Entry(void * param)
{
	for(;;)
	{
		task3Flag = 0;
		delay(0xff);
		task3Flag = 1;
		delay(0xff);
	}
}

wTask wTaskIdle;
wTaskStack idleTaskEnv[1024];

/*******************************************************************************************************************
  * @brief  空闲任务函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void idleTaskEntry(void * param)
{
	for(;;)
	{
	}
}

wTask wTask1;
wTask wTask2;
wTask wTask3;

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];
wTaskStack Task3Env[1024];

/*******************************************************************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
int main()
{
	wTaskSchedInit();            //内核功能初始化
	
	wTaskDelayInit();            //初始化延时队列
	
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, 0, &Task1Env[1024]);	//初始化任务
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, 1, &Task2Env[1024]);
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 1, &Task3Env[1024]);
	

	wTaskInit(&wTaskIdle, idleTaskEntry, (void *)0, WQ_OS_PRO_COUNT - 1, &idleTaskEnv[1024]);    //初始化空闲任务
	idleTask = &wTaskIdle;
	
    nextTask = wTaskHighestReady();	//第一个要运行的任务指向任务1
	
	wTaskRunFirst();			//将CPU控制权交给OS，开始运行系统
	
	return 0;
}
