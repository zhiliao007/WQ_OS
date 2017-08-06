#ifndef WMEMBLOCK_H
#define WMEMBLOCK_H

#include"wEvent.h"

typedef struct _wMemBlock     //定义存储块类型
{
	wEvent event;             //事件控制块，wMemBlock同时是一个wEvent
	void * memStart;          //存储块首地址
	uint32_t blockSize;       //存储块大小
	uint32_t maxCount;        //存储块最大个数
	wList blockList;          //存储块链表
}wMemBlock;

void wMemBlockInit(wMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t wMemBlockWait(wMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t wMemBlockNoWaitGet(wMemBlock * memBlock, void **mem);
void wMemBlockNotify(wMemBlock * memBlock, uint8_t * mem);

#endif
