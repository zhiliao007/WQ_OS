#include "WQ_OS.h"

wTask * currentTask;
wTask * nextTask;
wTask * taskTable[2];

void wTaskInit(wTask * task, void (*entry)(void *), void * param, wTaskStack * stack)
{
	*(--stack) = (unsigned long)(1<<24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x3;
	*(--stack) = (unsigned long)0x2;
	*(--stack) = (unsigned long)0x1;
	*(--stack) = (unsigned long)param;
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x9;
	*(--stack) = (unsigned long)0x8;
	*(--stack) = (unsigned long)0x7;
	*(--stack) = (unsigned long)0x6;
	*(--stack) = (unsigned long)0x5;
	*(--stack) = (unsigned long)0x4;
	
	task->stack = stack;
}

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


void delay(int count)
{
    while(--count>0);
}

wTask wTask1;
wTask wTask2;

wTaskStack Task1Env[1024];
wTaskStack Task2Env[1024];

int task1Flag;
void task1Entry(void * param)
{
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
	}
}

int task2Flag;
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
	}
}
	
int main()
{
	wTaskInit(&wTask1, task1Entry, (void *)0x11111111, &Task1Env[1024]);
	wTaskInit(&wTask2, task2Entry, (void *)0x22222222, &Task2Env[1024]);
	
	taskTable[0] = &wTask1;
	taskTable[1] = &wTask2;

    nextTask = taskTable[0];
	
	wTaskRunFirst();
	
	return 0;
}
