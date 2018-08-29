/*
 * @file wMbox.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   邮箱
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "WQ_OS.h"

#if WQ_OS_ENABLE_MBOX == 1

/*!
 * @brief  初始化邮箱函数
 * @param  mbox        邮箱结构指针
 * @param  msgBuffer   消息存储缓冲区
 * @param  maxCount    最大消息计数值 
 * @retval 无
 */	
void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount)
{
	wEventInit(&mbox->event, wEventTypeMbox);
	
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}

/*!
 * @brief  等待邮箱获取消息函数
 * @param  mbox        邮箱结构指针
 * @param  msg         消息存储缓存区
 * @param  waitTicks   等待的最大ticks数 
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
uint32_t wMboxWait(wMbox * mbox, void **msg, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else 
	{
		wEventWait(&mbox->event, currentTask, (void *)0, wEventTypeMbox, waitTicks);
		wTaskExitCritical(status);
		
		wTaskSched();
		
		*msg = currentTask->eventMsg;
		return currentTask ->waitEventResult;
	}
	
}

/*！
 * @brief  无等待的获取邮箱消息函数
 * @param  mbox        邮箱结构指针
 * @param  msg         消息存储缓存区
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
uint32_t wMboxNoWaitGet(wMbox * mbox,void **msg)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else 
	{
		wTaskExitCritical(status);
		return wErrorResourceUnavaliable;
    }
}

/*！
 * @brief  邮箱唤醒任务函数
 * @param  mbox          邮箱结构指针
 * @param  msg           消息存储缓存区
 * @param  notifyOption  发送的选项
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
uint32_t wMboxNotify(wMbox * mbox, void *msg, uint32_t notifyOption)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wEventWaitCount(&mbox->event) > 0)
	{
		wTask *task = wEventWakeUp(&mbox->event, (void *)msg, wErrorNoError);
		if(task->prio < currentTask->prio)
		{
			wTaskSched();
		}
	}
	else 
	{
		if(mbox->count >= mbox->maxCount)
		{
			wTaskExitCritical(status);
			return wErrorResourseFull;
		}
		
		if(notifyOption & wMBOXSendFront)
		{
			if(mbox->read <= 0)
			{
				mbox->read = mbox->maxCount - 1;
			}
			else
			{
				--mbox->read;
			}
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		mbox->count++;
	}
	
	wTaskExitCritical(status);
	return wErrorNoError;
}

/*！
 * @brief  清空邮箱函数
 * @param  mbox  邮箱结构指针
 * @retval 无
 */	
void wMboxFlush(wMbox * mbox)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wEventWaitCount(&mbox->event) == 0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
	wTaskExitCritical(status);
}
	
/*！
 * @brief  删除邮箱函数
 * @param  mbox        邮箱结构指针
 * @retval 邮箱中任务数量
 */	
uint32_t wMboxDestory(wMbox * mbox)
{
	uint32_t status = wTaskEnterCritical();
	uint32_t count = wEventRemoveAll(&mbox->event,(void *)0, wErrorDel);
	wTaskExitCritical(status);
	
	if(count > 0)
	{
		wTaskSched();
	}
	return count;
}

/*！
 * @brief  邮箱状态查询函数
 * @param  mbox   邮箱结构指针
 * @param  info   状态查询结构指针
 * @retval 无
 */	
void wMboxGetInfo(wMbox * mbox, wMboxInfo * info)
{
	uint32_t status = wTaskEnterCritical();        
    
    info->count = mbox->count;
    info->maxCount = mbox->maxCount;
    info->taskCount = wEventWaitCount(&mbox->event);
    
    wTaskExitCritical(status);
}

#endif
