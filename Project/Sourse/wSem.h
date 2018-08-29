#ifndef WSEM_H
#define WSEM_H

#include "wEvent.h"

/*! 
 * @brief 定义信号量类型
 */
typedef struct _wSem    
{
	wEvent event;       /*!< 事件控制块，wSem同时是一个wEvent */ 
	
	uint32_t count;     /*!< 当前的计数值 */

	uint32_t maxCount;  /*!< 最大计数值 */
}wSem;

/*! 
 * @brief 定义信号量的信息类型
 */
typedef struct _wSemInfo  
{
    uint32_t count;       /*!< 当前信号量的计数 */

    uint32_t maxCount;    /*!< 信号量允许的最大计数 */

    uint32_t taskCount;   /*!< 当前等待的任务数 */
}wSemInfo;

void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount);
uint32_t wSemWait(wSem * sem, uint32_t waitTicks);
uint32_t wSemNoWaitGet (wSem * sem);
void wSemNotify (wSem * sem);
void wSemGetInfo (wSem * sem, wSemInfo * info);
uint32_t wSemDestroy (wSem * sem);

#endif
