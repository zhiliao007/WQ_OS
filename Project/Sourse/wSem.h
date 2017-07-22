#ifndef WSEM_H
#define WSEM_H

#include "wEvent.h"

typedef struct _wSem    //�����ź�������
{
	wEvent event;       //�¼����ƿ飬wSemͬʱ��һ��wEvent
	
	uint32_t count;     // ��ǰ�ļ���ֵ

	uint32_t maxCount;  // ������ֵ
}wSem;

void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount);

#endif
