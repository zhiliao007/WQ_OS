#ifndef WQ_OS_H
#define WQ_OS_H

#include <stdint.h>

typedef uint32_t wTaskStack;

typedef struct _wTask
{
	wTaskStack * stack;
}wTask;

extern wTask * currentTask;
extern wTask * nextTask;

void wTaskRunFirst(void);
void wTaskSwitch(void);

#endif
