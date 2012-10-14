
#include "GeneralMemManage.h"

int main()
{
	GeneralMemManage memory;
	memory.GeneralMemCreate(1024);
	char *pMalloc1 = memory.GeneralMemMalloc(8);
	char *pMalloc2 = memory.GeneralMemMalloc(17);
	char *pMalloc3 = memory.GeneralMemMalloc(24);
	char *pMalloc4 = memory.GeneralMemMalloc(14);
	char *pMalloc5 = memory.GeneralMemMalloc(9);
	char *pMalloc6 = memory.GeneralMemMalloc(77);
	char *pMalloc7 = memory.GeneralMemMalloc(333);
	memory.GeneralMemFree(pMalloc3);
	memory.GeneralMemFree(pMalloc1);
	memory.GeneralMemFree(pMalloc7);
	memory.GeneralMemFree(pMalloc5);
	//memory.GeneralMemFree(pMalloc4);
	//memory.GeneralMemFree(pMalloc2);
	//memory.GeneralMemFree(pMalloc6);
	memory.GeneralMemDestroy();
	return 0;
}