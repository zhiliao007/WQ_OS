#ifndef WFLAGGROUP_H
#define WFLAGGROUP_H

#include "wEvent.h"

typedef struct _wFlagGroup     //�����¼���־������
{
	wEvent event;              //�¼����ƿ飬wFlagGroupͬʱ��һ��wEvent
	uint32_t flag;             //��ǰ�¼���־
}wFlagGroup;              

void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags);
	
#endif
