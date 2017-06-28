#define NVIC_INT_CTRL          0xE000ED04
#define NVIC_PENDSVSET         0x10000000
#define NVIC_SYSPRI2           0xE000ED22
#define NVIC_PENDSV_PRT        0x000000FF

#define MEM32(addr)            *(volatile unsigned long *)(addr)
#define MEM8(addr)             *(volatile unsigned char *)(addr)

void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

typedef struct _BlockType_t
{
    unsigned long * stackPtr;
}BlockType_t;

BlockType_t * blockPtr;


void delay(int count)
{
    while(--count>0);
}

int flag;

BlockType_t block;
unsigned long stackBuffer[1024];
	
int main()
{
	block.stackPtr = &stackBuffer[1024];
	blockPtr = &block;
    for(;;)
	{
		flag = 0;
		delay(1000);
		flag = 1;
		delay(1000);
		
		triggerPendSVC();
	}
}
