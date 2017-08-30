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

typedef struct _wMemBlockInfo   //定义存储块信息结构
{
	uint32_t count;             //当前存储块的计数
	uint32_t maxCount;          //允许的存储块最大计数
	uint32_t blockSize;         //每个存储块的大小
	uint32_t taskCount;         //当前等待的任务计数
}wMemBlockInfo;

void wMemBlockInit(wMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t wMemBlockWait(wMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t wMemBlockNoWaitGet(wMemBlock * memBlock, void **mem);
void wMemBlockNotify(wMemBlock * memBlock, uint8_t * mem);
void wMemBlockGetInfo(wMemBlock * memBlock, wMemBlockInfo * info);
uint32_t wMemBlockDestroy(wMemBlock * memBlock);

#endif
