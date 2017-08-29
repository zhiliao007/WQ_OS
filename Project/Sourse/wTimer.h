#ifndef WTIMER_H
#define WTIMER_H

#include "wEvent.h"

typedef enum _wTimerState               //��ʱ��״̬����
{
	timerCreated,                       //��ʱ���Ѵ���
	timerStarted,                       //��ʱ��������
	timerRunning,                       //��ʱ������ִ�лص�����
	timerStopped,                       //��ʱ����ֹͣ
	timerDestroyed                      //��ʱ��������
}wTimerState;

typedef struct _wTimer                  //���������ʱ������
{
	wNode linkNode;                     //������
	uint32_t startDelayTicks;           //�����������ticks��
	uint32_t durationTicks;             //���ڶ�ʱʱ��ticks��
	uint32_t delayTicks;                //��ǰ��ʱ�ݼ�����ֵ
	void (*timerFunc) (void * arg);     //��ʱ�ص�����
	void * arg;                         //�����ص������Ĳ���
	uint32_t config;                    //��ʱ�����ò���
	wTimerState state;                  //��ʱ��״̬
}wTimer;

#define TIMER_CONFIG_TYPE_HARD        (1 << 0)    //Ӳ����ʱ�����жϺ����
#define TIMER_CONFIG_TYPE_SOFT        (0 << 0)    //�����ʱ�����������

void wTimerInit(wTimer * timer, uint32_t delayTicks, uint32_t derationTicks, void(*timerFunc) (void * arg), void * arg, uint32_t config);

#endif
