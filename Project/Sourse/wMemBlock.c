#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化存储块函数
  * @param  memBlock    存储块结构指针
			memStart    存储块首地址
			blockSize   每个存储块大小 
			blockCnt    存储块个数
  * @retval 无
  ******************************************************************************************************************/	
void wMemBlockInit(wMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt)
{
	uint8_t * memBlockStart = (uint8_t *)memStart;
	uint8_t * memBlockEnd = memBlockStart + blockSize * blockCnt;
	
	if(blockSize < sizeof(wNode))
	{
		return;
	}
	
	wEventInit(&memBlock->event, wEventTypeMemBlock);
	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxCount = blockCnt;
	
	wListInit(&memBlock->blockList);
	while(memBlockStart < memBlockEnd)
	{
		wNodeInit((wNode *)memBlockStart);
		wListAddLast(&memBlock->blockList,(wNode *)memBlockStart);
		
		memBlockStart += blockSize;
	}
}

/*******************************************************************************************************************
  * @brief  等待存储块函数
  * @param  memBlock    存储块结构指针
			mem        存储块存储的地址
			waitTicks   等待的最大ticks数
  * @retval 等待结果    wErrorResourceUnavaliable    资源不可用
                        wErrorNoError                没有错误
                        wErrorTimeout                等待超时
  ******************************************************************************************************************/	
uint32_t wMemBlockWait(wMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)wListRemoveFirst(&memBlock->blockList);
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else
	{
		wEventWait(&memBlock->event, currentTask, (void *)0, wEventTypeMemBlock, waitTicks);
		wTaskExitCritical(status);
		
		wTaskSched();
		
		*mem = currentTask->eventMsg;
		return currentTask->waitEventResult;
			
	}
}

/*******************************************************************************************************************
  * @brief  获取存储块函数
  * @param  memBlock    存储块结构指针
			mem        存储块存储的地址 
  * @retval 等待结果     wErrorNoError 
	                     wErrorTimeout             
	                     wErrorResourceUnavaliable
  ******************************************************************************************************************/	
uint32_t wMemBlockNoWaitGet(wMemBlock * memBlock, void **mem)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)wListRemoveFirst(&memBlock->blockList);
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
  * @brief  存储块唤醒任务函数
  * @param  memBlock    存储块结构指针
			mem        存储块存储的地址 
  * @retval 无
  ******************************************************************************************************************/
void wMemBlockNotify(wMemBlock * memBlock, uint8_t * mem)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wEventWaitCount(&memBlock->event) > 0)
	{
		wTask * task = wEventWakeUp(&memBlock->event, (void *)mem, wErrorNoError);
		if(task->prio < currentTask->prio)
		{
			wTaskSched();
		}
	}
	else
	{
		wListAddLast(&memBlock->blockList, (wNode *)mem);
	}
	wTaskExitCritical(status);
	
}
