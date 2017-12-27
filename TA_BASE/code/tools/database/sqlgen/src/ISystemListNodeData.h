///////////////////////////////////////////////////////////
//  ISystemListNodeData.h
//  Implementation of the Interface ISystemListNodeData
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_9324F2C7_0CB8_4468_93C2_CA7BDF52F2F9__INCLUDED_)
#define EA_9324F2C7_0CB8_4468_93C2_CA7BDF52F2F9__INCLUDED_

#include "CommonData.h"

class ISystemListNodeData
{

public:
	ISystemListNodeData() {

	}

	virtual ~ISystemListNodeData() 
	{

	}

	virtual void* getData() =0;
	virtual enumDataType getDataType() =0;
	virtual void setData(void* pData) =0;
	virtual void setDataType(enumDataType nDataTypeValue) =0;

};
#endif // !defined(EA_9324F2C7_0CB8_4468_93C2_CA7BDF52F2F9__INCLUDED_)
