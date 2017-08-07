#ifndef WMEMBLOCK_H
#define WMEMBLOCK_H

#include"wEvent.h"

typedef struct _wMemBlock     //����洢������
{
	wEvent event;             //�¼����ƿ飬wMemBlockͬʱ��һ��wEvent
	void * memStart;          //�洢���׵�ַ
	uint32_t blockSize;       //�洢���С
	uint32_t maxCount;        //�洢��������
	wList blockList;          //�洢������
}wMemBlock;

typedef struct _wMemBlockInfo   //����洢����Ϣ�ṹ
{
	uint32_t count;             //��ǰ�洢��ļ���
	uint32_t maxCount;          //����Ĵ洢��������
	uint32_t blockSize;         //ÿ���洢��Ĵ�С
	uint32_t taskCount;         //��ǰ�ȴ����������
}wMemBlockInfo;

void wMemBlockInit(wMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t wMemBlockWait(wMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t wMemBlockNoWaitGet(wMemBlock * memBlock, void **mem);
void wMemBlockNotify(wMemBlock * memBlock, uint8_t * mem);
void wMemBlockGetInfo(wMemBlock * memBlock, wMemBlockInfo * info);
uint32_t wMemBlockDestroy(wMemBlock * memBlock);

#endif
