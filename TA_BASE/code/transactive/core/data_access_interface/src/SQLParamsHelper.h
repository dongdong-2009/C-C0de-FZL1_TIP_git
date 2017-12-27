//////////////////////////////////////////////////////////////////////
///		@file		SQLParamsHelper.h
///		@author		zhilin,ouyang
///		@date		2012-12-11 14:44:41
///
///		@brief	    to help class SQLTypeAdapter to accept dynamic 
///                 parameters between different database such as 
///                 datetime format different between Oracle & MySQL.
///                             
//////////////////////////////////////////////////////////////////////
#ifndef SQLPARAMSHELPER_H
#define SQLPARAMSHELPER_H
#include "SQLCommonDef.h"
#include <string>

NS_BEGIN(TA_Base_Core)

typedef struct st_dynamic_data_tag
{
	std::string   strOracl;
	std::string   strMysql;
}DynamicData, *PDynamicData;

class CSQLParamsHelper
{

public:
	 void getOperator(int nKey, std::string& strVal);
	 void getDynamicSQLPara(int nKey, DynamicData& rData);
};

NS_END(TA_Base_Core)

#endif /// SQLPARAMSHELPER_H
