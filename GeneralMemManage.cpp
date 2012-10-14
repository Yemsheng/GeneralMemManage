

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
	//ɾ��FreeMemNode����
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

	//������û��ͷŵ��ڴ����FreeMemNode����
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
			//�嵽��һ�ڵ�ǰ��
			if(nodePre!=NULL)
			{
				printf("GeneralMemFree insert new free node to FreeMemNode list: insert in the general node\n");
				nodePre->nextFreeMem = newNode;
			}
			else
			{
				//nodePre==m_head
				//��������ͷ
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
		//�����꣬��û�б�realAddress��ĵ�ַ����ʾҪ���뵽����β
		if(nodePre!=NULL)
		{
			printf("GeneralMemFree insert new free node to FreeMemNode list: insert in tail\n");
			nodePre->nextFreeMem = newNode;
		}
		else
		{
			//nodePre==m_head==NULL
			//�����������ʾԭ��������û�нڵ�
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


//���õݹ����ѭ����ɾ��FreeMemNode������Ż�
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

//��һ��int���������ڴ��С��Ҫ�жϹ�����size+sizeof(int)���ڴ�
//����ָ����ڴ棬��not used�ǲ��ֲ���FreeMemNode����
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
				//������ܵ���m_head==NULL,��ʾû���ڴ�ɷ���
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


//��һ��int���������ڴ��С��Ҫ�жϹ�����size+sizeof(int)���ڴ�
//����ָ����ڴ棬��not used�ǲ��ֲ���FreeMemNode����
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
				//�ҵ���best best fit�ڵ���ͷ���
				//�����п��ܵ���m_head==NULL,��ʾû���ڴ�ɷ���
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


//�û��ͷ��ڴ�ʱ������ڵ�ָ����ڴ������������Ǻϲ�
void GeneralMemManage::MergeFreeMemory(FreeMemNode *firstNode, FreeMemNode *secondNode, FreeMemNode *thirdNode)
{
	//firstNode==NULL��ʾsecondNodeΪͷ���ڵ�
	//thirdNode==NULL��ʾsecondNodeΪβ���ڵ�
	//firstNode��thirdNode��ΪNULL,��ʾ����ֻ��һ���ڵ㲻�úϲ�,ֱ�ӷ���
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
			//��һ�͵ڶ����ڵ�ָ��ĵ�ַ����
			printf("MergeFreeMemory firstNode and SecondNode merge \n");
			firstNode = MergeTwoFreeMemory(firstNode,secondNode);
			if((firstNode->address+firstNode->length)==thirdNode->address)
			{
				//�����ڵ��ָ��ĵ�ַ������
				printf("MergeFreeMemory three Node merge \n");
				firstNode = MergeTwoFreeMemory(firstNode,thirdNode);
			}
		}
		else if((secondNode->address+secondNode->length)==thirdNode->address)
		{
			//�ڶ��͵������ڵ�ָ��ĵ�ַ����
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