///////////////////////////////////////////////////////////
//  ISystemList.h
//  Implementation of the Interface ISystemList
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_66AD7532_D478_4d3d_A5AD_7184FFA60354__INCLUDED_)
#define EA_66AD7532_D478_4d3d_A5AD_7184FFA60354__INCLUDED_

#include "ISystemListNode.h"

class ISystemList
{

public:
	ISystemList() {

	}

	virtual ~ISystemList() {

	}

	virtual void addSystemListNode(ISystemListNode* pSystemNode) =0;
	virtual void addSystemListNodeWithData(enumDataType nDataType, void* pData) =0;
	virtual void freeSystemList() =0;
	virtual int getNodesCount() =0;
	virtual ISystemListNode* getSystemListNodeByIndex(int nIndex) =0;
	virtual void removeSystemListNodeByIndex(int nIndex) =0;

};
#endif // !defined(EA_66AD7532_D478_4d3d_A5AD_7184FFA60354__INCLUDED_)
