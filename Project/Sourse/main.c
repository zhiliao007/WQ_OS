#include "WQ_OS.h"
#include <ARMCM3.h>

wTask * currentTask;       //指向当前任务的指针
wTask * nextTask;          //指向将要运行任务的指针
wTask * idleTask;          //指向空闲任务的指针
wTask * taskTable[2];      //任务数组指针

uint8_t schedlockCount;     //调度锁计数器

/*******************************************************************************************************************
  * @brief  任务初始化函数
  * @param  task： 任务结构体地址
		  entry：任务入口函数名
		  param：传递给任务的参数
          stack：任务栈空间地址
  * @retval 无
  ******************************************************************************************************************/	
void wTaskInit(wTask * task, void (*entry)(void *), void * param, wTaskStack * stack)
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
}

/*******************************************************************************************************************
  * @brief  调度锁初始化函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTaskSchedInit(void)
{
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
	uint32_t status = wTaskEnterCritical();        
	
	if(schedlockCount > 0)
	{
		wTaskExitCritical(status);
		return;
	}
	// 空闲任务只有在所有其它任务都不是延时状态时才执行
    // 所以，我们先检查下当前任务是否是空闲任务
    if (currentTask == idleTask) 
    {
        // 如果是的话，那么去执行task1或者task2中的任意一个
        // 当然，如果某个任务还在延时状态，那么就不应该切换到他。
        // 如果所有任务都在延时，那么就继续运行空闲任务，不进行任何切换了
        if (taskTable[0]->delayTicks == 0) 
        {
            nextTask = taskTable[0];
        }           
        else if (taskTable[1]->delayTicks == 0) 
        {
            nextTask = taskTable[1];
        } else 
        {
			wTaskExitCritical(status);
            return;
        }
    } 
    else 
    {
        // 如果是task1或者task2的话，检查下另外一个任务
        // 如果另外的任务不在延时中，就切换到该任务
        // 否则，判断下当前任务是否应该进入延时状态，如果是的话，就切换到空闲任务。否则就不进行任何切换
        if (currentTask == taskTable[0]) 
        {
            if (taskTable[1]->delayTicks == 0) 
            {
                nextTask = taskTable[1];
            }
            else if (currentTask->delayTicks != 0) 
            {
                nextTask = idleTask;
            } 
            else 
            {
				wTaskExitCritical(status);
                return;
            }
        }
        else if (currentTask == taskTable[1]) 
        {
            if (taskTable[0]->delayTicks == 0) 
            {
                nextTask = taskTable[0];
            }
            else if (currentTask->delayTicks != 0) 
            {
                nextTask = idleTask;
            }
            else 
            {
				wTaskExitCritical(status);
                return;
            }
        }
    }
	
	wTaskSwitch();
	
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
	
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
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
		wTaskDelay(2);
		task2Flag = 1;
		wTaskDelay(2);
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
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, &Task1Env[1024]);	//初始化任务
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, &Task2Env[1024]);
	
	
	taskTable[0] = &wTask1;		//初始化任务列表
	taskTable[1] = &wTask2;

	wTaskInit(&wTaskIdle, idleTaskEntry, (void *)0, &idleTaskEnv[1024]);    //初始化空闲任务
	idleTask = &wTaskIdle;
	
    nextTask = taskTable[0];	//第一个要运行的任务指向任务1
	
	wTaskRunFirst();			//将CPU控制权交给OS，开始运行系统
	
	return 0;
}
