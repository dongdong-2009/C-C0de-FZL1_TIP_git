//////////////////////////////////////////////////////////////////////
///		@file		SQLFileHelper.cpp
///		@author		zhilin,ouyang
///		@date		2010-12-9 14:44:41
///
///		@brief	    to load the sql.dat file and store the SQL statements
///                 into a hash-table for later retrieve.
//////////////////////////////////////////////////////////////////////
#include "SQLCodeHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "sql.h"

NS_BEGIN(TA_AQ_Lib)

CSQLCodeHelper::CSQLCodeHelper() 
{
	m_nArrSize  =  countof(s_pzSQLArray);
}

CSQLCodeHelper::~CSQLCodeHelper()
{

}

void CSQLCodeHelper::getSQLString(int nDbType, const std::string& strSQLKey, std::string& strSQL)
{
	//FUNCTION_ENTRY("SQLFileHelper::GetSQLStringconst std::string& strSQLKey)");	

	int nSQLIndex = atoi(strSQLKey.c_str());
	TA_ASSERT( nSQLIndex >= 0 && nSQLIndex < m_nArrSize, "Invalid SQL array index...");
	strSQL = s_pzSQLArray[nSQLIndex];
	if ( !strSQL.empty() )
	{
		//FUNCTION_EXIT;
		return ;
	}

	switch (nDbType)
	{
	case enumOracleDb:
		strSQL = s_pzSQLArray[nSQLIndex + enumOracleDb];
		break;
	case enumMysqlDb:
		strSQL = s_pzSQLArray[nSQLIndex + enumMysqlDb];
		break;
	default:
		strSQL = s_pzSQLArray[nSQLIndex];
		break;
	}

	//FUNCTION_EXIT;
	return ;
}


NS_END(TA_AQ_Lib)











