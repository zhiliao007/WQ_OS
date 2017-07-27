#ifndef WMBOX_H
#define WMBOX_H

#include "wEvent.h"

typedef struct _wMbox        //定义邮箱类型
{
	wEvent event;            //事件控制块，wMbox同时是一个wEvent
	
	uint32_t count;          //当前消息数
	
	uint32_t read;           //读索引
	
	uint32_t write;          //写索引
	
	uint32_t maxCount;       //最大消息数
	
	void ** msgBuffer;       //消息存储缓冲区
}wMbox;

void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount);

#endif
