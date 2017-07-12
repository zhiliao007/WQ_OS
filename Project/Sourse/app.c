#include "WQ_OS.h"

int task1Flag;
int task2Flag;
int task3Flag;

wTask wTask1;
wTask wTask2;
wTask wTask3;

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];
wTaskStack Task3Env[1024];

/*******************************************************************************************************************
  * @brief  ����1��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task1Entry(void * param)
{
	wSetSysTickPeriod(10);
	for(;;)
	{
		task1Flag = 0;
        wTaskSuspend(currentTask);
		task1Flag = 1;
		wTaskSuspend(currentTask);
	}
}

/*******************************************************************************************************************
  * @brief  ����2��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		wTaskDelay(1);
		wTaskWakeUp(&wTask1);
		task2Flag = 1;
		wTaskDelay(1);
		wTaskWakeUp(&wTask1);
	}
}

/*******************************************************************************************************************
  * @brief  ����3��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task3Entry(void * param)
{
	for(;;)
	{
		task3Flag = 0;
		wTaskDelay(1);
		task3Flag = 1;
		wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  �����ʼ������
  * @param  ��
  * @retval ��
  ******************************************************************************************************************/
void wInitApp(void)
{
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, 0, &Task1Env[1024]);	//��ʼ������
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, 1, &Task2Env[1024]);
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 1, &Task3Env[1024]);
	
}
