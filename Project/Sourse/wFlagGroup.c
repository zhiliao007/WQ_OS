/*
 * @file wFlagGroup.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   事件标志组
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "WQ_OS.h"

#if WQ_OS_ENABLE_FLAGGROUP == 1

/*!
 * @brief  初始化事件标志组函数
 * @param  flagGroup   事件标志组结构指针
 * @param  flags       事件标志
 * @retval 无
 */	
void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags)
{
	wEventInit(&flagGroup->event, wEventTypeFlagGroup);
	flagGroup->flag = flags;
}

/*!
 * @brief  检查并消耗掉事件标志函数
 * @param  flagGroup   事件标志组结构指针
 * @param  type        事件标志检查类型
 * @param  flags       事件标志
 * @retval wErrorNoError 事件匹配
 * @retval wErrorResourceUnavaliable 事件未匹配
 */	
static uint32_t wFlagGroupCheckAndConsume(wFlagGroup * flagGroup, uint32_t type, uint32_t * flags)
{
	uint32_t srcFlag = *flags;
	uint32_t isSet = type & WFLAGGROUP_SET;
	uint32_t isAll = type & WFLAGGROUP_ALL;
	uint32_t isConsume = type & WFLAGGROUP_CONSUME;
	
	uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);
	
	if(((isAll != 0) && (calFlag ==srcFlag)) || ((isAll == 0) && (calFlag != 0)))
	{
		if(isConsume)
		{
			if(isSet)
			{
				flagGroup->flag &= ~srcFlag;
			}
			else
			{
				flagGroup->flag |=srcFlag;
			}
		}
		*flags = calFlag;
		return wErrorNoError;
	}
	
	*flags = calFlag;
	return wErrorResourceUnavaliable;
}
	
/*!
 * @brief  等待事件标志组函数
 * @param  flagGroup   事件标志组结构指针
 * @param  waitType    等待事件的类型     
 * @param  requstFlag  请求的事件标志
 * @param  resultFlag  等待标志结果
 * @param  waitTicks   等待的最大ticks数
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
uint32_t wFlagGroupWait(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = requestFlag;
	uint32_t status = wTaskEnterCritical();
	
	result = wFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	if(result != wErrorNoError)
	{
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		wEventWait(&flagGroup->event, currentTask, (void *)0, wEventTypeFlagGroup, waitTicks);
		
		wTaskExitCritical(status);
		
		wTaskSched();
		
		*resultFlag = currentTask->eventFlags;
		result = currentTask->waitEventResult;
	}
	else
	{
		*resultFlag = flags;
		wTaskExitCritical(status);
	}
	return result;
}

/*!
 * @brief  获取事件标志组函数
 * @param  flagGroup   事件标志组结构指针
 * @param  waitType    等待事件的类型     
 * @param  requstFlag  请求的事件标志
 * @param  resultFlag  等待标志结果
 * @retval wErrorResourceUnavaliable  等待结果为资源不可用    
 * @retval wErrorNoError              等待结果为正确  
 * @retval wErrorTimeout              等待结果为超时   
 */	
uint32_t wFlagGroupNoWaitGet(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag)
{
	uint32_t flags = requestFlag;
	
	uint32_t status = wTaskEnterCritical();
	uint32_t result = wFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	wTaskExitCritical(status);
	
	*resultFlag = flags;
	return wErrorNoError;
}

/*!
 * @brief  事件标志组唤醒任务函数
 * @param  flagGroup   事件标志组结构指针
 * @param  isSet       是否是设置事件标志
 * @param  flags       产生的事件标志
 * @retval 无
 */	
void wFlagGroupNotify(wFlagGroup * flagGroup, uint8_t isSet, uint32_t flag)
{
	wList * waitList;
	wNode * node;
	wNode * nextNode;
	uint8_t sched = 0;
	uint32_t status = wTaskEnterCritical();
	
	if(isSet)
	{
		flagGroup->flag |= flag;
	}
	else
	{
		flagGroup->flag &= ~flag;
	}
	
	waitList = &flagGroup->event.waitList;
	for(node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode)
	{
		uint32_t result;
		wTask * task = wNodeParent(node, wTask, linkNode);
		uint32_t flags = task->eventFlags;
		nextNode = node->nextNode;
		
		result = wFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
		if(result == wErrorNoError)
		{
			task->eventFlags =flags;
			wEventWakeUpTask(&flagGroup->event, task, (void *)0, wErrorNoError);
			sched = 1;
		}
	}
	
	if(sched)
	{
		wTaskSched();
	}
	wTaskExitCritical(status);
}

/*!
 * @brief  存储块状态查询函数
 * @param  flagGroup   事件标志组结构指针
 * @param  info        状态查询结构指针
 * @retval 无
 */	
void wFlagGroupGetInfo(wFlagGroup * flagGroup, wFlagGroupInfo * info)
{
	uint32_t status = wTaskEnterCritical();
	
	info->flags = flagGroup->flag;
	info->taskCount = wEventWaitCount(&flagGroup->event);
	
	wTaskExitCritical(status);
}	

/*!
 * @brief  删除存储块函数
 * @param  flagGroup   事件标志组结构指针
 * @retval 存储块中任务数量
 */	
uint32_t wFlagGroupDestroy(wFlagGroup * flagGroup)
{
	uint32_t status = wTaskEnterCritical();
	uint32_t count = wEventRemoveAll(&flagGroup->event, (void *)0, wErrorDel);
	wTaskExitCritical(status);
	
	if(count > 0)
	{
		wTaskSched();
	}
	return count;
}

#endif
