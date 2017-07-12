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
  * @brief  粗暴延时函数
  * @param  count： 延时时间
  * @retval 无
  ******************************************************************************************************************/
void delay(int count)
{
     while(--count>0);
}

wTask wTaskIdle;
wTaskStack idleTaskEnv[WQ_OS_IDLETASK_STACK_SIZE];

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


/*******************************************************************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
int main()
{
	wTaskSchedInit();            //内核功能初始化
	
	wTaskDelayInit();            //初始化延时队列
	
    wInitApp();                  //初始化任务
	
	wTaskInit(&wTaskIdle, idleTaskEntry, (void *)0, WQ_OS_PRO_COUNT - 1, &idleTaskEnv[WQ_OS_IDLETASK_STACK_SIZE]);    //初始化空闲任务
	idleTask = &wTaskIdle;
	
    nextTask = wTaskHighestReady();	//第一个要运行的任务指向任务1
	
	wTaskRunFirst();			//将CPU控制权交给OS，开始运行系统
	
	return 0;
}
