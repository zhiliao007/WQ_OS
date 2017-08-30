#include "WQ_OS.h"

static wList wTimerHardList;
static wList wTimerSoftList;

static wSem wTimerProtectSem;
static wSem wTimerTicksSem;

/*******************************************************************************************************************
  * @brief  初始化软件定时器函数
  * @param  timer 等待初始化的定时器结构指针
			delayTicks 定时器初始启动的延时ticks数。
			durationTicks 给周期性定时器用的周期tick数，一次性定时器无效
			timerFunc 定时器回调函数
			arg 传递给定时器回调函数的参数
			timerFunc 定时器回调函数
			config 定时器的初始配置
  * @retval 无
  ******************************************************************************************************************/	
void wTimerInit(wTimer * timer, uint32_t delayTicks, uint32_t durationTicks, void(*timerFunc) (void * arg), void * arg, uint32_t config)
{
	wNodeInit(&timer->linkNode);
	timer->startDelayTicks = delayTicks;
	timer->durationTicks = durationTicks;
	timer->timerFunc = timerFunc;
	timer->arg = arg;
	timer->config = config;
	
	if(delayTicks == 0)
	{
		timer->delayTicks = durationTicks;
	}
	else
	{
		timer->delayTicks = timer->startDelayTicks;
	}
	
	timer->state = timerCreated;
}

/*******************************************************************************************************************
  * @brief  软件定时器启动函数
  * @param  timer 软件定时器结构指针
  * @retval 无
  ******************************************************************************************************************/	
void wTimerStart (wTimer * timer)
{
    switch (timer->state)
    {
        case timerCreated:
        case timerStopped:
            timer->delayTicks = timer->startDelayTicks ? timer->startDelayTicks : timer->durationTicks;
            timer->state = timerStarted;
		
            if (timer->config & TIMER_CONFIG_TYPE_HARD)
            {
                uint32_t status = wTaskEnterCritical();

                wListAddLast(&wTimerHardList, &timer->linkNode);

                wTaskExitCritical(status);
            }
            else
            {
                wSemWait(&wTimerProtectSem, 0);
				
                wListAddLast(&wTimerSoftList, &timer->linkNode);
				
                wSemNotify(&wTimerProtectSem);
            }
            break;
        default:
            break;
    }
}

/*******************************************************************************************************************
  * @brief  软件定时器停止函数
  * @param  timer 软件定时器结构指针
  * @retval 无
  ******************************************************************************************************************/	
void wTimerStop (wTimer * timer)
{
    switch (timer->state)
    {
        case timerStarted:
        case timerRunning:
            if (timer->config & TIMER_CONFIG_TYPE_HARD)
            {
                uint32_t status = wTaskEnterCritical();

                wListRemove(&wTimerHardList, &timer->linkNode);

                wTaskExitCritical(status);
            }
            else
            {
                wSemWait(&wTimerProtectSem, 0);
				
                wListRemove(&wTimerSoftList, &timer->linkNode);
				
                wSemNotify(&wTimerProtectSem);
            }
            timer->state = timerStopped;
            break;
        default:
            break;
    }
} 
	
/*******************************************************************************************************************
  * @brief  软件定时器链表处理函数
  * @param  timerList 定时器链表结构指针
  * @retval 无
  ******************************************************************************************************************/	
static void wTimerCallFuncList(wList * timerList)
{
	wNode * node;
	for(node = timerList->headNode.nextNode; node != &(timerList->headNode); node = node->nextNode)
	{
		wTimer * timer = wNodeParent(node, wTimer, linkNode);
		if((timer->delayTicks == 0) || (--timer->delayTicks == 0))
		{
			timer->state = timerRunning;
			timer->timerFunc(timer->arg);
			if(timer->durationTicks > 0)
			{
				timer->delayTicks = timer->durationTicks;
				timer->state = timerStarted;
			}
			else
			{
				wListRemove(timerList, &timer->linkNode);
				timer->state = timerStopped;
			}
		}
	}
}

static wTask wTimerTask;
static wTaskStack wTimerTaskStack[WQ_OS_TIMERTASK_STACK_SIZE];

/*******************************************************************************************************************
  * @brief  软件定时器任务函数
  * @param  param 参数
  * @retval 无
  ******************************************************************************************************************/	
static void wTimerSoftTask(void * param)
{
	for(;;)
	{
		wSemWait(&wTimerTicksSem, 0);
		
		wSemWait(&wTimerProtectSem, 0);
		
		wTimerCallFuncList(&wTimerSoftList);
		
		wSemNotify(&wTimerProtectSem);
	}
}

/*******************************************************************************************************************
  * @brief  软件定时器模块时钟节拍处理函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTimerModuleTickNotify(void)
{
	uint32_t status = wTaskEnterCritical();

    wTimerCallFuncList(&wTimerHardList);

    wTaskExitCritical(status);

    wSemNotify(&wTimerTicksSem);	
}
	
/*******************************************************************************************************************
  * @brief  定时器模块初始化函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/	
void wTimerModuleInit(void)
{
	wListInit(&wTimerHardList);
	wListInit(&wTimerSoftList);
	wSemInit(&wTimerProtectSem, 1,1);
	wSemInit(&wTimerTicksSem,0,0);
	 
#if WQ_OS_TIMERTASK_PRIO >= (WQ_OS_PRO_COUNT - 1)
	#error "The proprity of timer task must be greater then (WQ_OS_PRO_COUNT - 1)"
#endif /*WQ_OS_TIMERTASK_PRIO >= (WQ_OS_PRO_COUNT - 1)*/

	wTaskInit(&wTimerTask, wTimerSoftTask, (void *)0, WQ_OS_TIMERTASK_PRIO, &wTimerTaskStack[WQ_OS_TIMERTASK_STACK_SIZE]);
}
