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

typedef struct _wMutexInfo       //定义互斥信号量信息结构
{
	uint32_t taskCount;          //等待的任务数量
	uint32_t ownerPrio;          //拥有者原始优先级
	uint32_t inheritedPrio;      //继承者优先级
	wTask * owner;               //拥有者
	uint32_t lockedCount;        //锁定次数
}wMutexInfo;

void wMutexInit(wMutex * mutex);
uint32_t wMutexWait(wMutex * mutex, uint32_t waitTicks);
uint32_t wMutexNoWaitGet(wMutex * mutex);
uint32_t wMutexNotify(wMutex * mutex);
uint32_t wMutexDestroy(wMutex * mutex);
void wMutexGetInfo(wMutex * mutex, wMutexInfo * info);

#endif
