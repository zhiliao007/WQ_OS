#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ�������ʱ������
  * @param  timer �ȴ���ʼ���Ķ�ʱ���ṹָ��
			delayTicks ��ʱ����ʼ��������ʱticks����
			durationTicks �������Զ�ʱ���õ�����tick����һ���Զ�ʱ����Ч
			timerFunc ��ʱ���ص�����
			arg ���ݸ���ʱ���ص������Ĳ���
			timerFunc ��ʱ���ص�����
			config ��ʱ���ĳ�ʼ����
  * @retval ��
  ******************************************************************************************************************/	
void wTimerInit(wTimer * timer, uint32_t delayTicks, uint32_t durationTicks, void(*timerFunc) (void * arg), void * arg, uint32_t config)
{
	wNodeInit(&timer->linkNode);
	timer->startDelayTicks = delayTicks;
	timer->durationTicks = durationTicks;
	timer->timerFunc = timerFunc;
	timer->arg = arg;
	timer->config = config;
	
	if(delayTicks == 0)
	{
		timer->delayTicks = durationTicks;
	}
	else
	{
		timer->delayTicks = timer->startDelayTicks;
	}
	
	timer->state = timerCreated;
}
