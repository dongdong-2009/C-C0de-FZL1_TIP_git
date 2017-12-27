///////////////////////////////////////////////////////////
//  ISystemListNodeImp.h
//  Implementation of the Class ISystemListNodeImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_E6DD16B6_478E_4add_B8C2_8A1F9907A39F__INCLUDED_)
#define EA_E6DD16B6_478E_4add_B8C2_8A1F9907A39F__INCLUDED_

#include "ISystemListNodeData.h"
#include "ISystemListNode.h"

class ISystemListNodeImp : public ISystemListNode
{

public:
	ISystemListNodeImp();
	virtual ~ISystemListNodeImp();

	ISystemListNode* getNextNode();
	ISystemListNodeData* getNodeData();
	ISystemListNode* getPrevNode();
	void setPrevNode(ISystemListNode* pNode);
	void setNextNode(ISystemListNode* pNode);
	void setNodeData(ISystemListNodeData* pNodeData);

private:
	ISystemListNodeData* m_pData;
	ISystemListNode* m_pNext;
	ISystemListNode* m_pPrev;

};
#endif // !defined(EA_E6DD16B6_478E_4add_B8C2_8A1F9907A39F__INCLUDED_)
