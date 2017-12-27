///////////////////////////////////////////////////////////
//  ISystemListImp.h
//  Implementation of the Class ISystemListImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_3C40769B_23E4_4e3e_8450_4725CF866C6A__INCLUDED_)
#define EA_3C40769B_23E4_4e3e_8450_4725CF866C6A__INCLUDED_

#include "ISystemListNode.h"
#include "ISystemList.h"

class ISystemListImp : public ISystemList
{

public:
	ISystemListImp();
	virtual ~ISystemListImp();

	void addSystemListNode(ISystemListNode* pSystemNode);
	void addSystemListNodeWithData(enumDataType nDataType, void* pData);
	void freeSystemList();
	int getNodesCount();
	ISystemListNode* getSystemListNodeByIndex(int nIndex);
	void removeSystemListNodeByIndex(int nIndex);

private:
	int m_nNodesCount;
	ISystemListNode* m_pHead;
	ISystemListNode* m_pTail;

};
#endif // !defined(EA_3C40769B_23E4_4e3e_8450_4725CF866C6A__INCLUDED_)
