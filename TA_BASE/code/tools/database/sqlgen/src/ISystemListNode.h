///////////////////////////////////////////////////////////
//  ISystemListNode.h
//  Implementation of the Interface ISystemListNode
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_589C7FB2_7DC0_4824_A621_480CEAA587B4__INCLUDED_)
#define EA_589C7FB2_7DC0_4824_A621_480CEAA587B4__INCLUDED_

#include "ISystemListNodeData.h"

class ISystemListNode
{

public:
	ISystemListNode() {

	}

	virtual ~ISystemListNode() {

	}

	virtual ISystemListNode* getNextNode() =0;
	virtual ISystemListNodeData* getNodeData() =0;
	virtual ISystemListNode* getPrevNode() =0;
	virtual void setPrevNode(ISystemListNode* pNode) =0;
	virtual void setNextNode(ISystemListNode* pNode) =0;
	virtual void setNodeData(ISystemListNodeData* pNodeData) =0;

};
#endif // !defined(EA_589C7FB2_7DC0_4824_A621_480CEAA587B4__INCLUDED_)
