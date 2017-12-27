///////////////////////////////////////////////////////////
//  ISystemListNodeImp.cpp
//  Implementation of the Class ISystemListNodeImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////
#include <iostream>

#include "ISystemListNodeImp.h"


ISystemListNodeImp::ISystemListNodeImp()
{

	this->m_pData = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

}



ISystemListNodeImp::~ISystemListNodeImp()
{
	if (NULL != this->m_pData)
	{
		delete this->m_pData;
		this->m_pData = NULL;
	}

	this->m_pData = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

}




ISystemListNode* ISystemListNodeImp::getNextNode()
{
	

	return  this->m_pNext;
}


ISystemListNodeData* ISystemListNodeImp::getNodeData()
{


	return  this->m_pData;
}


ISystemListNode* ISystemListNodeImp::getPrevNode(){

	return  this->m_pPrev;
}


void ISystemListNodeImp::setNodeData(ISystemListNodeData* pNodeData)
{
	this->m_pData = pNodeData;
}

void ISystemListNodeImp::setPrevNode( ISystemListNode* pNode )
{
	this->m_pPrev = pNode;
}

void ISystemListNodeImp::setNextNode( ISystemListNode* pNode )
{
	this->m_pNext = pNode;
}
