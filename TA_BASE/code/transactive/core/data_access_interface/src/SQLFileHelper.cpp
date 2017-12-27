//////////////////////////////////////////////////////////////////////
///		@file		SQLFileHelper.cpp
///		@author		zhilin,ouyang
///		@date		2010-12-9 14:44:41
///
///		@brief	    to load the sql.dat file and store the SQL statements
///                 into a hash-table for later retrieve.
//////////////////////////////////////////////////////////////////////
#include "SQLFileHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/uuid/src/TAuuid.h"
#include "SQLUtilityFun.h"
#include <fstream>
#include <time.h>

#if defSQL_MODE_TXT
#include "sqlmacrodeftxt.h"
#else
#include "sql.h"
#endif 

#define  defSQLFileName        "sql.dat"
#define  defSQLTempFilePre     "sql_"
#define  defSQLTempFileEnd     ".txt"
#define  defSQLCommonSection   "[[common]]"
#define  defSQLOracleSection   "[[oracle]]"
#define  defSQLMySQLSection    "[[mysql]]"
#define  defSectionWordLen      20

NS_BEGIN(TA_Base_Core)

SQLFileHelper::SQLFileHelper() 
: m_strSQLFile(defSQLFileName)
{
#if defSQL_MODE_TXT
	_LoadSQLFile();
#endif
	m_nArrSize = sizeof(s_SQLArray) / sizeof (*s_SQLArray);

	_InitSQLHashIndex();
}

SQLFileHelper::SQLFileHelper(const char* pszSQLFile)
: m_strSQLFile(pszSQLFile)
{
#if defSQL_MODE_TXT
	_LoadSQLFile();
#endif
	m_nArrSize = sizeof(s_SQLArray) / sizeof (*s_SQLArray);

	_InitSQLHashIndex();
}

SQLFileHelper::~SQLFileHelper()
{
#if defSQL_MODE_TXT	
	m_SQLCommonHashTable.clear();
	m_SQLOracleHashTable.clear();
	m_SQLMySQLHashTable.clear();
#endif
	m_SQLHashTable.clear();
	m_hSQLHashIndex.clear();
}

std::string SQLFileHelper::getSQLString(u_long uSQLID)
{
	FUNCTION_ENTRY("SQLFileHelper::GetSQLString(u_long uSQLID)");		

#if defSQL_MODE_TXT

	if (0 == m_SQLHashTable.size())
	{
		// should never reach here, but in case of load file
		// failed at system start, reload it once more
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
			"SQLFileHelper::GetSQLString() The SQL hash table is empty when load SQLID: %ld", uSQLID);
		_LoadSQLFile();
	}
	TA_ASSERT(0 != m_SQLHashTable.size(), "Load sql.dat file failed");

#endif

	FUNCTION_EXIT;
	return m_SQLHashTable[uSQLID];
}

void  SQLFileHelper::getSQLString(const std::string& strSQLKey, SQLStatement& rSqlStatements)
{
	FUNCTION_ENTRY("SQLFileHelper::GetSQLStringconst std::string& strSQLKey)");	


	if (m_hSQLHashIndex.empty())
	{
		// should never reach here, but in case of load file
		// failed at system start, reload it once more
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
			"SQLFileHelper::GetSQLString() The SQL hash table is empty when load strSQLKey: %s", strSQLKey.c_str());
		_InitSQLHashIndex();
	}

	int nIndex = -1;
	//nIndex = m_hSQLHashIndex[strSQLKey];
	strSQLIndexIterT iterFound = m_hSQLHashIndex.find(strSQLKey);
	if (iterFound == m_hSQLHashIndex.end())
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
			"SQLFileHelper::GetSQLString() the SQLKey cannot match in Hash table. strSQLKey: %s", strSQLKey.c_str());
		TA_THROW(BadParamCount("the SQLKey cannot match in Hash table"));
	}

	nIndex = iterFound->second;

	rSqlStatements.strSQLID  = strSQLKey;
	rSqlStatements.strCommon = s_SQLArray[nIndex].pCommonSQL;
	rSqlStatements.strOracle = s_SQLArray[nIndex].pOracleSQL;
	rSqlStatements.strMySQL  = s_SQLArray[nIndex].pMySQLSQL;

	FUNCTION_EXIT;
}

void   SQLFileHelper::_InitSQLHashIndex()
{
	std::string  strSQLKey;

	for (int nIndex = 0; nIndex < m_nArrSize; nIndex++)
	{
		strSQLKey = s_SQLArray[nIndex].pSQLID;
		m_hSQLHashIndex.insert(strSQLHashIndexValT(strSQLKey, nIndex));
	}
}

#if defSQL_MODE_TXT
bool SQLFileHelper::_LoadSQLFile()
{
	FUNCTION_ENTRY("SQLFileHelper::_LoadSQLFile()");
	bool bFunRet = false;
	
	std::string strUUID = TA_Uuid::generateUUID(); // each process have different temp txt file name
	std::string  strTempFile(defSQLTempFilePre + strUUID + defSQLTempFileEnd);
	std::vector<std::string> vecAllLines;
	try
	{
		if (m_strSQLFile.empty())
			m_strSQLFile = defSQLFileName;
		_DeEncrySQLFile(strTempFile);

		bFunRet = _ReadAllLines(vecAllLines, strTempFile);
		TA_ASSERT(bFunRet, "Read sql file data error.");

		remove(strTempFile.c_str()); // after load, remove the temp file
	}
	catch (BadParamCount* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
	}
	catch (SQLFileNotExist* e)
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
	return bFunRet;
}

bool SQLFileHelper::_DeEncrySQLFile(const std::string& strTempFile)
{
	FUNCTION_ENTRY("SQLFileHelper::_DeEncrySQLFile(const std::string& strTempFile)");
	bool bFunRet = false;

	bFunRet = DecryptDatFile(m_strSQLFile, strTempFile);

	FUNCTION_EXIT;
	return bFunRet;
}

bool SQLFileHelper::_ReadAllLines(std::vector<std::string>& vecAllLines, const std::string& strFile)
{
	FUNCTION_ENTRY("SQLFileHelper::_ReadAllLines(std::vector<std::string>& vecAllLines, const std::string& strFile)");
	bool bFunRet = false;

	try
	{
		std::ifstream inFile(strFile.c_str());
		if (inFile.fail())
			TA_THROW(SQLFileNotExist("the sql temp file not exist"));

		char inBuf[MAX_SQLSTRING_LEN] = {0};
		// Read in all lines and add them to a vector
		while ( inFile.getline ( inBuf, MAX_SQLSTRING_LEN ) )
		{
			char * crPtr = strchr( inBuf, '\r' );
			if( 0 != crPtr )
			{
				*crPtr = '\0';
			}
			std::string line = std::string( inBuf );
			vecAllLines.push_back( line );
		}

		// close file
		inFile.close();

		// Fill the data in the hash map
		bFunRet = _FillAllHashData(vecAllLines);
	}
	catch (SQLFileNotExist* e)
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
	return bFunRet;
}

bool SQLFileHelper::_FillHashData(const std::vector<std::string>& vecAllLines)
{
	FUNCTION_ENTRY("SQLFileHelper::_FillHashData(const std::vector<std::string>& vecAllLines)");
	bool bFunRet = false;
	std::vector<std::string>::const_iterator iterAllLine = vecAllLines.begin();
	m_SQLHashTable.clear(); // before fill data, clean it first
	u_long uSQLID = 0;

	while ( iterAllLine != vecAllLines.end() )
	{
		std::string strLine(*iterAllLine);
		strLine = ltrim(strLine);
		// comment line, ignore it.
		if ( (!strLine.empty()) && strLine[0] == ';')  
		{
			iterAllLine++;
			continue;
		}
		// the SQL Statement separate in multiple lines
		if ( (!strLine.empty()) && strLine[0] == '+' &&
			 (!m_SQLHashTable[uSQLID].empty()) 
		   )
		{
			strLine = strLine.substr(1);
			m_SQLHashTable[uSQLID] += strLine;
			iterAllLine++;
			continue;
		}
		
		if ( (!strLine.empty()) &&
			 (strLine.find("[") == std::string::npos) &&
			 (strLine.find("]") == std::string::npos) &&
			  strLine[0] != '+')
		{
			size_t uPos = strLine.find_first_of("=");
			std::string strSQLID = strLine.substr(0, uPos);
			std::string strSQL   = strLine.substr(uPos + 1);
			uSQLID = atol(strSQLID.c_str());
			m_SQLHashTable.insert(SQLValueType(uSQLID, strSQL));
		}
		// next line
		iterAllLine++;
	}
	bFunRet = (m_SQLHashTable.size() > 0 ? true : false);

	FUNCTION_EXIT;
	return bFunRet;
}

bool SQLFileHelper::_FillAllHashData(const std::vector<std::string>& vecAllLines)
{
	FUNCTION_ENTRY("SQLFileHelper::_FillAllHashData(const std::vector<std::string>& vecAllLines)");
	bool bFunRet = false;
	cStrIterT iterAllLine = vecAllLines.begin();
	cStrIterT iterEnd     = vecAllLines.end();
	m_SQLCommonHashTable.clear(); // before fill data, clean it first
	m_SQLOracleHashTable.clear();
	m_SQLMySQLHashTable.clear();

	while ( iterAllLine != iterEnd )
	{
		std::string strLine(*iterAllLine);
		strLine = ltrim(strLine);
		// empty or comment line, ignore it.
		if ( (strLine.empty()) || strLine[0] == ';')  
		{
			iterAllLine++;
			continue;
		}
		if ( strLine.size() < defSectionWordLen )
		{
			std::string strSection = rtrim(strLine);
			char *pzSection = strtolwr((char*)strSection.c_str());
			if ( !strcmp(pzSection, defSQLCommonSection) )
			{
				_FillOneHashData(iterAllLine, m_SQLCommonHashTable, iterEnd);
			}
			else if ( !strcmp(pzSection, defSQLOracleSection) )
			{
				_FillOneHashData(iterAllLine, m_SQLOracleHashTable, iterEnd);
			}
			else if ( !strcmp(pzSection, defSQLMySQLSection) )
			{
				_FillOneHashData(iterAllLine, m_SQLMySQLHashTable, iterEnd);
			}
		}
		
		if (iterAllLine != iterEnd)
		{
			// next line
			iterAllLine++;
		}		
	}
	bFunRet = (m_SQLOracleHashTable.size() == m_SQLMySQLHashTable.size() ? true : false);

	FUNCTION_EXIT;
	return bFunRet;
}

void SQLFileHelper::_FillOneHashData(cStrIterT& rIterStr, strSQLHashTableT& rHashTable, cStrIterT& rIterEnd)
{
	std::string strSQLKey;
	rIterStr++;

	while (rIterStr != rIterEnd)
	{
		std::string strLine(*rIterStr);
		strLine = ltrim(strLine);
		// empty or comment line, ignore it.
		if ( (strLine.empty()) || strLine[0] == ';')  
		{
			rIterStr++;
			continue;
		}

		// the SQL Statement separate in multiple lines
		if ((strLine[0] == '+') &&
			(!rHashTable[strSQLKey].empty()) 
			)
		{
			strLine = strLine.substr(1);
			rHashTable[strSQLKey] += strLine;
			rIterStr++;
			continue;
		}

		if ((strLine.find("[") == std::string::npos) &&
			(strLine.find("]") == std::string::npos) &&
			strLine[0] != '+')
		{
			size_t uPos = strLine.find_first_of("=");
			strSQLKey = strLine.substr(0, uPos);
			std::string strSQL   = strLine.substr(uPos + 1);
			rHashTable.insert(strSQLValueType(strSQLKey, strSQL));
		}
		else
		{
			rIterStr--;
			break;
		}

		// next line
		rIterStr++;		
	}
}

#endif  // for text mode

NS_END(TA_Base_Core)