#ifndef WHOOKS_H
#define WHOOKS_H

#include "WQ_OS.h"

void wHooksCpuIdle(void);
void wHooksSysTick(void);
void wHooksTaskSwitch(wTask * from, wTask * to);
void wHooksTaskInit(wTask * task);

#endif
