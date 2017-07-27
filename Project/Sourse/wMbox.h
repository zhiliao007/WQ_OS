#ifndef WMBOX_H
#define WMBOX_H

#include "wEvent.h"

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

#endif
