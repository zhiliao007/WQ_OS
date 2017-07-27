#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  初始化邮箱函数
  * @param  mbox        邮箱结构指针
			msgBuffer   消息存储缓冲区
			maxCount    最大消息计数值 
  * @retval 无
  ******************************************************************************************************************/	
void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount)
{
	wEventInit(&mbox->event, wEventTypeMbox);
	
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}
