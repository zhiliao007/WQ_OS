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
	
	wListInit(&memBlock->blocklist);
	while(memBlockStart < memBlockEnd)
	{
		wNodeInit((wNode *)memBlockStart);
		wListAddLast(&memBlock->blocklist,(wNode *)memBlockStart);
		
		memBlockStart += blockSize;
	}
}