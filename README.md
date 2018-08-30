# WQ_OS——用于CM3内核的RTOS
#### 请用Keil5.23打开工程

#### [文档注释](http://htmlpreview.github.io/?https://github.com/zhiliao007/WQ_OS/blob/master/doc/html/index.html)

#### 包含内容如下：

1.   任务切换（通过PendSVC异常）
2.   临界区保护
3.   调度锁
4.   多优先级（通过位图数据结构）
5.   任务队列（通过双向链表）
6.   同优先级任务支持时间片轮转
7.   任务的挂起与唤醒
8.   任务的删除
9.   任务状态查询
10.   事件控制块
11.   计数信号量
12.   邮箱