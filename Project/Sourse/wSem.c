#include "wSem.h"

/*******************************************************************************************************************
  * @brief  ��ʼ���ź�������
  * @param  sem         �ź������ָ��
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
