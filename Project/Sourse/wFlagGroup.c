#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ���¼���־�麯��
  * @param  flagGroup   �¼���־��ṹָ��
			flags       �¼���־
  * @retval ��
  ******************************************************************************************************************/	
void wFlagGroupInit(wFlagGroup * flagGroup, uint32_t flags)
{
	wEventInit(&flagGroup->event, wEventTypeFlagGroup);
	flagGroup->flag = flags;
}

/*******************************************************************************************************************
  * @brief  ��鲢���ĵ��¼���־����
  * @param  flagGroup   �¼���־��ṹָ��
			type        �¼���־�������
			flags       �¼���־
  * @retval wErrorNoError �¼�ƥ��
			wErrorResourceUnavaliable �¼�δƥ��
  ******************************************************************************************************************/	
static uint32_t wFlagGroupCheckAndConsume(wFlagGroup * flagGroup, uint32_t type, uint32_t * flags)
{
	uint32_t srcFlag = *flags;
	uint32_t isSet = type & WFLAGGROUP_SET;
	uint32_t isAll = type & WFLAGGROUP_ALL;
	uint32_t isConsume = type & WFLAGGROUP_CONSUME;
	
	uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);
	
	if(((isAll != 0) && (calFlag ==srcFlag)) || ((isAll == 0) && (calFlag != 0)))
	{
		if(isConsume)
		{
			if(isSet)
			{
				flagGroup->flag &= ~srcFlag;
			}
			else
			{
				flagGroup->flag |=srcFlag;
			}
		}
		*flags = calFlag;
		return wErrorNoError;
	}
	
	*flags = calFlag;
	return wErrorResourceUnavaliable;
}
	
/*******************************************************************************************************************
  * @brief  �ȴ��¼���־�麯��
  * @param  flagGroup   �¼���־��ṹָ��
			waitType    �ȴ��¼�������     
			requstFlag  ������¼���־
			resultFlag  �ȴ���־���
			waitTicks   �ȴ������ticks��
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
  ******************************************************************************************************************/	
uint32_t wFlagGroupWait(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = requestFlag;
	uint32_t status = wTaskEnterCritical();
	
	result = wFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	if(result != wErrorNoError)
	{
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		wEventWait(&flagGroup->event, currentTask, (void *)0, wEventTypeFlagGroup, waitTicks);
		
		wTaskExitCritical(status);
		
		wTaskSched();
		
		*resultFlag = currentTask->eventFlags;
		result = currentTask->waitEventResult;
	}
	else
	{
		*resultFlag = flags;
		wTaskExitCritical(status);
	}
	return result;
}

/*******************************************************************************************************************
  * @brief  ��ȡ�¼���־�麯��
  * @param  flagGroup   �¼���־��ṹָ��
			waitType    �ȴ��¼�������     
			requstFlag  ������¼���־
			resultFlag  �ȴ���־���
  * @retval �ȴ����    wErrorResourceUnavaliable    ��Դ������
                        wErrorNoError                û�д���
                        wErrorTimeout                �ȴ���ʱ
  ******************************************************************************************************************/	
uint32_t wFlagGroupNoWaitGet(wFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag)
{
	uint32_t flags = requestFlag;
	
	uint32_t status = wTaskEnterCritical();
	uint32_t result = wFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	wTaskExitCritical(status);
	
	*resultFlag = flags;
	return wErrorNoError;
}

/*******************************************************************************************************************
  * @brief  �¼���־�黽��������
  * @param  flagGroup   �¼���־��ṹָ��
			isSet       �Ƿ��������¼���־
			flags       �������¼���־
  * @retval ��
  ******************************************************************************************************************/	
void wFlagGroupNotify(wFlagGroup * flagGroup, uint8_t isSet, uint32_t flag)
{
	wList * waitList;
	wNode * node;
	wNode * nextNode;
	uint8_t sched = 0;
	uint32_t status = wTaskEnterCritical();
	
	if(isSet)
	{
		flagGroup->flag |= flag;
	}
	else
	{
		flagGroup->flag &= ~flag;
	}
	
	waitList = &flagGroup->event.waitList;
	for(node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode)
	{
		uint32_t result;
		wTask * task = wNodeParent(node, wTask, linkNode);
		uint32_t flags = task->eventFlags;
		nextNode = node->nextNode;
		
		result = wFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
		if(result == wErrorNoError)
		{
			task->eventFlags =flags;
			wEventWakeUpTask(&flagGroup->event, task, (void *)0, wErrorNoError);
			sched = 1;
		}
	}
	
	if(sched)
	{
		wTaskSched();
	}
	wTaskExitCritical(status);
}
