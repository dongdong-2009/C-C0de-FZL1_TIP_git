//////////////////////////////////////////////////////////////////////
///		@file		CommonData.h
///		@author		zhilin,ouyang
///		@date		2012-01-05 11:44:41
///
///		@brief	    to define some common use data or structure which 
///                 used in the DB synchronization engine.
//////////////////////////////////////////////////////////////////////
#ifndef  COMMONDATA_H
#define  COMMONDATA_H
#include "CommonDef.h"

#include <memory.h>
#include <string>

//NS_BEGIN(TA_DB_Sync)

enum enumDataType
{
	DataType_NULL		   = 0,
	DataType_CFileAllClass      = 1,
	DataType_CTableAllClass     ,
	DataType_CDbAllClass     ,
	DataType_CSqlAllClass     ,


};

//NS_END(TA_DB_Sync)

#endif  // COMMONDATA_H





