#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ�����亯��
  * @param  mbox        ����ṹָ��
			msgBuffer   ��Ϣ�洢������
			maxCount    �����Ϣ����ֵ 
  * @retval ��
  ******************************************************************************************************************/	
void wMboxInit(wMbox * mbox, void **msgBuffer, uint32_t maxCount)
{
	wEventInit(&mbox->event, wEventTypeMbox);
	
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}

/*******************************************************************************************************************
  * @brief  �ȴ������ȡ��Ϣ����
  * @param  mbox        ����ṹָ��
			msg         ��Ϣ�洢������
			waitTicks   �ȴ������ticks�� 
  * @retval �ȴ����    wErrorResourceUnavaliable    
                        wErrorNoError               
                        wErrorTimeout               
  ******************************************************************************************************************/	
uint32_t wMboxWait(wMbox * mbox, void **msg, uint32_t waitTicks)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		wTaskExitCritical(status);
		return wErrorNoError;
	}
	else 
	{
		wEventWait(&mbox->event, currentTask, (void *)0, wEventTypeMbox, waitTicks);
		wTaskExitCritical(status);
		
		wTaskSched();
		
		*msg = currentTask->eventMsg;
		return currentTask ->waitEventResult;
	}
	
}

/*******************************************************************************************************************
  * @brief  �޵ȴ��Ļ�ȡ������Ϣ����
  * @param  mbox        ����ṹָ��
			msg         ��Ϣ�洢������
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
  ******************************************************************************************************************/	
uint32_t wMboxNoWaitGet(wMbox * mbox,void **msg)
{
	uint32_t status = wTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
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
  * @brief  ���份��������
  * @param  mbox          ����ṹָ��
			msg           ��Ϣ�洢������
			notifyOption  ���͵�ѡ��
  * @retval �ȴ����      wErrorResourceUnavaliable    ��Դ������
                          wErrorNoError                û�д���
                          wErrorTimeout                �ȴ���ʱ
  ******************************************************************************************************************/	
uint32_t wMboxNotify(wMbox * mbox, void *msg, uint32_t notifyOption)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wEventWaitCount(&mbox->event) > 0)
	{
		wTask *task = wEventWakeUp(&mbox->event, (void *)msg, wErrorNoError);
		if(task->prio < currentTask->prio)
		{
			wTaskSched();
		}
	}
	else 
	{
		if(mbox->count >= mbox->maxCount)
		{
			wTaskExitCritical(status);
			return wErrorResourseFull;
		}
		
		if(notifyOption & wMBOXSendFront)
		{
			if(mbox->read <= 0)
			{
				mbox->read = mbox->maxCount - 1;
			}
			else
			{
				--mbox->read;
			}
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		mbox->count++;
	}
	
	wTaskExitCritical(status);
	return wErrorNoError;
}

/*******************************************************************************************************************
  * @brief  ������亯��
  * @param  mbox  ����ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
void wMboxFlush(wMbox * mbox)
{
	uint32_t status = wTaskEnterCritical();
	
	if(wEventWaitCount(&mbox->event) == 0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
	wTaskExitCritical(status);
}
	
/*******************************************************************************************************************
  * @brief  ɾ�����亯��
  * @param  mbox        ����ṹָ��
  * @retval ��������������
  ******************************************************************************************************************/	
uint32_t wMboxDestory(wMbox * mbox)
{
	uint32_t status = wTaskEnterCritical();
	uint32_t count = wEventRemoveAll(&mbox->event,(void *)0, wErrorDel);
	wTaskExitCritical(status);
	
	if(count > 0)
	{
		wTaskSched();
	}
	return count;
}

/*******************************************************************************************************************
  * @brief  ����״̬��ѯ����
  * @param  mbox   ����ṹָ��
			info   ״̬��ѯ�ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
void wMboxGetInfo(wMbox * mbox, wMboxInfo * info)
{
	uint32_t status = wTaskEnterCritical();        
    
    info->count = mbox->count;
    info->maxCount = mbox->maxCount;
    info->taskCount = wEventWaitCount(&mbox->event);
    
    wTaskExitCritical(status);
}
