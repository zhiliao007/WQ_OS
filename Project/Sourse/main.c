#include "WQ_OS.h"

wTask * currentTask;
wTask * nextTask;
wTask * taskTable[2];

/**
  * @brief  任务初始化函数
  * @param  task： 任务结构体地址
			entry：任务入口函数名
			param：传递给任务的参数
            stack：任务栈空间地址
  * @retval 无
  */	
void wTaskInit(wTask * task, void (*entry)(void *), void * param, wTaskStack * stack)
{
	*(--stack) = (unsigned long)(1<<24);                // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stack) = (unsigned long)entry;                  // R15(PC) 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
														// R14内核寄存器用来保存函数的返回值地址
    *(--stack) = (unsigned long)0x12;                   // R12, 未用
    *(--stack) = (unsigned long)0x3;                    // R3,  未用
    *(--stack) = (unsigned long)0x2;                    // R2,  未用
    *(--stack) = (unsigned long)0x1;                    // R1,  未用
    *(--stack) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
	                                                    // 函数的第一个参数的地址保存在R0内核寄存器
	
    *(--stack) = (unsigned long)0x11;                   // R11, 未用
    *(--stack) = (unsigned long)0x10;                   // R10, 未用
    *(--stack) = (unsigned long)0x9;                    // R9,  未用
    *(--stack) = (unsigned long)0x8;                    // R8,  未用
    *(--stack) = (unsigned long)0x7;                    // R7,  未用
    *(--stack) = (unsigned long)0x6;                    // R6,  未用
    *(--stack) = (unsigned long)0x5;                    // R5,  未用
    *(--stack) = (unsigned long)0x4;                    // R4,  未用
	 
	task->stack = stack;								// 保存最终的值，使任务栈指针指向缓冲区栈顶
}

/**
  * @brief  任务调度函数
  * @param  无
  * @retval 无
  */	
void wTaskSched()
{
	if (currentTask == taskTable[0])
	{
		nextTask = taskTable[1];
	}
	else
	{
		nextTask = taskTable[0];
	}
	wTaskSwitch();
}

/**
  * @brief  粗暴延时函数
  * @param  count： 延时时间
  * @retval 无
  */	
void delay(int count)
{
     while(--count>0);
}

wTask wTask1;
wTask wTask2;

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];

int task1Flag;
int task2Flag;

/**
  * @brief  任务1入口函数
  * @param  param：传给任务的参数
  * @retval 无
  */	
void task1Entry(void * param)
{
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
		wTaskSched();
	}
}

/**
  * @brief  任务2入口函数
  * @param  param：传给任务的参数
  * @retval 无
  */	
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		wTaskSched();
	}
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */	
int main()
{
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, &Task1Env[1024]);	//初始化任务
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, &Task2Env[1024]);
	
	taskTable[0] = &wTask1;		//初始化任务列表
	taskTable[1] = &wTask2;

    nextTask = taskTable[0];	//第一个要运行的任务指向任务1
	
	wTaskRunFirst();			//将CPU控制权交给OS，开始运行系统
	
	return 0;
}
