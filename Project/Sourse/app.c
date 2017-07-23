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

wSem sem1;
wSem sem2;

/*******************************************************************************************************************
  * @brief  任务1入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task1Entry(void * param)
{
	uint32_t status = 0;
	wSetSysTickPeriod(10);
	
	// 最大10个信号量计数，初始化值为0
    wSemInit(&sem1, 0, 10);
    
    // 等待信号量，sem1将由task2删除后，才能继续恢复运行
    status = wSemWait(&sem1, 0);
	for(;;)
	{		
		task1Flag = 0;
        wTaskDelay(1);
        task1Flag = 1;
        wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  任务2入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task2Entry(void * param)
{
	wSemInfo semInfo;
    int destroyed = 0;

	for(;;)
	{
		task2Flag = 0;
        wTaskDelay(1);
        task2Flag = 1;
        wTaskDelay(1);
		wSemNotify(&sem1);
		//删除信号量，将唤醒task1
        if (!destroyed) 
		{
            wSemGetInfo(&sem1, &semInfo);
            wSemDestroy(&sem1);
            destroyed = 1;
        }
	}
}

/*******************************************************************************************************************
  * @brief  任务3入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task3Entry(void * param)
{
	 wSemInit(&sem2, 0, 0);
	for(;;)
	{
		wSemWait(&sem2, 10);
		task3Flag = 0;
		wTaskDelay(1);
		task3Flag = 1;
		wTaskDelay(1);
	}
}

/*******************************************************************************************************************
  * @brief  任务4入口函数
  * @param  param：传给任务的参数
  * @retval 无
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
  * @brief  任务初始化函数
  * @param  无
  * @retval 无
  ******************************************************************************************************************/
void wInitApp(void)
{
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, 0, &Task1Env[1024]);	//初始化任务
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, 1, &Task2Env[1024]);
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 1, &Task3Env[1024]);
	wTaskInit(&wTask4, task4Entry, (void *)0x44444444, 1, &Task4Env[1024]);
	
}
