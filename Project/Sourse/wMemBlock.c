#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ���洢�麯��
  * @param  memBlock    �洢��ṹָ��
			memStart    �洢���׵�ַ
			blockSize   ÿ���洢���С 
			blockCnt    �洢�����
  * @retval ��
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
  * @brief  �ȴ��洢�麯��
  * @param  memBlock    �洢��ṹָ��
			mem        �洢��洢�ĵ�ַ
			waitTicks   �ȴ������ticks��
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
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
  * @brief  ��ȡ�洢�麯��
  * @param  memBlock    �洢��ṹָ��
			mem        �洢��洢�ĵ�ַ 
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
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
  * @brief  �洢�黽��������
  * @param  memBlock    �洢��ṹָ��
			mem        �洢��洢�ĵ�ַ 
  * @retval ��
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

/*******************************************************************************************************************
  * @brief  �洢��״̬��ѯ����
  * @param  memBlock    �洢��ṹָ��
			info        ״̬��ѯ�ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
void wMemBlockGetInfo(wMemBlock * memBlock, wMemBlockInfo * info)
{
	uint32_t status = wTaskEnterCritical();
	
	info->count = wListCount(&memBlock->blockList);
	info->maxCount = memBlock->maxCount;
	info->blockSize = memBlock->blockSize;
	info->taskCount = wEventWaitCount(&memBlock->event);
	
	wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  ɾ���洢�麯��
  * @param  memBlock    �洢��ṹָ��
  * @retval �洢������������
  ******************************************************************************************************************/	
uint32_t wMemBlockDestroy(wMemBlock * memBlock)
{
	uint32_t status = wTaskEnterCritical();
	uint32_t count = wEventRemoveAll(&memBlock->event, (void *)0, wErrorDel);
	wTaskExitCritical(status);
	
	if(count > 0)
	{
		wTaskSched();
	}
	return count;
}
