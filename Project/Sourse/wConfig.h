#ifndef WCONFIG_H
#define WCONFIG_H

#define WQ_OS_PRO_COUNT            32            //WQ_OS任务的优先级序号
#define WQ_OS_SLICE_MAX            10            //时间片时间10*10 ms
#define WQ_OS_IDLETASK_STACK_SIZE  1024          //空闲任务的堆栈大小
#define WQ_OS_TIMERTASK_STACK_SIZE 1024          //定时器任务的堆栈大小
#define WQ_OS_TIMERTASK_PRIO       1             //定时器任务的优先级

#define WQ_OS_SYSTICK_MS           10            //时钟节拍的周期，以ms为单位

// 内核功能裁剪部分
#define WQ_OS_ENABLE_SEM               1                       // 是否使能信号量
#define WQ_OS_ENABLE_MUTEX             1                       // 是否使能互斥信号量
#define WQ_OS_ENABLE_FLAGGROUP         1                       // 是否使能事件标志组
#define WQ_OS_ENABLE_MBOX              1                       // 是否使能邮箱
#define WQ_OS_ENABLE_MEMBLOCK          1                       // 是否使能存储块
#define WQ_OS_ENABLE_TIMER             1                       // 是否使能定时器
#define WQ_OS_ENABLE_CPUUSAGE_STAT     1                       // 是否使能CPU使用率统计

#endif
