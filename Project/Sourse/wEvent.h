#ifndef WEVENT_H
#define WEVENT_H

#include "wLib.h"
#include "wConfig.h"
#include "wTask.h"

/*! 
 * @brief 事件类型
 */
typedef enum  _wEventType       
{   
    wEventTypeUnknown,  	/*!< 未知类型 */
	wEventTypeSem,          /*!< 信号量类型 */
	wEventTypeMbox,         /*!< 邮箱类型 */ 
	wEventTypeMemBlock,     /*!< 存储块类型 */
	wEventTypeFlagGroup,    /*!< 事件标志组类型 */ 
	wEventTypeMutex,        /*!< 互斥信号量类型 */
}wEventType;

/*! 
 * @brief 事件控制结构
 */
typedef struct _wEvent          
{
    wEventType type;			/*!< Event类型 */
    wList waitList;				/*!< 任务等待列表 */
}wEvent;

void wEventInit(wEvent * event, wEventType type);
void wEventWait (wEvent * event, wTask * task, void * msg, uint32_t state, uint32_t timeout);
wTask * wEventWakeUp (wEvent * event, void * msg, uint32_t result);
wTask * wEventWakeUpTask(wEvent * event, wTask * task, void * msg, uint32_t result);
void wEventRemoveTask (wTask * task, void * msg, uint32_t result);
uint32_t wEventRemoveAll (wEvent * event, void * msg, uint32_t result);
uint32_t wEventWaitCount (wEvent * event);

#endif
