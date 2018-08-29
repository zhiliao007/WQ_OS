/*
 * @file app.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   用户
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 
 
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

/*!
 * @brief  粗暴延时函数
 * @param  count： 延时时间
 * @retval 无
 */
void delay(int count)
{
     while(--count>0);
}

/*!
 * @brief  任务1入口函数
 * @param  param：传给任务的参数
 * @retval 无
 */
void task1Entry(void * param)
{	
	for(;;)
	{	
		task1Flag = 0;
        wTaskDelay(1);
        task1Flag = 1;
        wTaskDelay(1);
		
	}
}

/*!
 * @brief  任务2入口函数
 * @param  param：传给任务的参数
 * @retval 无
 */
void task2Entry(void * param)
{
	for (;;) 
    {
        task2Flag = 0;
        wTaskDelay(1);
        task2Flag = 1;
        wTaskDelay(1);
    }
}

/*!
 * @brief  任务3入口函数
 * @param  param：传给任务的参数
 * @retval 无
 */
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

/*!
 * @brief  任务4入口函数
 * @param  param：传给任务的参数
 * @retval 无
 */
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
/*!
 * @brief  任务初始化函数
 * @param  无
 * @retval 无
 */
void wInitApp(void)
{
	/* 初始化任务 */
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, 0, Task1Env, sizeof(Task1Env));	
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, 1, Task2Env, sizeof(Task2Env));
	wTaskInit(&wTask3, task3Entry, (void *)0x22222222, 1, Task3Env, sizeof(Task3Env));
	wTaskInit(&wTask4, task4Entry, (void *)0x44444444, 1, Task4Env, sizeof(Task4Env));	
}
