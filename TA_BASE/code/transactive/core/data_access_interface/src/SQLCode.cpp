//////////////////////////////////////////////////////////////////////
///		@file		SQLCode.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to hold all of the SQL statements used in our 
///                 system in one location. The SQLCode class is based 
///                 on the Singleton design pattern.
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "SQLCode.h"
#include "SQLFileHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

NS_BEGIN(TA_Base_Core)

static const char*  s_pcAQOraclePrefix  = "BEGIN audit_data_queue_pkg.enqueue_audit_data";
static const char*  s_pcAQOracleSubfix  = "; END;";
static const char*  s_pcAQMySQLPrefix   = "CALL prc_enqueue_audit_data";
static const char*  s_pcAQMySQLSubfix   = "";
static const char*  s_pcAQTag           = "AQ:";
static const char*  s_pcAQOracleDelim   = "||";
static const char*  s_pcAQMySQLDelim    = ",";
static const char*  s_pcAQMySQLConStr   = "CONCAT(";
static const char*  s_pcAQStrTag        ="'";
static const size_t s_nAQTagLen         = 3;
static const size_t s_nStrBeginPos      = 0;

SQLCode* SQLCode::m_pInstance = 0;
NonReEntrantThreadLockable SQLCode::m_instanceLock;

SQLCode& SQLCode::getInstance()
{
	FUNCTION_ENTRY("SQLCode::GetInstance()");
	 ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new SQLCode();
	}

	FUNCTION_EXIT;
	return (*m_pInstance);
}

void SQLCode::removeInstance()
{
	if ( NULL != m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


SQLCode::SQLCode()
{
	LOG0(SourceInfo, DebugUtil::DebugSQL,"Begin build the Hash table...");

	m_pSqlFileHelper = new SQLFileHelper();

	LOG0(SourceInfo, DebugUtil::DebugSQL,"End build the Hash table...");
}

SQLCode::~SQLCode()
{
	if (NULL != m_pSqlFileHelper)
	{
		delete m_pSqlFileHelper;
		m_pSqlFileHelper = NULL;
	}
}

void SQLCode::buildSQLStatement(SQLVarParms& varParms, SQLStatement& rSqlStatement)
{
	FUNCTION_ENTRY("SQLCode::_ConstructSQLStatement()");
	std::string strSQLKey;
	SQLStatement SQLFormats;
	int nDbType = 0;
	size_t uiTotalSQLSize = 0;	

	try
	{
		size_t uVarCount = varParms.size();
		if (uVarCount < defMINPARAMSIZE)
			TA_THROW(BadParamCount("the PrepareStatement parameter count error"));

		_GetDbTypeAndSQLKey(varParms, strSQLKey);
		_GetSQLFormat(strSQLKey, SQLFormats);
		_GetSQLID(SQLFormats, rSqlStatement);

		if (_ChkAQSQL(SQLFormats))
		{
			_BuildAQSQL(varParms, SQLFormats, rSqlStatement); 
		}
		else
		{
			_BuildNormalSQL(varParms, SQLFormats, rSqlStatement);
		}

		_PrintSQL(strSQLKey, rSqlStatement);			
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
	catch(DbTypeNotSupported* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
	}
	catch (...)
	{
		SQLCodeException SQLException("Unknown SQLCode exception");
		throw SQLException;
	}

	FUNCTION_EXIT;
}

inline
void  SQLCode::_GetDbTypeAndSQLKey(const SQLVarParms& varParms, std::string& strSQLKey)
{
	strSQLKey = varParms[0].data(enumBeginDbType);
}

size_t  SQLCode::_GetSQLSize(const SQLVarParms& varParms, const std::string& strFormat, int nSQLType)
{
	size_t uiTotalSize = strFormat.size();
	size_t uVarCount   = varParms.size();
	if ( defMINPARAMSIZE == uVarCount )
		return uiTotalSize;

	size_t i = defMINPARAMSIZE;	
	for (; i < uVarCount; i++)
	{
		uiTotalSize += varParms[i].length(nSQLType);
	}

	return uiTotalSize;
}

void  SQLCode::_BuildSQL(const SQLVarParms& varParms, const std::string& strSQLFormat, std::string& strSQL, int nSQLType)
{	
	size_t uVarCount = varParms.size();
	if (defMINPARAMSIZE == uVarCount)	
	{
		strSQL = strSQLFormat;	
	}
	else
	{
		char szSQL[MAX_SQLSTRING_LEN+1] = {0};

		DEF_CONSTRUCT_SQLCODE(MAX_SQLSTRING_LEN, nSQLType);		

		strSQL = szSQL;
	}
}

void  SQLCode::_BuildLargeSQL(const SQLVarParms& varParms, size_t uSQLSize, const std::string& strSQLFormat, std::string& strSQL, int nSQLType)
{
	size_t uVarCount = varParms.size();
	if (defMINPARAMSIZE == uVarCount)	
	{
		strSQL = strSQLFormat;	
	}
	else
	{
		char *szSQL = new char[uSQLSize + 1];		
		memset(szSQL, 0, uSQLSize + 1);

		DEF_CONSTRUCT_SQLCODE(uSQLSize, nSQLType);		

		strSQL = szSQL;

		delete[] szSQL;
		szSQL = 0;
	}
	
}

void  SQLCode::_GetSQLFormat(const std::string& strSQLKey, SQLStatement& strSQLFormats)
{
	ThreadGuard guard(m_threadLock); // avoid threads racing
	TA_ASSERT(NULL != m_pSqlFileHelper, "sql file helper handler is null.");
	m_pSqlFileHelper->getSQLString(strSQLKey, strSQLFormats);

	if (strSQLFormats.strCommon.empty() && strSQLFormats.strMySQL.empty() && strSQLFormats.strOracle.empty())
		TA_THROW(BadParamCount("Cannot find the SQL statement in the hash-table"));	
}

inline
void  SQLCode::_GetSQLID(const std::string& strSQLKey, SQLStatement& rSqlStatement)
{
	rSqlStatement.nSQLID = atoi(strSQLKey.c_str());
}

inline
void  SQLCode::_GetSQLID(const SQLStatement& rSrcSQL, SQLStatement& rSqlStatement)
{
	rSqlStatement.strSQLID = rSrcSQL.strSQLID;
}

bool  SQLCode::_ChkAQSQL(SQLStatement& strSQLFormats)
{
	bool bRetCode = false;

	if (strSQLFormats.strMySQL.empty() && strSQLFormats.strOracle.empty())
	{
		return bRetCode;
	}

	if (!strnicmp(strSQLFormats.strMySQL.c_str(), s_pcAQTag, s_nAQTagLen))
	{
		bRetCode = true;
	}

	return bRetCode;
}

void  SQLCode::_PrintSQL(const std::string& strSQLKey, SQLStatement& rSqlStatement)
{
	if (!rSqlStatement.strCommon.empty())
	{
		if ( rSqlStatement.strCommon.size() > MAXLOGMESGSIZE)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"SQLID: %s, LargeSQL: ", strSQLKey.c_str());
			LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, rSqlStatement.strCommon.c_str());
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,
				"SQLID: %s, SQL: %s", strSQLKey.c_str(), rSqlStatement.strCommon.c_str());
		}
	}
	if (!rSqlStatement.strMySQL.empty())
	{
		if ( rSqlStatement.strMySQL.size() > MAXLOGMESGSIZE)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"SQLID: %s, MySQL LargeSQL: ", strSQLKey.c_str());
			LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, rSqlStatement.strMySQL.c_str());
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,
				"SQLID: %s, MySQL SQL: %s", strSQLKey.c_str(), rSqlStatement.strMySQL.c_str());
		}
	}
	if (!rSqlStatement.strOracle.empty())
	{
		if ( rSqlStatement.strOracle.size() > MAXLOGMESGSIZE)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"SQLID: %s, Oracle LargeSQL: ", strSQLKey.c_str());
			LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, rSqlStatement.strOracle.c_str());
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,
				"SQLID: %s, Oracle SQL: %s", strSQLKey.c_str(), rSqlStatement.strOracle.c_str());
		}
	}
}

void  SQLCode::_BuildAQSQL(SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement)
{
	std::string strTemSQL, strOracle, strMySQL;
	int  nDbT = enumBeginDbType;
	int  nInsertPos = 0;
	size_t uiTotalSQLSize = 0;
	while (++nDbT != enumEndDbType)
	{
		switch (nDbT)
		{
		case enumOracleDb:
			uiTotalSQLSize = _GetSQLSize(varParms, rSQLFormats.strOracle, enumOracle_SQL);
			if ( uiTotalSQLSize > MAX_SQLSTRING_LEN )
			{
				_BuildLargeSQL(varParms, uiTotalSQLSize, rSQLFormats.strOracle, strTemSQL, enumOracle_SQL);
			}
			else
			{
				_BuildSQL(varParms, rSQLFormats.strOracle, strTemSQL, enumOracle_SQL);
			}

			strOracle = strTemSQL.substr(s_nAQTagLen);
			break;

		case enumMysqlDb:
			uiTotalSQLSize = _GetSQLSize(varParms, rSQLFormats.strMySQL, enumMySQL_SQL);
			if ( uiTotalSQLSize > MAX_SQLSTRING_LEN )
			{
				_BuildLargeSQL(varParms, uiTotalSQLSize, rSQLFormats.strMySQL, strTemSQL, enumMySQL_SQL);
			}
			else
			{
				_BuildSQL(varParms, rSQLFormats.strMySQL, strTemSQL, enumMySQL_SQL);
			}
			strMySQL  = strTemSQL.substr(s_nAQTagLen);
			break;
		}
	}

//#ifdef TDS_DEVELOP
	if((TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).compare("ConfigurationEditor") == 0) &&
		(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_TDSCE)))
	{
		_BuildTDSAQSQL(rSqlStatement, strOracle, strMySQL);
	}
//#endif

	_BuildMultiSQL(strOracle,  strMySQL);

	rSqlStatement.strOracle = s_pcAQOraclePrefix;
	rSqlStatement.strOracle +=  strMySQL;
	rSqlStatement.strOracle +=  s_pcAQOracleSubfix;

	rSqlStatement.strMySQL =  s_pcAQMySQLPrefix;
	rSqlStatement.strMySQL += strMySQL;	
}


void  SQLCode::_BuildNormalSQL(const SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement)
{
	if (!rSQLFormats.strCommon.empty())
	{
		size_t uiTotalSQLSize = _GetSQLSize(varParms, rSQLFormats.strCommon);
		if ( uiTotalSQLSize > MAX_SQLSTRING_LEN )
		{
			_BuildLargeSQL(varParms, uiTotalSQLSize, rSQLFormats.strCommon, rSqlStatement.strCommon);
		}
		else
		{
			_BuildSQL(varParms, rSQLFormats.strCommon, rSqlStatement.strCommon);
		}
	}
	else
	{
		if (rSQLFormats.strMySQL.empty() || rSQLFormats.strOracle.empty())
			TA_THROW(BadParamCount("The MySQL or Oracle SQL Format is empty"));

		// build MySQL SQL statement
		size_t uiTotalSQLSize = _GetSQLSize(varParms, rSQLFormats.strMySQL, enumMySQL_SQL);
		if ( uiTotalSQLSize > MAX_SQLSTRING_LEN )
		{
			_BuildLargeSQL(varParms, uiTotalSQLSize, rSQLFormats.strMySQL, rSqlStatement.strMySQL, enumMySQL_SQL);
		}
		else
		{
			_BuildSQL(varParms, rSQLFormats.strMySQL, rSqlStatement.strMySQL, enumMySQL_SQL);
		}
		
		// build Oracle SQL Statement
		uiTotalSQLSize = _GetSQLSize(varParms, rSQLFormats.strOracle, enumOracle_SQL);
		if ( uiTotalSQLSize > MAX_SQLSTRING_LEN )
		{
			_BuildLargeSQL(varParms, uiTotalSQLSize, rSQLFormats.strOracle, rSqlStatement.strOracle, enumOracle_SQL);
		}
		else
		{
			_BuildSQL(varParms, rSQLFormats.strOracle, rSqlStatement.strOracle, enumOracle_SQL);
		}
	}	
//#ifdef TDS_DEVELOP
	if((RunParams::getInstance().get(RPARAM_ENTITYNAME).compare("ConfigurationEditor") == 0) &&
		(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_TDSCE)))
	{
		rSqlStatement.bIsTdsAQ = false;
	}
//#endif
}

void  SQLCode::_BuildMultiSQL(const std::string& strOracle, std::string& strMultiSQL)
{
	std::string::size_type  nTempPos = 0, nTempEnd = 0, nInsertPos = 0;
	std::string strTempSQL;

	nTempPos   = strOracle.find_first_of(defAQSQL_Demit);
	nTempPos   = strOracle.find_first_of(defAQSQL_Demit, nTempPos + 1);
	nTempEnd   = strOracle.find_last_of(defAQSQL_EndTag);
	strTempSQL = strOracle.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));
	strTempSQL += defAQSQL_Demit;

	nInsertPos  = strMultiSQL.find_first_of(defAQSQL_Demit);
	nInsertPos  = strMultiSQL.find_first_of(defAQSQL_Demit, nInsertPos + 1);
	nInsertPos++;
	
	strMultiSQL.insert(nInsertPos, strTempSQL);

}

//#ifdef TDS_DEVELOP
void  SQLCode::_BuildTDSAQSQL(SQLStatement& rSqlStatement, const std::string& strOracle, std::string& strMySQL)
{
	std::string::size_type  nTempPos = 0, nTempEnd = 0;

	rSqlStatement.bIsTdsAQ = true;
	nTempPos   = strOracle.find_first_of(defAQSQL_Demit);
	nTempPos   = strOracle.find_first_of(defAQSQL_Demit, nTempPos + 1);
	nTempPos   = strOracle.find_first_of(s_pcAQStrTag, nTempPos);
	nTempEnd   = strOracle.find_last_of(s_pcAQStrTag);
	rSqlStatement.strTDSOracle = strOracle.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));

	nTempPos   = strMySQL.find_first_of(defAQSQL_Demit);
	nTempPos   = strMySQL.find_first_of(defAQSQL_Demit, nTempPos + 1);
	nTempPos   = strMySQL.find_first_of(s_pcAQStrTag, nTempPos);
	nTempEnd   = strMySQL.find_last_of(s_pcAQStrTag);
	rSqlStatement.strTDSMySQL = strMySQL.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));
}
//#endif


NS_END(TA_Base_Core)
