#ifndef WCONFIG_H
#define WCONFIG_H

#define WQ_OS_PRO_COUNT            32            //WQ_OS任务的优先级序号
#define WQ_OS_SLICE_MAX            10            //时间片时间10*10 ms
#define WQ_OS_IDLETASK_STACK_SIZE  1024          //空闲任务的堆栈大小
#define WQ_OS_TIMERTASK_STACK_SIZE 1024          //定时器任务的堆栈大小
#define WQ_OS_TIMERTASK_PRIO       1             //定时器任务的优先级

#endif
