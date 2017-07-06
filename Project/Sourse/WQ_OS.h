#ifndef WQ_OS_H
#define WQ_OS_H

#include <stdint.h>

#include "wConfig.h"
#include "wLib.h"

#define WQOS_TASK_STATE_RDY              0
#define WQOS_TASK_STATE_DELAYED          (1 << 1)

typedef uint32_t wTaskStack;	//定义任务堆栈类型

typedef struct _wTask        //任务结构
{								
	wTaskStack * stack;		 //任务堆栈指针
	uint32_t delayTicks;	 //任务延时个数 
	wNode delayNode;         //通用延时结点结构
	uint32_t prio;           //任务优先级
	uint32_t state;          //状态字段，用于判断是不是处于延时状态
}wTask;

extern wTask * currentTask;
extern wTask * nextTask;

void wTaskRunFirst(void);
void wTaskSwitch(void);

uint32_t wTaskEnterCritical(void);
void wTaskExitCritical(uint32_t status);

void wTaskSchedInit(void);	
void wTaskSchedDisable(void);	
void wTaskSchedEnable(void);
void wTaskSched(void);

#endif
