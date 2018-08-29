/*
 * @file wSem.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   信号量
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "wSem.h"
#include "WQ_OS.h"

#if WQ_OS_ENABLE_SEM == 1

/*!
 * @brief  初始化信号量函数
 * @param  sem         信号量结构指针
 * @param  startCount  初始计数值
 * @param  maxCount    最大计数值，若为0则不限数量 
 * @retval 无
 */	
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

/*!
 * @brief  等待信号量函数
 * @param  sem       信号量结构指针
 * @param  waitTicks 等待的最大ticks数 ,当信号量计数为0时，等待的ticks数，为0时表示永远等待
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用
 */
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
		/* 当由于等待超时或者计数可用时，执行会返回到这里，然后取出等待结构 */
        return currentTask->waitEventResult;    
	}
}

/*!
 * @brief  获取信号量函数
 * @param  sem       信号量结构指针
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用
 */
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
	
/*!
 * @brief  信号量唤醒任务函数
 * @param  sem       信号量结构指针
 * @retval 无
 */
void wSemNotify (wSem * sem)
{
    uint32_t status = wTaskEnterCritical();        
    
	/* 检查是否有任务在等待 */
    if (wEventWaitCount(&sem->event) > 0)  
    {
        /* 此处唤醒的是队列首部的任务 */
        wTask * task = wEventWakeUp(&sem->event, (void *)0, wErrorNoError );  

        if (task->prio < currentTask->prio)
        {
            wTaskSched(); 
    	}
    }
    else
    {
		/* 可用资源（事件）+1 */
    	++sem->count;    

    	if ((sem->maxCount != 0) && (sem->count > sem->maxCount)) 
    	{	
    		sem->count = sem->maxCount;
    	}
    }
    wTaskExitCritical(status);
}

/*!
 * @brief  信号量状态查询函数
 * @param  sem    信号量结构指针
 * @param  info   状态查询结构指针
 * @retval 无
 */
void wSemGetInfo (wSem * sem, wSemInfo * info)
{
    uint32_t status = wTaskEnterCritical();        
    
    info->count = sem->count;
    info->maxCount = sem->maxCount;
    info->taskCount = wEventWaitCount(&sem->event);
    
    wTaskExitCritical(status);
}

/*!
 * @brief  销毁信号量函数
 * @param  sem       信号量结构指针
 * @retval 因销毁该信号量而唤醒的任务数量
 */
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

#endif
