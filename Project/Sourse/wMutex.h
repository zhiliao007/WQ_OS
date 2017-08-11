#ifndef WMUTEX_H
#define WMUTEX_H

#include "wEvent.h"

typedef struct _wMutex           //定义互斥信号量类型
{
	wEvent event;                //事件控制块，wMutex同时是一个wEvent
	uint32_t lockedCount;        //已被锁定的次数
	wTask * owner;               //拥有者
	uint32_t ownerOriginalPrio;  //拥有者原始的优先级
}wMutex;

void wMutexInit(wMutex * mutex);

#endif
