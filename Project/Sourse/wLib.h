#ifndef WLIB_H
#define WLIB_H

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}wBitmap;

void wBitmapInit(wBitmap * bitmap);
uint32_t wBitmapPosCount(void);
void wBitmapSet(wBitmap * bitmap, uint32_t pos);
void wBitmapClear(wBitmap * bitmap, uint32_t pos);
uint32_t wBitmapGetFirstSet(wBitmap * bitmap);

typedef struct _wNode                 //结点结构体
{
	struct _wNode * preNode;
	struct _wNode * nextNode;
}wNode;

void wNodeInit(wNode * node);

typedef struct _wList                 //链表结构体
{
	wNode headNode;
	uint32_t nodeCount;
}wList;

#define wNodeParent(node,parent,name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void wListInit(wList * list); 	
uint32_t wListCount(wList * list);
wNode * wListFirst(wList * list);
wNode * wListLast(wList * list);
wNode * wListPre (wList * list, wNode * node);
wNode * wListNext (wList * list, wNode * node);	
void wListRemoveAll(wList * list);
void wListAddFirst (wList * list, wNode * node);
void wListAddLast (wList * list, wNode * node);
wNode * wListRemoveFirst (wList * list);
void wListInsertAfter (wList * list, wNode * nodeAfter, wNode * nodeToInsert);
void wListRemove (wList * list, wNode * node);

#endif
