#include "WQ_OS.h"
#include <ARMCM3.h>

wTask * currentTask;       //指向当前任务的指针
wTask * nextTask;          //指向将要运行任务的指针
wTask * idleTask;          //指向空闲任务的指针

wBitmap taskPrioBitmap;    //任务优先级位图
wTask * taskTable[WQ_OS_PRO_COUNT];      //任务就绪表

uint8_t schedlockCount;     //调度锁计数器

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
	 
	task->stack = stack;								// 保存最终的值，使任务栈指针指向缓冲区栈顶
	task->delayTicks = 0;
	task->prio = prio;
	
	taskTable[prio] = task;                             //将任务就绪表对应位指向任务
	wBitmapSet(&taskPrioBitmap, prio);                  //并将该位置1
}

/*******************************************************************************************************************
  * @brief  查找就绪表中优先级最高的任务函数
  * @param  无
  * @retval 就绪表中优先级最高的任务指针
  ******************************************************************************************************************/	
wTask * wTaskHighestReady(void)
{
	uint32_t highestPrio = wBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}
/*******************************************************************************************************************
  * @brief  内核初始化函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedInit(void)
{
	wBitmapInit(&taskPrioBitmap);        //初始化位图
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
  * @brief  任务SystemTick中断服务函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void wTaskSystemTickHandler()
{	
	int i;
	
	uint32_t status = wTaskEnterCritical();
	
	for(i = 0; i < WQ_OS_PRO_COUNT; i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
		else 
		{
			wBitmapSet(&taskPrioBitmap, i);
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
	
	currentTask->delayTicks = delay;
	wBitmapClear(&taskPrioBitmap, currentTask->prio);         //清零就绪表中该任务
	
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
		wTaskDelay(100);
		task1Flag = 1;
		wTaskDelay(100);
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
		wTaskDelay(200);
		task2Flag = 1;
		wTaskDelay(200);
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

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];

/*******************************************************************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
int main()
{
	wTaskSchedInit();            //内核功能初始化
	
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, 0, &Task1Env[1024]);	//初始化任务
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, 1, &Task2Env[1024]);
	
	
	taskTable[0] = &wTask1;		//初始化任务列表
	taskTable[1] = &wTask2;

	wTaskInit(&wTaskIdle, idleTaskEntry, (void *)0, WQ_OS_PRO_COUNT - 1, &idleTaskEnv[1024]);    //初始化空闲任务
	idleTask = &wTaskIdle;
	
    nextTask = wTaskHighestReady();	//第一个要运行的任务指向任务1
	
	wTaskRunFirst();			//将CPU控制权交给OS，开始运行系统
	
	return 0;
}
