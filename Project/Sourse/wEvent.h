#ifndef WEVENT_H
#define WEVENT_H

#include "wLib.h"
#include "wConfig.h"
#include "wTask.h"

typedef enum  _wEventType       //�¼�����
{   
    wEventTypeUnknown,  	// δ֪����
	wEventTypeSem,          //�ź�������
	wEventTypeMbox,         //��������
	wEventTypeMemBlock,     //�洢������
	wEventTypeFlagGroup,    //�¼���־������
}wEventType;

typedef struct _wEvent          //�¼����ƽṹ
{
    wEventType type;			// Event����

    wList waitList;				// ����ȴ��б�
}wEvent;

void wEventInit(wEvent * event, wEventType type);
void wEventWait (wEvent * event, wTask * task, void * msg, uint32_t state, uint32_t timeout);
wTask * wEventWakeUp (wEvent * event, void * msg, uint32_t result);
void wEventRemoveTask (wTask * task, void * msg, uint32_t result);
uint32_t wEventRemoveAll (wEvent * event, void * msg, uint32_t result);
uint32_t wEventWaitCount (wEvent * event);

#endif
