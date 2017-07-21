#ifndef WTASK_H
#define WTASK_H

#define WQOS_TASK_STATE_RDY              0
#define WQOS_TASK_STATE_DESTORYED        (1 << 1)
#define WQOS_TASK_STATE_DELAYED          (1 << 2)
#define WQOS_TASK_STATE_SUSPEND          (1 << 3)

#define WQOS_TASK_WAIT_MASK            (0xFF << 16)

struct _wEvent;

typedef uint32_t wTaskStack;	//���������ջ����

typedef struct _wTask        //����ṹ
{								
	wTaskStack * stack;		 //�����ջָ��
	wNode linkNode;          //���ȼ��������ӽ��
	uint32_t delayTicks;	 //������ʱ���� 
	wNode delayNode;         //ͨ����ʱ���ṹ
	uint32_t prio;           //�������ȼ�
	uint32_t state;          //״̬�ֶΣ������ж��ǲ��Ǵ�����ʱ״̬
	uint32_t slice;          //ʱ��Ƭ������
	uint32_t suspendCount;   //����״̬������
	
	void (*clean) (void * param);   //����ɾ��ʱ���õ�������
	void * cleanparam;                //���ݸ��������Ĳ���
	uint8_t requestDeleteFlag;       //����ɾ����־
	
	struct _wEvent * waitEvent;        //���ڵȴ���������ƿ�
	void * eventMsg;           //�ȴ��¼������ݴ��λ��
	uint32_t waitEventResult;  //�ȴ��¼��Ľ�� 
	
}wTask;

typedef struct _wTaskInfo      //���������Ϣ�ṹ
{
	uint32_t delayTicks;       //��ʱ������
	uint32_t prio;             //�������ȼ�
	uint32_t state;            //����ǰ״̬
	uint32_t slice;            //��ǰʣ���ʱ��Ƭ
	uint32_t suspendCount;     //������Ĵ���
}wTaskInfo;

void wTaskInit(wTask * task, void (*entry)(void *), void * param,uint32_t prio, wTaskStack * stack);
void wTaskSuspend(wTask * task);
void wTaskWakeUp(wTask * task);
void wTaskSetCleanCallFunc(wTask * task, void(*clean)(void * param), void * param);
void wTaskForceDelete(wTask * task); 
void wTaskRequestDelete(wTask * task);
uint8_t wTaskIsRequestedDelete(void);
void wTaskDeleteSelf(void);
void wTaskGetInfo(wTask * task, wTaskInfo * info);

#endif
