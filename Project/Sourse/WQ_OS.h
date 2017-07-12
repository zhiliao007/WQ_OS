#ifndef WQ_OS_H
#define WQ_OS_H

#include <stdint.h>

#include "wConfig.h"
#include "wLib.h"

#define WQOS_TASK_STATE_RDY              0
#define WQOS_TASK_STATE_DELAYED          (1 << 1)
#define WQOS_TASK_STATE_SUSPEND          (1 << 2)

typedef uint32_t wTaskStack;	//定义任务堆栈类型

typedef struct _wTask        //任务结构
{								
	wTaskStack * stack;		 //任务堆栈指针
	wNode linkNode;          //优先级队列链接结点
	uint32_t delayTicks;	 //任务延时个数 
	wNode delayNode;         //通用延时结点结构
	uint32_t prio;           //任务优先级
	uint32_t state;          //状态字段，用于判断是不是处于延时状态
	uint32_t slice;          //时间片计数器
	uint32_t suspendCount;   //挂起状态计数器
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
void wTaskSchedRdy(wTask * task);
void wTaskSchedUnRdy(wTask * task);
void wTaskSched(void);
void wTimeTaskWait(wTask * task, uint32_t ticks);
void wTimeTaskWakeUp(wTask * task);
void wTaskSystemTickHandler(void);
void wTaskDelay(uint32_t delay);
void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack);
void wSetSysTickPeriod(uint32_t ms);
void wInitApp(void);
void wTaskSuspend(wTask * task);
void wTaskWakeUp(wTask * task);

#endif
