#ifndef WFLAGGROUP_H
#define WFLAGGROUP_H

#include "wEvent.h"

typedef struct _wFlagGroup     //定义事件标志组类型
{
	wEvent event;              //事件控制块，wFlagGroup同时是一个wEvent
	uint32_t flag;             //当前事件标志
}wFlagGroup;              

void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags);
	
#endif
