#ifndef WMUTEX_H
#define WMUTEX_H

#include "wEvent.h"

typedef struct _wMutex           //���廥���ź�������
{
	wEvent event;                //�¼����ƿ飬wMutexͬʱ��һ��wEvent
	uint32_t lockedCount;        //�ѱ������Ĵ���
	wTask * owner;               //ӵ����
	uint32_t ownerOriginalPrio;  //ӵ����ԭʼ�����ȼ�
}wMutex;

void wMutexInit(wMutex * mutex);

#endif
