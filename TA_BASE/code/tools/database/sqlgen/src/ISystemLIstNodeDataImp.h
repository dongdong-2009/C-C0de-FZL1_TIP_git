///////////////////////////////////////////////////////////
//  ISystemLIstNodeDataImp.h
//  Implementation of the Class ISystemLIstNodeDataImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_DD70314E_AAFA_4679_8621_C5D66D083CAF__INCLUDED_)
#define EA_DD70314E_AAFA_4679_8621_C5D66D083CAF__INCLUDED_

#include "CommonData.h"
#include "ISystemListNodeData.h"

class ISystemLIstNodeDataImp : public ISystemListNodeData
{

public:
	ISystemLIstNodeDataImp();
	virtual ~ISystemLIstNodeDataImp();

	void* getData();
	enumDataType getDataType();
	void setData(void* pData);
	void setDataType(enumDataType nDataTypeValue);
private:
	void _FreeListNodeDataByDataType();
private:
	enumDataType m_nDataType;
	void* m_pData;

};
#endif // !defined(EA_DD70314E_AAFA_4679_8621_C5D66D083CAF__INCLUDED_)
