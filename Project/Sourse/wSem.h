#ifndef WSEM_H
#define WSEM_H

#include "wEvent.h"

typedef struct _wSem    //定义信号量类型
{
	wEvent event;       //事件控制块，wSem同时是一个wEvent
	
	uint32_t count;     // 当前的计数值

	uint32_t maxCount;  // 最大计数值
}wSem;

void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount);

#endif
