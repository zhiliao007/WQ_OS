#ifndef WEVENT_H
#define WEVENT_H

#include "wLib.h"
#include "wConfig.h"

typedef enum  _wEventType {   
    wEventTypeUnknown   = 0, 				// 未知类型
 }wEventType;

// Event控制结构
typedef struct _wEvent {
    wEventType type;						// Event类型

    wList waitList;							// 任务等待列表
}wEvent;

void wEventInit(wEvent * event, wEventType type);

#endif
