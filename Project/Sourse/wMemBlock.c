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
	
	wListInit(&memBlock->blocklist);
	while(memBlockStart < memBlockEnd)
	{
		wNodeInit((wNode *)memBlockStart);
		wListAddLast(&memBlock->blocklist,(wNode *)memBlockStart);
		
		memBlockStart += blockSize;
	}
}