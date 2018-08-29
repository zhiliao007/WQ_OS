#ifndef WTASK_H
#define WTASK_H

#define WQOS_TASK_STATE_RDY              0
#define WQOS_TASK_STATE_DESTORYED        (1 << 1)
#define WQOS_TASK_STATE_DELAYED          (1 << 2)
#define WQOS_TASK_STATE_SUSPEND          (1 << 3)

#define WQOS_TASK_WAIT_MASK            (0xFF << 16)

struct _wEvent;
/* 定义任务堆栈类型 */
typedef uint32_t wTaskStack;	

/*! 
 * @brief 任务结构
 */
typedef struct _wTask        
{								
	wTaskStack * stack;		 /*!< 任务堆栈指针 */
	uint32_t * stackBase;    /*!< 任务堆栈首地址 */
	uint32_t stackSize;      /*!< 堆栈大小 */ 
	wNode linkNode;          /*!< 优先级队列链接结点 */
	uint32_t delayTicks;	 /*!< 任务延时个数 */
	wNode delayNode;         /*!< 通用延时结点结构 */
	uint32_t prio;           /*!< 任务优先级 */ 
	uint32_t state;          /*!< 状态字段，用于判断是不是处于延时状态 */
	uint32_t slice;          /*!< 时间片计数器 */
	uint32_t suspendCount;   /*!< 挂起状态计数器 */
	
	void (*clean) (void * param);     /*!< 任务被删除时调用的清理函数 */
	void * cleanparam;                /*!< 传递给清理函数的参数 */
	uint8_t requestDeleteFlag;        /*!< 请求删除标志 */
	
	struct _wEvent * waitEvent;      /*!< 正在等待的任务控制块 */
	void * eventMsg;           /*!< 等待事件的数据存放位置 */
	uint32_t waitEventResult;  /*!< 等待事件的结果 */
	
	uint32_t waitFlagsType;    /*!< 等待的事件方式 */
	uint32_t eventFlags;       /*!< 等待的事件标志 */
}wTask;

/*! 
 * @brief 任务相关信息结构
 */
typedef struct _wTaskInfo      
{
	uint32_t delayTicks;       /*!< 延时计数器 */
	uint32_t prio;             /*!< 任务优先级 */
	uint32_t state;            /*!< 任务当前状态 */
	uint32_t slice;            /*!< 当前剩余的时间片 */
	uint32_t suspendCount;     /*!< 被挂起的次数 */
	uint32_t stackSize;        /*!< 堆栈大小 */
	uint32_t stackFree;        /*!< 空闲堆栈大小 */
}wTaskInfo;

void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack, uint32_t size);
void wTaskSuspend(wTask * task);
void wTaskWakeUp(wTask * task);
void wTaskSetCleanCallFunc(wTask * task, void(*clean)(void * param), void * param);
void wTaskForceDelete(wTask * task); 
void wTaskRequestDelete(wTask * task);
uint8_t wTaskIsRequestedDelete(void);
void wTaskDeleteSelf(void);
void wTaskGetInfo(wTask * task, wTaskInfo * info);

#endif
