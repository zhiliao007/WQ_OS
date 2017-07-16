#ifndef WEVENT_H
#define WEVENT_H

#include "wLib.h"
#include "wConfig.h"

typedef enum  _wEventType {   
    wEventTypeUnknown   = 0, 				// δ֪����
 }wEventType;

// Event���ƽṹ
typedef struct _wEvent {
    wEventType type;						// Event����

    wList waitList;							// ����ȴ��б�
}wEvent;

void wEventInit(wEvent * event, wEventType type);

#endif
