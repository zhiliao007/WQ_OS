#ifndef WQ_OS_H
#define WQ_OS_H

#include <stdint.h>

typedef uint32_t wTaskStack;	//定义任务堆栈类型

typedef struct _wTask        //任务结构
{								
	wTaskStack * stack;		 //任务堆栈指针
	uint32_t delayTicks;	 //任务延时个数
}wTask;

extern wTask * currentTask;
extern wTask * nextTask;

void wTaskRunFirst(void);
void wTaskSwitch(void);

uint32_t wTaskEnterCritical(void);
void wTaskExitCritical(uint32_t status);

#endif
