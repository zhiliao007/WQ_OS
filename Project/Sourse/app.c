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

wFlagGroup flagGroup1;

/*******************************************************************************************************************
  * @brief  任务1入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task1Entry(void * param)
{	
    wSetSysTickPeriod(10);
	
	wFlagGroupInit(&flagGroup1, 0xFF);
	wTaskDelay(1);
	wFlagGroupDestroy(&flagGroup1);
	
	for(;;)
	{	
		task1Flag = 0;
        wTaskDelay(1);
        task1Flag = 1;
        wTaskDelay(1);
		
		wFlagGroupNotify(&flagGroup1, 0, 0x6);
	}
}

/*******************************************************************************************************************
  * @brief  任务2入口函数
  * @param  param：传给任务的参数
  * @retval 无
  ******************************************************************************************************************/
void task2Entry(void * param)
{
	uint32_t resultFlags = 0;
	wFlagGroupInfo info;
	wFlagGroupWait(&flagGroup1, WFLAGGROUP_SET_ALL | WFLAGGROUP_CONSUME, 0x1, &resultFlags, 0);
    wFlagGroupGetInfo(&flagGroup1, &info);
	wFlagGroupWait(&flagGroup1, WFLAGGROUP_SET_ALL | WFLAGGROUP_CONSUME, 0x1, &resultFlags, 0);

	for (;;) 
    {
    	wFlagGroupWait(&flagGroup1, WFLAGGROUP_CLEAR_ALL, 0x4, &resultFlags, 10);
        wFlagGroupNoWaitGet(&flagGroup1, WFLAGGROUP_CLEAR_ALL, 0x3, &resultFlags);

        task2Flag = 0;
        wTaskDelay(1);
        task2Flag = 1;
        wTaskDelay(1);
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
