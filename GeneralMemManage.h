

#ifndef __GENERAL_MEM_MANAGE_H__
#define __GENERAL_MEM_MANAGE_H__

#include <iostream>


typedef struct _node{
	char *address;
	int length;
	_node *nextFreeMem;
}FreeMemNode;

class GeneralMemManage
{
	
public:
	GeneralMemManage():
	  m_createAddress(NULL), m_createSize(0), m_head(NULL), m_tail(NULL)
	  {}

	char* GeneralMemCreate(int size);
	char* GeneralMemMalloc(int size);
	void GeneralMemFree(char *address);
	void GeneralMemDestroy();


private:

	void DeleteFreeLinkList(FreeMemNode *head);
	void DeleteFreeLinkList();
	
	//�����ڴ���䷽ʽfirst fit��best fit
	char* FindFirstFitFreeMemory(int size);
	char* FindBestFitFreeMemory(int size);

	//�û��ͷ��ڴ�ʱ������ڵ�ָ����ڴ������������Ǻϲ�
	void MergeFreeMemory(FreeMemNode *firstNode, FreeMemNode *secondNode, FreeMemNode *thirdNode);
	//����MergeFreeMemory
	FreeMemNode* MergeTwoFreeMemory(FreeMemNode *firstNode, FreeMemNode *secondNode);

	char *m_createAddress;
	int m_createSize;
	FreeMemNode *m_head;
	FreeMemNode *m_tail;

};

#endif //__GENERAL_MEM_MANAGE_H__