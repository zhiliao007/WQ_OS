#ifndef WMEMBLOCK_H
#define WMEMBLOCK_H

#include"wEvent.h"

typedef struct _wMemBlock     //����洢������
{
	wEvent event;             //�¼����ƿ飬wMemBlockͬʱ��һ��wEvent
	void * memStart;          //�洢���׵�ַ
	uint32_t blockSize;       //�洢���С
	uint32_t maxCount;        //�洢��������
	wList blocklist;          //�洢������
}wMemBlock;

void wMemBlockInit(wMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
#endif
