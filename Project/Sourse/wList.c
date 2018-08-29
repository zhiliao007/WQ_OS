/*
 * @file wList.c
 * @author 李文晴
 * @version 1.0.0.0
 * @brief   链表
 * 
 * 更新历史
 * --
 * 版本号|说明|修订者|修订日期
 * ------|----|------|--------
 * v1.0.0.0|创建文档|李文晴|2017-7
 * 
 */
 
#include "wLib.h"

/*******************************************************************************
 * 宏定义
 ******************************************************************************/
/*! 首尾结点 */
#define firstNode headNode.nextNode
#define lastNode headNode.preNode

/*!
 * @brief  结点初始化函数
 * @param  node   结点结构指针
 * @retval 无
 */	
void wNodeInit(wNode * node)
{
	node->nextNode = node;
	node->preNode = node;
}

/*!
 * @brief  链表初始化函数
 * @param  list   链表结构指针
 * @retval 无
 */	
void wListInit(wList * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/*!
 * @brief  链表结点数计数函数
 * @param  list   链表结构指针
 * @retval 链表结点数
 */	
uint32_t wListCount(wList * list)
{
	return list->nodeCount;
}

/*!
 * @brief  返回首个结点函数
 * @param  list  链表结构指针
 * @retval 第一个结点 若无结点，则返回0
 */	
wNode * wListFirst(wList * list)
{
	wNode * node = (wNode *)0;
	
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

/*!
 * @brief  返回最后一个结点函数
 * @param  list  链表结构指针
 * @retval 最后一个结点 若无结点，则返回0
 */	
wNode * wListLast(wList * list)
{
	wNode * node = (wNode *)0;
	
	if(list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

/*!
 * @brief  返回链表中指定结点的前一结点
 * @param  list 链表结构指针
 * @param  node 指定结点指针
 * @retval 链表中指定结点的前一结点名
 */	
wNode * wListPre (wList * list, wNode * node)
{
	if (node->preNode == node) 
	{
		return (wNode *)0;
	} 
	else 
	{
		return node->preNode;
	}
}

/*!
 * @brief  返回链表中指定结点的后一结点
 * @param  list 链表结构指针
 * @param  node 指定结点指针
 * @retval 链表中指定结点的后一结点名
 */	
wNode * wListNext (wList * list, wNode * node)
{
	if (node->nextNode == node) 
	{
		return (wNode *)0;
	} 
	else 
	{
		return node->nextNode;
	}
}

/*!
 * @brief  清空链表函数
 * @param  list   链表结构指针
 * @retval 无
 */	
void wListRemoveAll(wList * list)
{
	uint32_t count;
	wNode * nextNode;
	
	/* 保存链表的第一个结点 */
	nextNode = list->firstNode;
	/* 遍历所有结点 */
	for(count = list->nodeCount; count != 0; count --) 
	{
		/* 保存结点到当前结点 */
		wNode * currentNode = nextNode; 
		/* 指向下一个结点 */
		nextNode = nextNode->nextNode;             
		/* 删除当前结点 */
		currentNode->nextNode = currentNode;       
		currentNode->preNode = currentNode;
	}
	
	/* 删除第一个结点 */
	list->firstNode = &(list->headNode);           
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/*!
 * @brief  添加指定结点到链表头部函数
 * @param  list 链表结构指针
 * @param  node 指定结点指针
 * @retval 无
 */	
void wListAddFirst (wList * list, wNode * node)
{
    node->preNode = list->firstNode->preNode;
    node->nextNode = list->firstNode;

    list->firstNode->preNode = node;
    list->firstNode = node;
    list->nodeCount++;
}

/*!
 * @brief  添加指定结点到链表尾部函数
 * @param  list 链表结构指针
 * @param  node 指定结点指针
 * @retval 无
 */	
void wListAddLast (wList * list, wNode * node)
{
	node->nextNode = &(list->headNode);
    node->preNode = list->lastNode;

    list->lastNode->nextNode = node;
    list->lastNode = node;
    list->nodeCount++;
}

/*!
 * @brief  移除链表中的第1个结点函数
 * @param  list   链表结构指针
 * @retval 第1个结点
 */	
wNode * wListRemoveFirst (wList * list)
{
    wNode * node = (wNode *)0;

	if( list->nodeCount != 0 )
    {
        node = list->firstNode;

        node->nextNode->preNode = &(list->headNode);
        list->firstNode = node->nextNode;
        list->nodeCount--;
    }
    return  node;
}

/*!
 * @brief  插入结点到某个结点后面函数
 * @param  list          链表结构指针
 * @param	nodeAfter 		参考结点指针
 * @param	nodeToInsert 	待插入结点指针
 * @retval 无
 */	
void wListInsertAfter (wList * list, wNode * nodeAfter, wNode * nodeToInsert)
{
    nodeToInsert->preNode = nodeAfter;
    nodeToInsert->nextNode = nodeAfter->nextNode;

    nodeAfter->nextNode->preNode = nodeToInsert;
    nodeAfter->nextNode = nodeToInsert;

    list->nodeCount++;
}

/*!
 * @brief  移除链表中指定结点函数
 * @param  list    链表结构指针
 * @param  node    移除结点指针
 * @retval 无
 */	
void wListRemove (wList * list, wNode * node)
{
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount--;
}
