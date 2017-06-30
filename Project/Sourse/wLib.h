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

#endif
