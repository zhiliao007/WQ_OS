#include "wSem.h"
#include "WQ_OS.h"
/*******************************************************************************************************************
  * @brief  ��ʼ���ź�������
  * @param  sem         �ź����ṹָ��
			startCount  ��ʼ����ֵ
			maxCount    ������ֵ����Ϊ0�������� 
  * @retval ��
  ******************************************************************************************************************/	
void wSemInit (wSem * sem, uint32_t startCount, uint32_t maxCount) 
{
	wEventInit(&sem->event, wEventTypeSem);

	sem->maxCount = maxCount;
	if (maxCount == 0)
	{
		sem->count = startCount;
	} 
	else 
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;
	}
}

/*******************************************************************************************************************
  * @brief  �ȴ��ź�������
  * @param  sem       �ź����ṹָ��
			waitTicks �ȴ������ticks��
                      //���ź�������Ϊ0ʱ���ȴ���ticks����Ϊ0ʱ��ʾ��Զ�ȴ�
* @retval �ȴ����  wErrorNoError 
	                wErrorTimeout             
	                wErrorResourceUnavaliable 
  ******************************************************************************************************************/
uint32_t wSemWait(wSem * sem, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	if(sem->count > 0)
	{
		--sem->count;
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else
	{
		wEventWait(&sem->event, currentTask, (void *)0,  wEventTypeSem, waitTicks);                                                                                                                                                
        wTaskExitCritical(status);
		wTaskSched();
        return currentTask->waitEventResult;   // �����ڵȴ���ʱ���߼�������ʱ��ִ�л᷵�ص����Ȼ��ȡ���ȴ��ṹ
	}
}

/*******************************************************************************************************************
  * @brief  ��ȡ�ź�������
  * @param  sem       �ź����ṹָ��
  * @retval �ȴ����  wErrorNoError 
	                  wErrorTimeout             
	                  wErrorResourceUnavaliable 
  ******************************************************************************************************************/
uint32_t wSemNoWaitGet (wSem * sem)
{
    uint32_t status = wTaskEnterCritical();
 
	if (sem->count > 0) 
    {
		--sem->count; 
        wTaskExitCritical(status);
    	return wErrorNoError;
    } 
	else 
	{
        wTaskExitCritical(status);
    	return wErrorResourceUnavaliable;
    }    
}
	
/*******************************************************************************************************************
  * @brief  �ź�������������
  * @param  sem       �ź����ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wSemNotify (wSem * sem)
{
    uint32_t status = wTaskEnterCritical();        
    
    if (wEventWaitCount(&sem->event) > 0)  //����Ƿ��������ڵȴ�
    {
    
        wTask * task = wEventWakeUp(&sem->event, (void *)0, wErrorNoError );  //�˴����ѵ��Ƕ����ײ�������

        if (task->prio < currentTask->prio)
        {
            wTaskSched(); 
    	}
    }
    else
    {
    	++sem->count;    //������Դ���¼���+1

    	if ((sem->maxCount != 0) && (sem->count > sem->maxCount)) 
    	{	
    		sem->count = sem->maxCount;
    	}
    }
    wTaskExitCritical(status);
}

/*******************************************************************************************************************
  * @brief  �ź���״̬��ѯ����
  * @param  sem    �ź����ṹָ��
			info   ״̬��ѯ�ṹָ��
  * @retval ��
  ******************************************************************************************************************/
void wSemGetInfo (wSem * sem, wSemInfo * info)
{
    uint32_t status = wTaskEnterCritical();        
    
    info->count = sem->count;
    info->maxCount = sem->maxCount;
    info->taskCount = wEventWaitCount(&sem->event);
    
    wTaskExitCritical(status);
}

	/*******************************************************************************************************************
  * @brief  �����ź�������
  * @param  sem       �ź����ṹָ��
  * @retval �����ٸ��ź��������ѵ���������
  ******************************************************************************************************************/
uint32_t wSemDestroy (wSem * sem)
{       
    uint32_t status = wTaskEnterCritical(); 

    uint32_t count = wEventRemoveAll(&sem->event, (void *)0, wErrorDel);  
    sem->count = 0;
    wTaskExitCritical(status);
    
    if (count > 0) 
    {
        wTaskSched();
    } 
    return count;  
}
