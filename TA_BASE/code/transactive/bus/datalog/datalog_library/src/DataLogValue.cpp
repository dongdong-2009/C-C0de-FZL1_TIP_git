///////////////////////////////////////////////////////////
//  DataLogValue.cpp
//  Implementation of the Class DataLogValue
//  Created on:      13-Ê®¶þÔÂ-2010 14:08:00
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#include "DataLogValue.h"
namespace TA_Base_Bus
{
	tm DataLogValue::getCreateTime()
	{
		return m_createTime;
	}

	tm DataLogValue::getPlanLogTime()
	{
		return m_planLogTime;
	}
	

	unsigned long DataLogValue::getEntityKey()
	{
		return m_entityKey;
	}

	double DataLogValue::getValue()
	{
		return m_value;
	}


	DataLogValue::~DataLogValue()
	{

	}
}