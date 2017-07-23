#ifndef WSEM_H
#define WSEM_H

#include "wEvent.h"

typedef struct _wSem    //�����ź�������
{
	wEvent event;       //�¼����ƿ飬wSemͬʱ��һ��wEvent
	
	uint32_t count;     // ��ǰ�ļ���ֵ

	uint32_t maxCount;  // ������ֵ
}wSem;

typedef struct _wSemInfo  //�ź�������Ϣ����
{
    uint32_t count;       // ��ǰ�ź����ļ���

    uint32_t maxCount;    // �ź��������������

    uint32_t taskCount;   // ��ǰ�ȴ���������
}wSemInfo;

void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount);
uint32_t wSemWait(wSem * sem, uint32_t waitTicks);
uint32_t wSemNoWaitGet (wSem * sem);
void wSemNotify (wSem * sem);
void wSemGetInfo (wSem * sem, wSemInfo * info);
uint32_t wSemDestroy (wSem * sem);

#endif
