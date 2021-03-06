/*
 * @file wBitmap.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   位图
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 
#include "wLib.h"

/*!
 * @brief  位图结构初始化函数
 * @param  bitmap   位图结构指针
 * @retval 无
 */	
void wBitmapInit(wBitmap * bitmap)
{
	bitmap->bitmap = 0;
}

/*!
 * @brief  位图长度函数
 * @param  无
 * @retval 位图长度
 */
uint32_t wBitmapPosCount(void)
{
	return 32;
}

/*!
 * @brief  位图结构置位函数 
 * @param  pos     需要置位的位
 * @param  bitmap  位图结构指针
 * @retval 无
 */	uint32_t wBitmapPosCount(void);
void wBitmapSet(wBitmap * bitmap, uint32_t pos)
{ 
	bitmap->bitmap |= 1 << pos;
}

/*!
 * @brief  位图结构清零函数 
 * @param  pos     需要清零的位
 * @param  bitmap  位图结构指针
 * @retval 无
 */	
void wBitmapClear(wBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(1 << pos);
}

/*!
 * @brief  位图查找函数       
 * @param  无
 * @retval 第一个被置位的位序号
 */	
uint32_t wBitmapGetFirstSet(wBitmap * bitmap)
{
	static const uint8_t quickFindTable[] =     
	{
	    /* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if (bitmap->bitmap & 0xff)
    {
        return quickFindTable[bitmap->bitmap & 0xff];         
    }
    else if (bitmap->bitmap & 0xff00)
    {
        return quickFindTable[(bitmap->bitmap >> 8) & 0xff] + 8;        
    }
    else if (bitmap->bitmap & 0xff0000)
    {
        return quickFindTable[(bitmap->bitmap >> 16) & 0xff] + 16;        
    }
    else if (bitmap->bitmap & 0xFF000000)
    {
        return quickFindTable[(bitmap->bitmap >> 24) & 0xFF] + 24;
    }
    else
    {
        return wBitmapPosCount();
    }
}
