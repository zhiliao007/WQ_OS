#ifndef WMBOX_H
#define WMBOX_H

#include "wEvent.h"

#define wMBOXSendNormal       0x00  
#define wMBOXSendFront        0x01

typedef struct _wMbox        //������������
{
	wEvent event;            //�¼����ƿ飬wMboxͬʱ��һ��wEvent
	
	uint32_t count;          //��ǰ��Ϣ��
	
	uint32_t read;           //������
	
	uint32_t write;          //д����
	
	uint32_t maxCount;       //�����Ϣ��
	
	void ** msgBuffer;       //��Ϣ�洢������
}wMbox;

void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount);
uint32_t wMboxWait(wMbox * mbox, void **msg, uint32_t waitTicks);
uint32_t wMboxNoWaitGet(wMbox * mbox,void **msg);
uint32_t wMboxNotify(wMbox * mbox, void *msg, uint32_t notifyOption);
void wMboxFlush(wMbox * mbox);
uint32_t wMboxDestory(wMbox * mbox);
#endif
