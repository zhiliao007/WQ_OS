#include "WQ_OS.h"

/*******************************************************************************************************************
  * @brief  ��ʼ�������ź�������
  * @param  mutex   �����ź����ṹָ��
  * @retval ��
  ******************************************************************************************************************/	
void wMutexInit(wMutex * mutex)
{
	wEventInit(&mutex->event, wEventTypeMutex);
	
	mutex->lockedCount = 0;
	mutex->owner = (wTask *)0;
	mutex->ownerOriginalPrio = WQ_OS_PRO_COUNT;  //����Ϊ��Чֵ����Ϊ���ȼ�Ϊ0~31
}
