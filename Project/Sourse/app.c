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
  * @brief  任务清理函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task1DestroyFunc (void * param) 
{
    task1Flag = 0;
}

/*******************************************************************************************************************
  * @brief  任务1入口函数
  * @param  param：传给任务的参数
  * @retval 无
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
  * @brief  任务2入口函数
  * @param  param：传给任务的参数
  * @retval 无
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

        // 请求删除任务1
        if (!task1Deleted) 
        {
            wTaskForceDelete(&wTask1);
            task1Deleted = 1;
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
	for(;;)
	{
		// 检查是否要求删除任务自己
        if (wTaskIsRequestedDelete())
        {
            // 做一些清理工作
            task3Flag = 0;

            // 然后主动删除自己
            wTaskDeleteSelf();
        }
		
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
	int task3Deleted = 0;
	
	for(;;)
	{
		task4Flag = 0;
		wTaskDelay(1);
		task4Flag = 1;
		wTaskDelay(1);
		
		// 请求删除任务3
        if (!task3Deleted) 
        {
            wTaskRequestDelete(&wTask3);
            task3Deleted = 1;
        }
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
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 0, &Task3Env[1024]);
	wTaskInit(&wTask4, task4Entry, (void *)0x44444444, 1, &Task4Env[1024]);
	
}
