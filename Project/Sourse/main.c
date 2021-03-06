/*
 * @file main.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   主
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "WQ_OS.h"
#include <ARMCM3.h>


/******************************************************************************
 * 定义变量                              
 ******************************************************************************/
wTask * currentTask;       /*!< 指向当前任务的指针 */
wTask * nextTask;          /*!< 指向将要运行任务的指针 */
wTask * idleTask;          /*!< 指向空闲任务的指针 */

wBitmap taskPrioBitmap;    /*!< 任务优先级位图 */
wList taskTable[WQ_OS_PRO_COUNT];      /*!< 任务就绪表 */

uint8_t schedlockCount;     /*!< 调度锁计数器 */

uint32_t tickCount;         /*!< 时钟节拍计数器 */

wList wTaskDelayList;       /*!< 延时队列 */

#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1

uint32_t idleCount;         /*!< 空闲任务计数器 */

uint32_t idleMaxCount;      /*!< 空闲任务最大计数器 */

static void initCpuUsageStat (void);
static void checkCpuUsage (void);
static void cpuUsageSyncWithSysTick (void);
#endif

/*!
 * @brief  获取当前最高优先级且可运行的任务函数
 * @param  无
 * @retval 优先级最高的且可运行的任务
 */	
wTask * wTaskHighestReady(void)
{
	uint32_t highestPrio = wBitmapGetFirstSet(&taskPrioBitmap);
	wNode * node = wListFirst(&taskTable[highestPrio]);
	return wNodeParent(node, wTask, linkNode);
}

/*!
 * @brief  内核初始化函数（初始化调度器）
 * @param  无
 * @retval 无
 */	
void wTaskSchedInit(void)
{
	int i;
	
	/* 初始化位图 */
	wBitmapInit(&taskPrioBitmap);
	/* 初始化优先级数组的各个列表项 */
	for(i = 0; i < WQ_OS_PRO_COUNT; i++)   
	{
		wListInit(&taskTable[i]);
	}
	schedlockCount = 0;
}

/*!
 * @brief  调度锁上锁函数
 * @param  无
 * @retval 无
 */	
void wTaskSchedDisable(void)
{
	/* 临界区保护,上锁过程不允许被中断打断 */
	uint32_t status = wTaskEnterCritical();      
	
	if(schedlockCount < 255)
	{
		schedlockCount++;
	}
	
	/* 退出临界区保护 */
	wTaskExitCritical(status);                   
}

/*!
 * @brief  允许任务调度函数
 * @param  无
 * @retval 无
 */	
void wTaskSchedEnable(void)
{
	/* 临界区保护,上锁过程不允许被中断打断 */
	uint32_t status = wTaskEnterCritical();      
	if(schedlockCount > 0)
	{
		if(--schedlockCount == 0)
		{
			wTaskSched();
		}
	}
	/* 退出临界区保护 */
	wTaskExitCritical(status);                   
}

/*!
 * @brief  将任务插入就绪列表（将任务从延时队列中移除时）函数
 * @param  task   任务结构指针
 * @retval 无
 */	
void wTaskSchedRdy(wTask * task)
{
	wListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	wBitmapSet(&taskPrioBitmap,task->prio);
}

/*!
 * @brief  将任务从就绪列表中删除函数
 * @param  task   任务结构指针
 * @retval 无
 */	
void wTaskSchedUnRdy(wTask * task)
{
	wListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(wListCount(&taskTable[task->prio]) == 0)
	{
		wBitmapClear(&taskPrioBitmap,task->prio);
	}
}

/*!
 * @brief  将任务从优先级列表中删除函数
 * @param  task   任务结构指针
 * @retval 无
 */	
void  wTaskSchedRemove(wTask * task)
{
	wListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(wListCount(&taskTable[task->prio]) == 0)
	{
		wBitmapClear(&taskPrioBitmap,task->prio);
	}
}

/*!
 * @brief  任务调度函数
 * @param  无
 * @retval 无
 */	
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
		
#if WQ_OS_ENABLE_HOOKS == 1
	wHooksTaskSwitch(currentTask, nextTask);
#endif
	}
	
	wTaskExitCritical(status);
}

/*!
 * @brief  任务延时初始化函数
 * @param  无
 * @retval 无
 */
void wTaskDelayInit(void)
{
	/* 初始化延时队列 */
	wListInit(&wTaskDelayList);             
}

/*!
 * @brief  将任务插入延时队列函数
 * @param  task   任务结构指针
 * @param  ticks  延时的ticks数
 * @retval 无
 */
void wTimeTaskWait(wTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	wListAddLast(&wTaskDelayList, &(task->delayNode));
	task->state |= WQOS_TASK_STATE_DELAYED;
}

/*!
 * @brief  将任务从延时队列唤醒函数
 * @param  task   延时结构指针
 * @retval 无
 */
void wTimeTaskWakeUp(wTask * task)
{
	wListRemove(&wTaskDelayList,&(task->delayNode));
	task->state &= ~WQOS_TASK_STATE_DELAYED;
}

/*!
 * @brief  将任务从延时队列删除函数
 * @param  task   延时结构指针
 * @retval 无
 */
void wTimeTaskRemove(wTask * task)
{
	wListRemove(&wTaskDelayList, &(task->delayNode));
}	

/*!
 * @brief  时钟节拍计数器初始化函数
 * @param  无
 * @retval 无
 */
void wTimeTickInit(void)
{
	tickCount = 0;
}
	
/*!
 * @brief  任务SystemTick中断服务函数
 * @param  无
 * @retval 无
 */
void wTaskSystemTickHandler(void)
{	
	wNode * node;
	
	uint32_t status = wTaskEnterCritical();
	for(node = wTaskDelayList.headNode.nextNode; node != &(wTaskDelayList.headNode); node = node->nextNode)
	{
		wTask * task = wNodeParent(node, wTask, delayNode);
		if(--task->delayTicks == 0)
		{
			if (task->waitEvent) 
            {
				/* 此时，消息为空，等待结果为超时 */
                wEventRemoveTask(task, (void *)0, wErrorTimeout);  
            }
			/* 从延时队列中删除 */
			wTimeTaskWakeUp(task);       
			
			wTaskSchedRdy(task);        
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
	
	tickCount++;
	
#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1	
	checkCpuUsage();
#endif
	
	wTaskExitCritical(status);
	
#if WQ_OS_ENABLE_TIMER == 1
	wTimerModuleTickNotify();
#endif

#if WQ_OS_ENABLE_HOOKS == 1
	    wHooksSysTick();
#endif
	
	wTaskSched();
}

#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1	

static float cpuUsage;                      /*!< cpu使用率统计 */
static uint32_t enableCpuUsageStat;         /*!< 是否使能cpu统计 */

/*!
 * @brief  初始化CPU统计函数
 * @param  无
 * @retval 无
 */
static void initCpuUsageStat (void)
{
    idleCount = 0;
    idleMaxCount = 0;
    cpuUsage = 0.0f;
    enableCpuUsageStat = 0;
}

/*!
 * @brief  检查CPU使用率函数
 * @param  无
 * @retval 无
 */
static void checkCpuUsage (void)
{
    if (enableCpuUsageStat == 0)
    {
        enableCpuUsageStat = 1;
        tickCount = 0;
        return;
    }

    if (tickCount == TICKS_PER_SEC)
    {
        idleMaxCount = idleCount;
        idleCount = 0;

        wTaskSchedEnable();
    }
    else if (tickCount % TICKS_PER_SEC == 0)
    {
        cpuUsage = 100 - (idleCount * 100.0 / idleMaxCount);
        idleCount = 0;
    }
}

/*!
 * @brief  等待时钟同步函数
 * @param  无
 * @retval 无
 */
static void cpuUsageSyncWithSysTick (void)
{
    /* 等待与时钟节拍同步 */
    while (enableCpuUsageStat == 0)
    {
        ;;
    }
}

/*!
 * @brief  获取CPU使用率函数
 * @param  无
 * @retval CPU使用率
 */
float wCpuUsageGet (void)
{
    float usage = 0;

    uint32_t status = wTaskEnterCritical();
    usage = cpuUsage;
    wTaskExitCritical(status);

    return usage;
}
#endif

wTask wTaskIdle;
wTaskStack idleTaskEnv[WQ_OS_IDLETASK_STACK_SIZE];

/*!
 * @brief  空闲任务函数
 * @param  无
 * @retval 无
 */
void idleTaskEntry(void * param)
{
	wTaskSchedDisable();
	
	/* 初始化任务 */
    wInitApp();                  
	
#if WQ_OS_ENABLE_TIMER == 1
	/* 初始化定时器任务 */
	wTimerInitTask();            
#endif
	
	wSetSysTickPeriod(WQ_OS_SYSTICK_MS);  
	
#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1	
	/* 等待与时钟同步 */
	cpuUsageSyncWithSysTick();       
#endif
	
    for (;;)
    {
		
#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1	
        uint32_t status = wTaskEnterCritical();
        idleCount++;
        wTaskExitCritical(status);
#endif
		
#if WQ_OS_ENABLE_HOOKS == 1
	wHooksCpuIdle();
#endif		
    }
}


/*!
 * @brief  主函数
 * @param  无
 * @retval 无
 */	
int main()
{
	/* 内核功能初始化 */
	wTaskSchedInit();            
	
	/* 初始化延时队列 */
	wTaskDelayInit();            
#if WQ_OS_ENABLE_TIMER == 1
	
	#if WQ_OS_ENABLE_SEM == 0
	#error "Sem module did not open!"
	#endif
	
	/* 初始化定时器模块 */
	wTimerModuleInit();          
#endif
	
	/* 初始化时钟节拍 */
	wTimeTickInit();             
	
#if WQ_OS_ENABLE_CPUUSAGE_STAT == 1	
	/* 初始化CPU统计 */
	initCpuUsageStat();          
#endif
	
	wTaskInit(&wTaskIdle, idleTaskEntry, (void *)0, WQ_OS_PRO_COUNT - 1, idleTaskEnv, WQ_OS_IDLETASK_STACK_SIZE);    //初始化空闲任务
	idleTask = &wTaskIdle;
	
	/* 第一个要运行的任务指向任务1 */
    nextTask = wTaskHighestReady();	
	
	/*将CPU控制权交给OS，开始运行系统 */
	wTaskRunFirst();			
	
	return 0;
}
