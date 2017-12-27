///////////////////////////////////////////////////////////
//  ISystemLIstNodeDataImp.cpp
//  Implementation of the Class ISystemLIstNodeDataImp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////
#include <iostream>

#include "ISystemLIstNodeDataImp.h"
#include "CAllDataClass.h"
#include "CommonData.h"

ISystemLIstNodeDataImp::ISystemLIstNodeDataImp()
{
	m_nDataType = DataType_NULL;
	m_pData = NULL;
}



ISystemLIstNodeDataImp::~ISystemLIstNodeDataImp()
{
	if (DataType_NULL != m_nDataType && NULL != m_pData)
	{
		_FreeListNodeDataByDataType();
	}
	
	m_nDataType = DataType_NULL;
	m_pData = NULL;
}

void ISystemLIstNodeDataImp::_FreeListNodeDataByDataType()
{
	

	switch (this->m_nDataType)
	{
	case DataType_NULL:
		break;
	case DataType_CFileAllClass:
		{
			CFileAllClass *pGet = (CFileAllClass *)this->m_pData;
			delete pGet;
			pGet = NULL;
			this->m_pData = NULL;
		}
		break;
	case DataType_CTableAllClass:
		{
			CTableAllClass *pGet = (CTableAllClass *)this->m_pData;
			delete pGet;
			pGet = NULL;
			this->m_pData = NULL;
		}
		break;
	case DataType_CDbAllClass:
		{
			CDbAllClass *pGet = (CDbAllClass *)this->m_pData;
			delete pGet;
			pGet = NULL;
			this->m_pData = NULL;
		}
		break;
	case DataType_CSqlAllClass:
		{
			CSqlAllClass *pGet = (CSqlAllClass *)this->m_pData;
			delete pGet;
			pGet = NULL;
			this->m_pData = NULL;
		}
		break;
	default:
		break;
	}
}



void* ISystemLIstNodeDataImp::getData()
{

	return  this->m_pData;
}


enumDataType ISystemLIstNodeDataImp::getDataType()
{

	return this->m_nDataType;
}


void ISystemLIstNodeDataImp::setData(void* pData)
{
	this->m_pData = pData;
	return ;
}


void ISystemLIstNodeDataImp::setDataType(enumDataType nDataTypeValue)
{
	this->m_nDataType = nDataTypeValue;
}