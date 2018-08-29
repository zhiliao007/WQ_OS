/*
 * @file wHooks.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   钩子
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */

#include "WQ_OS.h"

/* 请添加您的代码 */

/*!
 * @brief  CPU空闲时的hooks函数
 * @param  无
 * @retval 无
 */	
void wHooksCpuIdle (void)
{

}

/*!
 * @brief  时钟节拍hooks函数
 * @param  无
 * @retval 无
 */	
void wHooksSysTick (void)
{

}

/*!
 * @brief  任务切换hooks函数
 * @param  from 从哪个任务开始切换
 * @param  to   切换至哪个任务
 * @retval 无
 */	
void wHooksTaskSwitch (wTask * from, wTask * to)
{

}

/*!
 * @brief  任务初始化的hooks函数
 * @param  task 等待初始化的任务
 * @retval 无
 */	
void wHooksTaskInit (wTask * task)
{

}
