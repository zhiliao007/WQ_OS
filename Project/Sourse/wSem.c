#include "wSem.h"
#include "WQ_OS.h"
/*******************************************************************************************************************
  * @brief  初始化信号量函数
  * @param  sem         信号量结构指针
			startCount  初始计数值
			maxCount    最大计数值，若为0则不限数量 
  * @retval 无
  ******************************************************************************************************************/	
void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount) 
{
	wEventInit(&sem->event, wEventTypeSem);

	sem->maxCount = maxCount;
	if (maxCount == 0)
	{
		sem->count = startCount;
	} 
	else 
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

/*******************************************************************************************************************
  * @brief  等待信号量函数
  * @param  sem       信号量结构指针
			waitTicks 等待的最大ticks数
                      //当信号量计数为0时，等待的ticks数，为0时表示永远等待
* @retval 等待结果  wErrorNoError 
	                wErrorTimeout             
	                wErrorResourceUnavaliable 
  ******************************************************************************************************************/
uint32_t wSemWait(wSem * sem, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	if(sem->count > 0)
	{
		--sem->count;
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else
	{
		wEventWait(&sem->event, currentTask, (void *)0,  wEventTypeSem, waitTicks);                                                                                                                                                
        wTaskExitCritical(status);
		wTaskSched();
        return currentTask->waitEventResult;   // 当由于等待超时或者计数可用时，执行会返回到这里，然后取出等待结构
	}
}

/*******************************************************************************************************************
  * @brief  获取信号量函数
  * @param  sem       信号量结构指针
  * @retval 等待结果  wErrorNoError 
	                  wErrorTimeout             
	                  wErrorResourceUnavaliable 
  ******************************************************************************************************************/
uint32_t wSemNoWaitGet (wSem * sem)
{
    uint32_t status = wTaskEnterCritical();
 
	if (sem->count > 0) 
    {
		--sem->count; 
        wTaskExitCritical(status);
    	return wErrorNoError;
    } 
	else 
	{
        wTaskExitCritical(status);
    	return wErrorResourceUnavaliable;
    }    
}
	
/*******************************************************************************************************************
  * @brief  信号量唤醒任务函数
  * @param  sem       信号量结构指针
  * @retval 无
  ******************************************************************************************************************/
void wSemNotify (wSem * sem)
{
    uint32_t status = wTaskEnterCritical();        
    
    if (wEventWaitCount(&sem->event) > 0)  //检查是否有任务在等待
    {
    
        wTask * task = wEventWakeUp(&sem->event, (void *)0, wErrorNoError );  //此处唤醒的是队列首部的任务

        if (task->prio < currentTask->prio)
        {
            wTaskSched(); 
    	}
    }
    else
    {
    	++sem->count;    //可用资源（事件）+1

    	if ((sem->maxCount != 0) && (sem->count > sem->maxCount)) 
    	{	
    		sem->count = sem->maxCount;
    	}
    }
    wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  信号量状态查询函数
  * @param  sem    信号量结构指针
			info   状态查询结构指针
  * @retval 无
  ******************************************************************************************************************/
void wSemGetInfo (wSem * sem, wSemInfo * info)
{
    uint32_t status = wTaskEnterCritical();        
    
    info->count = sem->count;
    info->maxCount = sem->maxCount;
    info->taskCount = wEventWaitCount(&sem->event);
    
    wTaskExitCritical(status);
}

	/*******************************************************************************************************************
  * @brief  销毁信号量函数
  * @param  sem       信号量结构指针
  * @retval 因销毁该信号量而唤醒的任务数量
  ******************************************************************************************************************/
uint32_t wSemDestroy (wSem * sem)
{       
    uint32_t status = wTaskEnterCritical(); 

    uint32_t count = wEventRemoveAll(&sem->event, (void *)0, wErrorDel);  
    sem->count = 0;
    wTaskExitCritical(status);
    
    if (count > 0) 
    {
        wTaskSched();
    } 
    return count;  
}
