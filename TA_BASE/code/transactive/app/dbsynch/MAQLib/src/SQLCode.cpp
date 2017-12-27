//////////////////////////////////////////////////////////////////////
///		@file		SQLCode.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to hold all of the SQL statements used in our 
///                 system in one location. The SQLCode class is based 
///                 on the Singleton design pattern.
//////////////////////////////////////////////////////////////////////
#include "SQLCode.h"
#include "sqldef.h"
#include "SQLCodeHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


NS_BEGIN(TA_AQ_Lib)

CSQLCode* CSQLCode::m_pInstance = 0;
NonReEntrantThreadLockable CSQLCode::m_instanceLock;

CSQLCode& CSQLCode::getInstance()
{
	//FUNCTION_ENTRY("SQLCode::GetInstance()");
	if (NULL != m_pInstance)
	{
		return (*m_pInstance);
	}

	ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new CSQLCode();
		LOG0(SourceInfo, DebugUtil::DebugTrace,"new instance");
	}
	//FUNCTION_EXIT;
	return (*m_pInstance);
}

void CSQLCode::removeInstance()
{
	DEF_DELETE(m_pInstance);
}


CSQLCode::CSQLCode()
{
	m_pSqlCodeHelper = new CSQLCodeHelper();
}

CSQLCode::~CSQLCode()
{
	DEF_DELETE(m_pSqlCodeHelper);
}

void CSQLCode::buildSQLStatement(const SQLVarParms& varParms, std::string& strSQL)
{
//	LOG0(SourceInfo, DebugUtil::DebugTrace,"SQLCode::_ConstructSQLStatement()");
	std::string strSQLFormat, strSQLKey;
	int nDbType = 0;
	size_t uiTotalSQLSize = 0;	

	try
	{
		size_t uVarCount = varParms.size();
		if (uVarCount < defMiniSQLParaSize)
			TA_THROW(BadParamCount("the PrepareStatement parameter count error"));

		_GetDbTypeAndSQLKey(varParms, nDbType, strSQLKey);
		//LOG_GENERIC(SourceInfo,  DebugUtil::DebugInfo, "SQL Id: %lu", uSQLID);

		{
			//ThreadGuard guard(m_threadLock); // avoid threads racing
			m_pSqlCodeHelper->getSQLString(nDbType, strSQLKey, strSQLFormat);
		}
		
		if (strSQLFormat.empty())
			TA_THROW(BadParamCount("Cannot find the SQL statement in the hash-table"));	
		//TA_ASSERT(_CheckStrCount(strSQLFormat.c_str(), "%s") == uVarCount - defMiniSQLParaSize, "the SQL dynamic parameter count not match.")
		
		uiTotalSQLSize = _GetSQLSize(varParms, strSQLFormat);
		if ( uiTotalSQLSize > defMaxSQLLen )
		{
			_BuildLargeSQL(varParms, uiTotalSQLSize, strSQLFormat, strSQL);
		}
		else
		{
			_BuildSQL(varParms, strSQLFormat, strSQL);
		}

	//	TA_ASSERT(strSQL.size() > 0, "the SQL statement is empty");
		if ( strSQL.size() > defMaxLogSize)
	{
			LOG2(SourceInfo, DebugUtil::DebugSQL,"SQLID: %s, DbType:%d, LargeSQL: ", strSQLKey.c_str(), nDbType);
			LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, strSQL.c_str());
	}
		else
	{
			LOG3(SourceInfo, DebugUtil::DebugSQL,
				"SQLID: %s, DbType:%d, SQL: %s", strSQLKey.c_str(), nDbType, strSQL.c_str());
	}
		
	}
	catch (BadParamCount* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
	}
	catch (BadIndex* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
}
	catch (...)
{
		SQLCodeException SQLException("Unknown SQLCode exception");
		throw SQLException;
}

//	LOG0(SourceInfo, DebugUtil::DebugTrace,"SQLCode::_ConstructSQLStatement() Exit");
}

inline
void  CSQLCode::_GetDbTypeAndSQLKey(const SQLVarParms& varParms, int& nDbType, std::string& strSQLKey)
{
	nDbType = atoi(varParms[0]);
	strSQLKey = varParms[1];
}

size_t  CSQLCode::_CheckStrCount(const char* pSrc, const char* pTarget)
{
	size_t  nCount = 0;
	size_t  nCmpSize = strlen(pTarget);
	int     nCmpVal  = *pTarget;
	const char  *pPos = strchr(pSrc, nCmpVal);
	while ( pPos )
	{
		if(!strncmp(pPos, pTarget, nCmpSize))
			nCount++;
		pPos += nCmpSize;
		pPos = strchr(pPos, nCmpVal);
	}

	return nCount;
}

size_t  CSQLCode::_GetSQLSize(const SQLVarParms& varParms, const std::string& strFormat)
{
	size_t uiTotalSize = strFormat.size();
	size_t uVarCount   = varParms.size();
	if ( defMiniSQLParaSize == uVarCount )
		return uiTotalSize;

	for (size_t i = defMiniSQLParaSize; i < varParms.size(); i++)
	{
		uiTotalSize += varParms[i].length();
	}

	return uiTotalSize;
}

void  CSQLCode::_BuildSQL(const SQLVarParms& varParms, const std::string& strSQLFormat, std::string& strSQL)
{	
	size_t uVarCount = varParms.size();
	if (defMiniSQLParaSize == uVarCount)	
	{
		strSQL = strSQLFormat;	
	}
	else
	{
		char szSQL[defMaxSQLLen+1] = {0};

		DEF_CONSTRUCT_SQLCODE(defMaxSQLLen);
		strSQL = szSQL;
	}
}

void  CSQLCode::_BuildLargeSQL(const SQLVarParms& varParms, size_t uSQLSize, const std::string& strSQLFormat, std::string& strSQL)
{
	size_t uVarCount = varParms.size();
	if (defMiniSQLParaSize == uVarCount)	
	{
		strSQL = strSQLFormat;	
	}
	else
	{
		char *szSQL = new char[uSQLSize + 1];		
		memset(szSQL, 0, uSQLSize + 1);

		DEF_CONSTRUCT_SQLCODE(uSQLSize);
		strSQL = szSQL;

		DEF_DELETE_ARRAY(szSQL);
	}
	
}

NS_END(TA_AQ_Lib)
