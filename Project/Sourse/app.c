#include "WQ_OS.h"

int task1Flag;
int task2Flag;
int task3Flag;
int task4Flag;

wTask wTask1;
wTask wTask2;
wTask wTask3;
wTask wTask4;

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];
wTaskStack Task3Env[1024];
wTaskStack Task4Env[1024];

/*******************************************************************************************************************
  * @brief  ����������
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task1DestroyFunc (void * param) 
{
    task1Flag = 0;
}

/*******************************************************************************************************************
  * @brief  ����1��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task1Entry(void * param)
{
	wSetSysTickPeriod(10);
	
	 wTaskSetCleanCallFunc(currentTask, task1DestroyFunc, (void *)0);
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
	int task1Deleted = 0;
	
	for(;;)
	{
		task2Flag = 1;
        wTaskDelay(1);
        task2Flag = 0;
        wTaskDelay(1);

        // ����ɾ������1
        if (!task1Deleted) 
        {
            wTaskForceDelete(&wTask1);
            task1Deleted = 1;
        }
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
		// ����Ƿ�Ҫ��ɾ�������Լ�
        if (wTaskIsRequestedDelete())
        {
            // ��һЩ������
            task3Flag = 0;

            // Ȼ������ɾ���Լ�
            wTaskDeleteSelf();
        }
		
		task3Flag = 0;
		wTaskDelay(1);
		task3Flag = 1;
		wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  ����4��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task4Entry(void * param)
{
	int task3Deleted = 0;
	
	for(;;)
	{
		task4Flag = 0;
		wTaskDelay(1);
		task4Flag = 1;
		wTaskDelay(1);
		
		// ����ɾ������3
        if (!task3Deleted) 
        {
            wTaskRequestDelete(&wTask3);
            task3Deleted = 1;
        }
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
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 0, &Task3Env[1024]);
	wTaskInit(&wTask4, task4Entry, (void *)0x44444444, 1, &Task4Env[1024]);
	
}
