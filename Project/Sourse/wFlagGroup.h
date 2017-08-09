#ifndef WFLAGGROUP_H
#define WFLAGGROUP_H

#include "wEvent.h"

typedef struct _wFlagGroup     //定义事件标志组类型
{
	wEvent event;              //事件控制块，wFlagGroup同时是一个wEvent
	uint32_t flag;             //当前事件标志
}wFlagGroup; 

#define WFLAGGROUP_CLEAR       (0x0 << 0)
#define WFLAGGROUP_SET         (0x1 << 0)
#define WFLAGGROUP_ANY         (0x0 << 1)
#define WFLAGGROUP_ALL         (0x1 << 1)

#define WFLAGGROUP_SET_ALL     (WFLAGGROUP_SET | WFLAGGROUP_ALL)
#define WFLAGGROUP_SET_ANY     (WFLAGGROUP_SET | WFLAGGROUP_ANY)
#define WFLAGGROUP_CLEAR_ALL   (WFLAGGROUP_CLEAR | WFLAGGROUP_ALL)
#define WFLAGGROUP_CLEAR_ANY   (WFLAGGROUP_CLEAR | WFLAGGROUP_ANY)

#define WFLAGGROUP_CONSUME     (1 << 7)

void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags);

uint32_t wFlagGroupWait(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks);
uint32_t wFlagGroupNoWaitGet(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag);
void wFlagGroupNotify(wFlagGroup * flagGroup, uint8_t isSet, uint32_t flag);

#endif
