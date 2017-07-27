#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ�����亯��
  * @param  mbox        ����ṹָ��
			msgBuffer   ��Ϣ�洢������
			maxCount    �����Ϣ����ֵ 
  * @retval ��
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
