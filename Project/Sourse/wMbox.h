#ifndef WMBOX_H
#define WMBOX_H

#include "wEvent.h"

#define wMBOXSendNormal       0x00  
#define wMBOXSendFront        0x01

/*! 
 * @brief 定义邮箱类型
 */
typedef struct _wMbox        
{
	wEvent event;            /*!< 事件控制块，wMbox同时是一个wEvent */
	
	uint32_t count;          /*!< 当前消息数 */
	
	uint32_t read;           /*!< 读索引 */
	
	uint32_t write;          /*!< 写索引 */
	
	uint32_t maxCount;       /*!< 最大消息数 */
	
	void ** msgBuffer;       /*!< 消息存储缓冲区 */
}wMbox;

/*! 
 * @brief 定义邮箱信息结构
 */
typedef struct _wMboxInfo    
{
	uint32_t count;          /*!< 当前消息数 */
	uint32_t maxCount;       /*!< 最大消息数 */
	uint32_t taskCount;      /*!< 等待的任务数 */
}wMboxInfo;

void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount);
uint32_t wMboxWait(wMbox * mbox, void **msg, uint32_t waitTicks);
uint32_t wMboxNoWaitGet(wMbox * mbox,void **msg);
uint32_t wMboxNotify(wMbox * mbox, void *msg, uint32_t notifyOption);
void wMboxFlush(wMbox * mbox);
uint32_t wMboxDestory(wMbox * mbox);
void wMboxGetInfo(wMbox * mbox, wMboxInfo * info);

#endif
