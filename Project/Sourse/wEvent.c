#include "WQ_OS.h"

void wEventInit(wEvent * event,wEventType type)
{
	event->type = wEventTypeUnknown;
	wListInit(&event->waitList);
}
