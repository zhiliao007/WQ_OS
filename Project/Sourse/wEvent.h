#ifndef WEVENT_H
#define WEVENT_H

#include "wLib.h"
#include "wConfig.h"
#include "wTask.h"

typedef enum  _wEventType       //事件类型
{   
    wEventTypeUnknown   = 0, 	// 未知类型
}wEventType;

typedef struct _wEvent          //事件控制结构
{
    wEventType type;			// Event类型

    wList waitList;				// 任务等待列表
}wEvent;

void wEventInit(wEvent * event, wEventType type);
void wEventWait (wEvent * event, wTask * task, void * msg, uint32_t state, uint32_t timeout);
wTask * wEventWakeUp (wEvent * event, void * msg, uint32_t result);
void wEventRemoveTask (wTask * task, void * msg, uint32_t result);

#endif
