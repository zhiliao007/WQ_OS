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
