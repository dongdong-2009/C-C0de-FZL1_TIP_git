///////////////////////////////////////////////////////////
//  ISystemListImp.cpp
//  Implementation of the Class ISystemListImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////
#include <iostream>

#include "ISystemListImp.h"
#include "ISystemListNodeImp.h"
#include "ISystemLIstNodeDataImp.h"

ISystemListImp::ISystemListImp()
{
	this->m_nNodesCount = 0;
	this->m_pHead = NULL;
	this->m_pTail = NULL;

}



ISystemListImp::~ISystemListImp()
{

	this->freeSystemList();
	this->m_nNodesCount = 0;
	this->m_pHead = NULL;
	this->m_pTail = NULL;


}





void ISystemListImp::addSystemListNode(ISystemListNode* pSystemNode)
{
	if (NULL != pSystemNode)
	{		
		pSystemNode->setNextNode(NULL);

		if (NULL != this->m_pTail)
		{
			this->m_pTail->setNextNode(pSystemNode); 
			pSystemNode->setPrevNode(this->m_pTail);
		}
		if (NULL == this->m_pHead)
		{
			this->m_pHead = pSystemNode;
			pSystemNode->setPrevNode(NULL);
		}
		this->m_pTail = pSystemNode;
		this->m_nNodesCount++;
	}

	return ;
}


void ISystemListImp::addSystemListNodeWithData(enumDataType nDataType, void* pData)
{
	ISystemListNode *pNewNode = new ISystemListNodeImp();
	ISystemListNodeData *pNewNodeData = new ISystemLIstNodeDataImp();
	pNewNodeData->setDataType(nDataType);
	pNewNodeData->setData(pData);
	pNewNode->setNodeData(pNewNodeData);

	this->addSystemListNode(pNewNode);

	pNewNode = NULL;
	pNewNodeData = NULL;

}


void ISystemListImp::freeSystemList()
{
	ISystemListNode *pNode = NULL;
	ISystemListNode *pNextNode = NULL;

	pNode = this->m_pHead;
	while (pNode != NULL)
	{
		pNextNode = pNode->getNextNode();

		delete pNode;
		pNode = NULL;

		pNode = pNextNode;
	}
	this->m_nNodesCount = 0;
	this->m_pHead = NULL;
	this->m_pTail = NULL;


}


int ISystemListImp::getNodesCount()
{

	return 	this->m_nNodesCount;
}


ISystemListNode* ISystemListImp::getSystemListNodeByIndex(int nIndex)
{
	ISystemListNode		*pNode = NULL;
	int					nPos = 0;

	if (nIndex >=  this->m_nNodesCount)
	{
		return NULL;
	}
	for (nPos = 0, pNode = this->m_pHead; (nPos < nIndex) && (pNode != NULL); nPos++)
	{
		pNode = pNode->getNextNode();
	}
	return pNode;

}





void ISystemListImp::removeSystemListNodeByIndex(int nIndex)
{
	ISystemListNode *pNode = NULL;
	ISystemListNode *pNextNode = NULL;
	ISystemListNode *pPrevNode = NULL;
	int nPos = 0;


	if (nIndex >=  this->m_nNodesCount)
	{
		return ;
	}
	for (nPos = 0, pNode = this->m_pHead; (nPos < nIndex) && (pNode != NULL); nPos++)
	{
		pNode = pNode->getNextNode();
	}

	pPrevNode = pNode->getPrevNode();
	pNextNode = pNode->getNextNode();

	pPrevNode->setNextNode(pNextNode);
	pNextNode->setPrevNode(pPrevNode);

	pNode->setNextNode(NULL);
	pNode->setPrevNode(NULL);
	delete pNode;
	pNode = NULL;



	pNextNode = NULL;
	pPrevNode = NULL;



}