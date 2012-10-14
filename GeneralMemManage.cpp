

#include "GeneralMemManage.h"

#include <iostream>

char* GeneralMemManage::GeneralMemCreate(int size)
{
	if(size<=0)
	{
		return NULL;
	}
	m_createAddress = new char[size];
	if(m_createAddress==NULL)
	{
		return NULL;
	}
	
	m_createSize = size;
	
	FreeMemNode *node = new FreeMemNode();
	node->address = m_createAddress;
	node->length = m_createSize;
	node->nextFreeMem = NULL;
	
	m_head = node;
	m_tail = node;
	
	return m_createAddress;
}

void GeneralMemManage::GeneralMemDestroy()
{
	//删除FreeMemNode链表
	DeleteFreeLinkList();
	
	if(m_createAddress!=NULL)
		delete []m_createAddress;
	m_createAddress = NULL;
}

char* GeneralMemManage::GeneralMemMalloc(int size)
{
	if(size<=0)
	{
		return NULL;
	}
	
	char *FreeMemoryAddress = FindFirstFitFreeMemory(size);
	//char *FreeMemoryAddress = FindBestFitFreeMemory(size);
	
	if(FreeMemoryAddress==NULL)
	{
		printf("GeneralMemMalloc no memory to malloc, return NULL\n");
		return NULL;
	}
	
	int *storeMallocSize = (int*)FreeMemoryAddress;
	*storeMallocSize = size;
	FreeMemoryAddress = FreeMemoryAddress + sizeof(int);
	
	return FreeMemoryAddress;
}

void GeneralMemManage::GeneralMemFree(char *address)
{
	if(address==NULL)
	{
		return;
	}

	char *realAddress = address - sizeof(int);
	int *size = (int*)realAddress;
	//memset(realAddress, 0, sizeof(int)+(*size));

	//把这段用户释放的内存插入FreeMemNode链表
	FreeMemNode *nodeCurr = m_head;
	FreeMemNode *nodePre = NULL;

	FreeMemNode *newNode = new FreeMemNode();
	if(newNode==NULL)
	{
		printf("GeneralMemFree alloc FreeMemNode failed\n");
		return;
	}
	newNode->address = realAddress;
	newNode->length = sizeof(int) + *size;
	newNode->nextFreeMem = NULL;

	while(nodeCurr!=NULL)
	{
		if(nodeCurr->address > realAddress)
		{
			//插到这一节点前面
			if(nodePre!=NULL)
			{
				printf("GeneralMemFree insert new free node to FreeMemNode list: insert in the general node\n");
				nodePre->nextFreeMem = newNode;
			}
			else
			{
				//nodePre==m_head
				//插入链表头
				printf("GeneralMemFree insert new free node to FreeMemNode list: insert in head\n");
				m_head = newNode;
			}
			newNode->nextFreeMem = nodeCurr;
			break;

		}
		else
		{
			nodePre = nodeCurr;
			nodeCurr = nodeCurr->nextFreeMem;
		}
	}
	if(nodeCurr==NULL)
	{
		//遍历完，都没有比realAddress大的地址，表示要插入到链表尾
		if(nodePre!=NULL)
		{
			printf("GeneralMemFree insert new free node to FreeMemNode list: insert in tail\n");
			nodePre->nextFreeMem = newNode;
		}
		else
		{
			//nodePre==m_head==NULL
			//这种情况，表示原来的链表没有节点
			printf("GeneralMemFree insert new free node to FreeMemNode list: nodePre==m_head==NULL, there is no node before insert \n");
			m_head = newNode;
		}
		newNode->nextFreeMem = nodeCurr;
	}

	MergeFreeMemory(nodePre, newNode, newNode->nextFreeMem);

}

void GeneralMemManage::DeleteFreeLinkList(FreeMemNode *head)
{
	if(head!=NULL)
	{
		FreeMemNode *pTemp = head;
		head = head->nextFreeMem;
		delete pTemp;
		DeleteFreeLinkList(pTemp);
	}
}


//不用递归而用循环对删除FreeMemNode链表的优化
void GeneralMemManage::DeleteFreeLinkList()
{
	if(m_head!=NULL)
	{
		while(m_head!=NULL)
		{
			FreeMemNode *pTemp = m_head;
			m_head = m_head->nextFreeMem;
			delete pTemp;
		}
		m_tail = NULL;
	}
}

//第一个int存放申请的内存大小，要判断够不够size+sizeof(int)的内存
//如果分割了内存，把not used那部分插入FreeMemNode链表
char* GeneralMemManage::FindFirstFitFreeMemory(int size)
{
	int needSize = size + sizeof(int);
	char *findAddress = NULL;
	FreeMemNode *nodeCurr = m_head;
	FreeMemNode *nodePre = NULL;
	
	while(nodeCurr!=NULL)
	{
		if(nodeCurr->length == needSize)
		{
			//find it
			findAddress = nodeCurr->address;
			if(nodePre!=NULL)
			{
				nodePre->nextFreeMem = nodeCurr->nextFreeMem;
			}
			else
			{
				//nodeCurr==m_head
				//这里可能导致m_head==NULL,表示没有内存可分了
				m_head = nodeCurr->nextFreeMem;
			}
			delete nodeCurr;
			
			return findAddress;
		}
		else if(nodeCurr->length > needSize)
		{
			//find it
			findAddress = nodeCurr->address;
			
			int cuttingMem = nodeCurr->length - needSize;
			
			FreeMemNode *newNode = new FreeMemNode();
			newNode->address = nodeCurr->address + needSize;
			newNode->length = cuttingMem;
			newNode->nextFreeMem = nodeCurr->nextFreeMem;
			
			if(nodePre!=NULL)
			{
				nodePre->nextFreeMem = newNode;
			}
			else
			{
				//nodeCurr==m_head
				m_head = newNode;
			}
			delete nodeCurr;
			
			return findAddress;
		}

		nodePre = nodeCurr;
		nodeCurr = nodeCurr->nextFreeMem;
		
	}
	
	return NULL;
}


//第一个int存放申请的内存大小，要判断够不够size+sizeof(int)的内存
//如果分割了内存，把not used那部分插入FreeMemNode链表
char* GeneralMemManage::FindBestFitFreeMemory(int size)
{
	int needSize = size + sizeof(int);
	char *findAddress = NULL;
	FreeMemNode *nodeCurr = m_head;
	FreeMemNode *nodePre = NULL;
	FreeMemNode *nodeBestFit = NULL;

	int findFitNodeAmount = 0;
	
	while(nodeCurr!=NULL)
	{
		if(nodeCurr->length == needSize)
		{
			//find the best best fit
			findAddress = nodeCurr->address;
			if(nodePre!=NULL)
			{
				nodePre->nextFreeMem = nodeCurr->nextFreeMem;
			}
			else
			{
				//nodeCurr==m_head
				//找到的best best fit节点是头结点
				//这里有可能导致m_head==NULL,表示没有内存可分了
				m_head = nodeCurr->nextFreeMem;
			}
			delete nodeCurr;
			
			return findAddress;
		}
		else if(nodeCurr->length > needSize)
		{
			//find a node
			findFitNodeAmount++;
			if(findFitNodeAmount==1)
			{
				nodeBestFit = nodeCurr;
			}
			else if(findFitNodeAmount > 1)
			{
				if(nodeBestFit->length > nodeCurr->length)
				{
					nodeBestFit = nodeCurr;
				}
			}
		}

		nodePre = nodeCurr;
		nodeCurr = nodeCurr->nextFreeMem;	
	}

	if(nodeBestFit==NULL)
	{
		printf("FindBestFitFreeMemory find zero fit, return NULL\n");
		return NULL;
	}
	else
	{
		    findAddress = nodeBestFit->address;
			
			int cuttingMem = nodeBestFit->length - needSize;
			
			FreeMemNode *newNode = new FreeMemNode();
			newNode->address = nodeBestFit->address + needSize;
			newNode->length = cuttingMem;
			newNode->nextFreeMem = nodeBestFit->nextFreeMem;
			
			if(nodePre!=NULL)
			{
				nodePre->nextFreeMem = newNode;
			}
			else
			{
				//nodeBestFit==m_head
				m_head = newNode;
			}
			delete nodeBestFit;
			
			return findAddress;
	}
}


//用户释放内存时，如果节点指向的内存连续，把它们合并
void GeneralMemManage::MergeFreeMemory(FreeMemNode *firstNode, FreeMemNode *secondNode, FreeMemNode *thirdNode)
{
	//firstNode==NULL表示secondNode为头部节点
	//thirdNode==NULL表示secondNode为尾部节点
	//firstNode和thirdNode都为NULL,表示链表只有一个节点不用合并,直接返回
	if(firstNode==NULL&&thirdNode==NULL)
	{
		printf("MergeFreeMemory firstNode==NULL&&thirdNode==NULL return \n");
		return;
	}
	else if(firstNode==NULL)
	{
		printf("MergeFreeMemory firstNode==NULL secondNode is head node. going to merge \n");
		if((secondNode->address+secondNode->length)==thirdNode->address)
		{
			MergeTwoFreeMemory(secondNode, thirdNode);
		}
	}
	else if(thirdNode==NULL)
	{
		printf("MergeFreeMemory thirdNode==NULL secondNode is tail node. going to merge \n");
		if((firstNode->address+firstNode->length)==secondNode->address)
		{
			MergeTwoFreeMemory(firstNode, secondNode);
		}
	}
	else
	{
		printf("MergeFreeMemory firstNode!=NULL&&thirdNode!=NULL , going to merge \n");
		if((firstNode->address+firstNode->length)==secondNode->address)
		{
			//第一和第二个节点指向的地址连续
			printf("MergeFreeMemory firstNode and SecondNode merge \n");
			firstNode = MergeTwoFreeMemory(firstNode,secondNode);
			if((firstNode->address+firstNode->length)==thirdNode->address)
			{
				//三个节点的指向的地址都连续
				printf("MergeFreeMemory three Node merge \n");
				firstNode = MergeTwoFreeMemory(firstNode,thirdNode);
			}
		}
		else if((secondNode->address+secondNode->length)==thirdNode->address)
		{
			//第二和第三个节点指向的地址连续
			printf("MergeFreeMemory secondNode and thirdNode merge \n");
			secondNode = MergeTwoFreeMemory(secondNode,thirdNode);
		}
	}
}

FreeMemNode* GeneralMemManage::MergeTwoFreeMemory(FreeMemNode *firstNode, FreeMemNode *secondNode)
{
	firstNode->length += secondNode->length;
	firstNode->nextFreeMem = secondNode->nextFreeMem;
	delete secondNode;

	return firstNode;
}