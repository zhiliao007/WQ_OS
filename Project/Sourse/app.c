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

wMbox mbox1;
wMbox mbox2;
void * mbox1MsgBuffer[20];
void * mbox2MsgBuffer[20];
uint32_t msg[20];

/*******************************************************************************************************************
  * @brief  ����1��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task1Entry(void * param)
{
	wSetSysTickPeriod(10);
	
	wMboxInit(&mbox1, mbox1MsgBuffer, 20);
	for(;;)
	{	
		uint32_t i = 0;
        for (i = 0; i < 20; i++) 
        {
            msg[i] = i;
            wMboxNotify(&mbox1, &msg[i], wMBOXSendNormal);
        }
        wTaskDelay(100);
		
	    for (i = 0; i < 20; i++) 
        {
            msg[i] = i;
            wMboxNotify(&mbox1, &msg[i], wMBOXSendFront);
        }   
        wTaskDelay(100);	
		
		task1Flag = 0;
        wTaskDelay(1);
        task1Flag = 1;
        wTaskDelay(1);
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
		void * msg;
        uint32_t err = wMboxWait(&mbox1, &msg, 10);
        if (err == wErrorNoError) 
        {
            uint32_t value = *(uint32_t*)msg;
            task2Flag = value;
            wTaskDelay(1);
        }
		task2Flag = 0;
		wTaskDelay(1);
		task2Flag = 1;
		wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  ����3��ں���
  * @param  param����������Ĳ���
  * @retval ��
  ******************************************************************************************************************/
void task3Entry(void * param)
{
	 wMboxInit(&mbox2, mbox2MsgBuffer, 20);
	for(;;)
	{
		void * msg;
        wMboxWait(&mbox2, &msg, 100);

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
	for(;;)
	{
		task4Flag = 0;
		wTaskDelay(1);
		task4Flag = 1;
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
	wTaskInit(&wTask4, task4Entry, (void *)0x44444444, 1, &Task4Env[1024]);
	
}
