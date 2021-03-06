#ifndef WQ_OS_H
#define WQ_OS_H

#include <stdint.h>

#include "wConfig.h"

#include "wLib.h"

#include "wTask.h"

#include "wEvent.h"

#include "wSem.h"

#include "wMbox.h"

#include "wMemBlock.h"

#include "wFlagGroup.h"

#include "wMutex.h"

#include "wTimer.h"

#include "wHooks.h"

#define TICKS_PER_SEC (1000 / WQ_OS_SYSTICK_MS)

/*! 
 * @brief WQ_OS的错误码
 */
typedef enum _wError           
{
	wErrorNoError = 0,         /*!< 没有错误 */
	wErrorTimeout,             /*!< 超时 */
	wErrorResourceUnavaliable, /*!< 没有资源可用 */
	wErrorDel,                 /*!< 删除任务 */
	wErrorResourseFull,        /*!< 资源已满 */
	wErrorOwner,               /*!< 不匹配的所有者 */
}wError;

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
void wTaskSchedRemove(wTask * task);
void wTaskSched(void);
void wTimeTaskWait(wTask * task, uint32_t ticks);
void wTimeTaskWakeUp(wTask * task);
void wTimeTaskRemove(wTask * task);
void wTaskSystemTickHandler(void);
void wTaskDelay(uint32_t delay);


void wSetSysTickPeriod(uint32_t ms);
void wInitApp(void);

float tCpuUsageGet (void);

#endif
