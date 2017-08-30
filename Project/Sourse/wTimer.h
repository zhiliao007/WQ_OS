#ifndef WTIMER_H
#define WTIMER_H

#include "wEvent.h"

typedef enum _wTimerState               //定时器状态类型
{
	timerCreated,                       //定时器已创建
	timerStarted,                       //定时器已启动
	timerRunning,                       //定时器正在执行回调函数
	timerStopped,                       //定时器已停止
	timerDestroyed                      //定时器已销毁
}wTimerState;

typedef struct _wTimer                  //定义软件定时器类型
{
	wNode linkNode;                     //链表结点
	uint32_t startDelayTicks;           //初次启动后的ticks数
	uint32_t durationTicks;             //周期定时时的ticks数
	uint32_t delayTicks;                //当前定时递减计数值
	void (*timerFunc) (void * arg);     //定时回调函数
	void * arg;                         //传给回调函数的参数
	uint32_t config;                    //定时器配置参数
	wTimerState state;                  //定时器状态
}wTimer;

typedef struct _wTimerInfo             //定义软件定时器信息结构
{
    uint32_t startDelayTicks;          // 初次启动延后的ticks数
    uint32_t durationTicks;            // 周期定时时的周期tick数
    void (*timerFunc) (void * arg);    // 定时回调函数
    void * arg;                        // 传递给回调函数的参数
    uint32_t config;                   // 定时器配置参数
    wTimerState state;                 // 定时器状态
}wTimerInfo;

#define TIMER_CONFIG_TYPE_HARD        (1 << 0)    //硬件定时器（中断函数里）
#define TIMER_CONFIG_TYPE_SOFT        (0 << 0)    //软件定时器（任务函数里）

void wTimerInit(wTimer * timer, uint32_t delayTicks, uint32_t derationTicks, void(*timerFunc) (void * arg), void * arg, uint32_t config);
void wTimerStart (wTimer * timer);
void wTimerStop (wTimer * timer);
void wTimerDestroy (wTimer * timer);
void wTimerGetInfo (wTimer * timer, wTimerInfo * info);
void wTimerModuleTickNotify(void);
void wTimerModuleInit(void);

#endif
