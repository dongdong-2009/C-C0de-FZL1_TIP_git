
//////////////////////////////////////////////////////////////////////
///		@file		ConfigFileHelper.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-06 16:44:41
///
///		@brief	    implementation of the config file I/O for the system. 
///                 for example read the config file at start.
///                 
//////////////////////////////////////////////////////////////////////
#include "ConfigInfoHelper.h"
#include "UtilityFun.h"

#if !defined( WIN32 )
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif // !WIN32

#include "SQLCode.h"
#include "DatabaseImpl.h"
#include "DbDataImpl.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/RunParams.h"
#include "TcpConnectionPool.h"
#include "CMysqlEnv.h"

NS_BEGIN(TA_DB_Sync)

using TA_Base_Core::DebugUtil;

struct DebugLevelNameValue
{
	const char*             name;
	DebugUtil::EDebugLevel   value;
};

static DebugLevelNameValue nameValueMapping[] =
{
	{ "FATAL",		DebugUtil::DebugFatal		},
	{ "ERROR",		DebugUtil::DebugError		},
	{ "WARNING",	DebugUtil::DebugWarn		},
	{ "NORMAL",		DebugUtil::DebugNorm		},
	{ "INFO",		DebugUtil::DebugInfo		},
	{ "DEBUG",		DebugUtil::DebugDebug		},
	{ "SQL",        DebugUtil::DebugSQL         },
	{ "CORBA",      DebugUtil::DebugCORBA       },
	{ "MSG",	    DebugUtil::DebugMsg	        },
	{ "MSGPUB",	    DebugUtil::DebugMsgPub	    },
	{ "PERIODIC",   DebugUtil::DebugPeriodic    },
	{ "DATABASE",	DebugUtil::DebugDatabase	},
	{ "EXTERNAL",	DebugUtil::DebugExternal	},
	{ "TRACE",		DebugUtil::DebugTrace		},
	{ NULL,			DebugUtil::DebugInfo		}
};

using namespace TA_Base_Core;  // for exceptions
CConfigInfoHelper::CConfigInfoHelper(void)
{
	m_mapValuePair.clear();
	m_pDatabase = new CDatabaseImpl();
}

CConfigInfoHelper::CConfigInfoHelper(const char* pczFileName)
: m_strFileName(pczFileName)
{
	m_mapValuePair.clear();
	m_pDatabase = new CDatabaseImpl();
}

CConfigInfoHelper::CConfigInfoHelper(const std::string& strFileName)
: m_strFileName(strFileName)
{
	m_mapValuePair.clear();
	m_pDatabase = new CDatabaseImpl();
}


CConfigInfoHelper::~CConfigInfoHelper(void)
{
	m_mapValuePair.clear();
	DEF_DELETE(m_pDatabase);
}

int CConfigInfoHelper::initConfigParameter(pg_config_parameter pConfigPara)
{
	int nRet = Ret_Success;
	
	nRet = _InitDataFromFile(pConfigPara);				

	return nRet;
}

void CConfigInfoHelper::setDBConStr(const std::string& strCon)
{
	if (!m_pDatabase)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	m_pDatabase->setDbConStr(strCon);
}

int  CConfigInfoHelper::monitorDBConfig(mapAllQThreadPoolT& rWorkThreadPool, mapAllQThreadPoolT& rNewThreadPool)
{
	int nRet = Ret_Success;

	nRet = _MonitorDBConfig(rWorkThreadPool, rNewThreadPool);

	return nRet;
}


int  CConfigInfoHelper::_InitDataFromFile(pg_config_parameter pConfigPara)
{
	int nRet = Ret_Success;
	vecLineT vecAllLines;

	try
	{
		if (m_strFileName.empty())
			m_strFileName = defDefault_ConfigFileName;

		nRet = _LoadConfigFile(vecAllLines);
		if ( Ret_Failed == nRet )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_NoData_inFile, defErrMsg(Err_NoData_inFile));
			TA_THROW(FileNotExist(Err_NoData_inFile, defErrMsg(Err_NoData_inFile)));
		}

		nRet = _FillCfgData(pConfigPara);
		if ( Ret_Failed == nRet )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_NoData_inFile, defErrMsg(Err_NoData_inFile));
			return nRet;
		}
		
		nRet = _GetAgentNameFromDB(pConfigPara);
		if ( Ret_Failed == nRet )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_NoData_inFile, defErrMsg(Err_NoData_inFile));
			return nRet;
		}
		
		if (enumOracleDb == pConfigPara->m_nDBType)
		{
			nRet = _GetAQSysQueueManagerInfoFromDB();
			if ( Ret_Failed == nRet )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_NoData_inFile, defErrMsg(Err_NoData_inFile));
				return nRet;
			}
		}		

	}
	catch (FileNotExist* e)
	{
		DbSyncException DBSyncException(e->getErrorCode(), e->what());
		throw DBSyncException;
	}
	catch(const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"ErrorCode: %d, Error message: %s", e.getNativeCode(), e.getReason().c_str() );
		DbSyncException DBSyncException(e.what());
		throw DBSyncException;
	}
	catch (...)
	{
		DbSyncException DBSyncException(defErrMsg(Err_Unknown));
		throw DBSyncException;
	}

	return nRet;
}

int  CConfigInfoHelper::_MonitorDBConfig(mapAllQThreadPoolT& rWorkThreadPool, mapAllQThreadPoolT& rNewThreadPool)
{
	int nRetCode = Ret_Success;

	rNewThreadPool.clear();
	mapQSubCntT mapSubCnt;
	std::string strQName;		
	//get queue info list from m table
	nRetCode = _GetQSubCntFromDB(mapSubCnt);
	DEF_ERROR_CHECK(!nRetCode);

	mapAllQThreadPoolIterT iterThreadPool;
	mapQSubCntIterT iterSubCnt = mapSubCnt.begin();
	while (iterSubCnt != mapSubCnt.end())
	{
		iterThreadPool = rWorkThreadPool.find(iterSubCnt->first);
		if (iterThreadPool != rWorkThreadPool.end())  //found
		{
			//TODO:: add one and remove one, the size is same
			//       if remove one, it will not remove the old one
			if (iterSubCnt->second.uiSubCnt != iterThreadPool->second->size())	  
			{
				strQName = iterSubCnt->first;
				std::string strExistSubID;				
				_GetExistSubIDs(iterThreadPool->second, strExistSubID);
				mapQThreadPoolT* pNewThreadMap = new mapQThreadPoolT();
				nRetCode = _GetNewThreadInfoFromDB(strQName, strExistSubID, mapSubCnt, pNewThreadMap);
				/*if ( Ret_Success == nRetCode)*/
				rNewThreadPool.insert(mapAllQThreadPoolValueT(strQName, pNewThreadMap));
			}
		}
		else
		{
			strQName = iterSubCnt->first;
			//create queue and its thread map
			mapQThreadPoolT* pNewThreadMap = new mapQThreadPoolT();			
			//get recipient info from s table
			nRetCode = _GetNewThreadInfoFromDB(strQName, mapSubCnt, pNewThreadMap);
			/*if ( Ret_Success == nRetCode)*/
			rNewThreadPool.insert(mapAllQThreadPoolValueT(strQName, pNewThreadMap));

		}
		iterSubCnt++;
	}


	return nRetCode;
}

void  CConfigInfoHelper::_InitLogVariables(pg_config_parameter pConfigPara)
{
	DebugUtil::getInstance().setLevel( static_cast<DebugUtil::EDebugLevel>(pConfigPara->m_LogLevel) );
	
	//RPARAM_DEBUGPIDFILENAMES   DebugPidFilenames=0 log file will not contain PID
	//RPARAM_DEBUGPIDFILENAMES   DebugPidFilenames=1 log file will contain PID
	//set RPARAM_DEBUGPIDFILENAMES first
	std::string	 strDebugPidFilenames  = pConfigPara->m_DebugPidFilenames;
	DebugUtil::getInstance().encryptPidInFilenames(strDebugPidFilenames);

	DebugUtil::getInstance().setFile( pConfigPara->m_LogFileName );
	DebugUtil::getInstance().setMaxSize(pConfigPara->m_LogFileSize);
	DebugUtil::getInstance().setMaxFiles(pConfigPara->m_LogFileCount);
	
}

int  CConfigInfoHelper::_ParseLogLevel(pg_config_parameter pConfigPara, const std::string& strValue)
{
	int  nRet = Ret_Success;
	DebugLevelNameValue* nv = nameValueMapping;

	while (nv->name != NULL)
	{
		if (!CUtilityFun::stricmp_(strValue.c_str(), nv->name))
		{
			pConfigPara->m_LogLevel = nv->value;
			break;
		}
		nv++;
	}

	return nRet;
}

int CConfigInfoHelper::_LoadConfigFile(vecLineT &vecLines)
{
	int nRet = Ret_Success;

	std::ifstream inFile(m_strFileName.c_str());
	if (inFile.fail())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s, File name: %s", Err_File_NotExist, defErrMsg(Err_File_NotExist), m_strFileName.c_str());
		nRet = Ret_Failed;
		//TA_THROW(FileNotExist(Err_File_NotExist, defErrMsg(Err_File_NotExist)));
		return nRet;
	}

	char inBuf[defMaxLogSize] = {0};
	// Read in all lines and add them to a vector
	while ( inFile.getline ( inBuf, defMaxLogSize ) )
	{
		char * crPtr = strchr( inBuf, '\r' );
		if( 0 != crPtr )
		{
			*crPtr = '\0';
		}
		std::string line = std::string( inBuf );
		vecLines.push_back( line );
	}

	// close file
	inFile.close();

	nRet = _FillMapData(vecLines);
	if ( Ret_Failed == nRet )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_NoData_inFile, defErrMsg(Err_NoData_inFile));
		return nRet;
	}

	return nRet;

}

int  CConfigInfoHelper::_FillCfgData(pg_config_parameter pConfigPara)
{
	int nRet = Ret_Success;
	DEF_ERROR_CHECK(pConfigPara);

	std::string strValue;
	mapConfigDataPairIterT iter;
	
	iter = m_mapValuePair.find( defConfig_MsgCompress );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = iter->second;
		pConfigPara->m_MsgCompress = atoi( strValue.c_str() );
	}
	
	iter = m_mapValuePair.find( defConfig_PackSize );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = trim(iter->second);
		//size_t nPos = 0;
		std::string strPagSize;
		//nPos = strValue.find("K");
		if (std::string::npos != strValue.find("K"))
		{
			strPagSize = strValue.substr(0, strValue.find("K"));
			pConfigPara->m_PackageSize = atoi( strPagSize.c_str() ) * defBytesKMask;
		}
	}

	iter = m_mapValuePair.find( defConfig_BackSlashSwitch );
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nBackSlashSwitch = atoi( strValue.c_str() );
	}

	iter = m_mapValuePair.find( defConfig_CharSet );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = (iter->second);
		memset(pConfigPara->m_szCharSet, 0, defMaxNameLen);
		memcpy(pConfigPara->m_szCharSet, strValue.c_str(), strValue.size() );
	}

	iter = m_mapValuePair.find( defConfig_SrcDbConStr );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = (iter->second);
		memcpy(pConfigPara->m_SrcDbConStr, strValue.c_str(), strValue.size());
		
		std::string strDbConSrc;
		std::string strDbCon;
		strDbConSrc = pConfigPara->m_SrcDbConStr;
		strDbCon = CUtilityFun::strtolwr((char *)strDbConSrc.c_str());
		if (std::string::npos != strDbCon.find(defDefault_Config_DbMySQL))
		{
			MAQ_CON_OPT conOpt;
			conOpt.m_nConSlashSwitch = pConfigPara->m_nBackSlashSwitch;
			conOpt.m_strCharSet      = pConfigPara->m_szCharSet;
			if (TA_Base_Core::RunParams::getInstance().isSet(defCmdLine_CharSet))
			{
				conOpt.m_strCharSet  = TA_Base_Core::RunParams::getInstance().get(defCmdLine_CharSet);
			}				
			pConfigPara->m_nDBType = enumMysqlDb;
			CMysqlEnv::getInstance().setMysqlConOpt(&conOpt);
		}
		else if (std::string::npos != strDbCon.find(defDefault_Config_DbOracle))
		{
			pConfigPara->m_nDBType = enumOracleDb;
		}
		m_pDatabase->setDbConStr(pConfigPara->m_SrcDbConStr);
	}
	
	iter = m_mapValuePair.find( defConfig_SkipErrors );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = (iter->second);
		memcpy(pConfigPara->m_SkipErrors, strValue.c_str(), strValue.size() );
	}
	
	iter = m_mapValuePair.find( defConfig_DebugPidFilenames );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = (iter->second);
		memset(pConfigPara->m_DebugPidFilenames, 0, defConfig_OneLineMaxLen);
		memcpy(pConfigPara->m_DebugPidFilenames, strValue.c_str(), strValue.size() );
	}

	iter = m_mapValuePair.find( defConfig_CenterDBName );
	if ( iter != m_mapValuePair.end() )
	{
		strValue = (iter->second);
		memcpy(pConfigPara->m_CenterDBName, strValue.c_str(), strValue.size() );
	}
	iter = m_mapValuePair.find(defConfig_LogLevel);
	if ( iter != m_mapValuePair.end() )
	{
		strValue = iter->second;
		_ParseLogLevel(pConfigPara, strValue);
	}
	iter = m_mapValuePair.find(defConfig_LogFileSize);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_LogFileSize = atoi(strValue.c_str());
	}
	iter = m_mapValuePair.find(defConfig_LogFileCount);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_LogFileCount = atoi(strValue.c_str());
	}
	
	iter = m_mapValuePair.find(defConfig_LogFileName);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		//m_LogFileName size is	defMaxFileLen
		//before memcpy must check value len
		if (strValue.size() < defMaxFileLen - 1)
		{
			memset(pConfigPara->m_LogFileName, 0, defMaxFileLen);
			sprintf(pConfigPara->m_LogFileName, "%s", strValue.c_str());
		}
		else
		{
			memset(pConfigPara->m_LogFileName, 0, defMaxFileLen);
			sprintf(pConfigPara->m_LogFileName, "%s", defDefault_LogFileName);
		}				
	}	

	_InitLogVariables(pConfigPara);
	
	//add	
	iter = m_mapValuePair.find(defConfig_ServerPort);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		if (strValue.size())
		{
			memset(pConfigPara->m_szPort, 0, defServerPortLen);
			memcpy(pConfigPara->m_szPort, strValue.c_str(), strValue.size());
		}		
	}
	else
	{
		std::string strHostName;
		nRet = _GetHostName( strHostName );
		nRet = _GetServerPortFromDB(strHostName, pConfigPara);
	}
	//add	

	iter = m_mapValuePair.find(defConfig_RunType);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		if (strValue.size())
		{
			if (!CUtilityFun::stricmp_(strValue.c_str(), defConfig_RunType_Client))
			{
				pConfigPara->m_nRunType = RunType_Client;
			}
			else if (!CUtilityFun::stricmp_(strValue.c_str(), defConfig_RunType_Srv))
			{
				pConfigPara->m_nRunType = RunType_Server;
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"CConfigInfoHelper::_FillCfgData()[run type config set error: %s]",
					strValue.c_str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
				"CConfigInfoHelper::_FillCfgData()[run type config not set, will run as default setting as client]");
		}
	}
	else
	{
		pConfigPara->m_nRunType = pConfigPara->m_nDBType;
	}

	iter = m_mapValuePair.find(defConFailMonSwitch);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nConFailedMonSwitch = atoi(strValue.c_str());
	}

	iter = m_mapValuePair.find(defHeartBeatSwitch);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nHeartBeatSwitch = atoi(strValue.c_str());
	}

	iter = m_mapValuePair.find(defHeartBeatInterval);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		// the input unit from config is seconds
		pConfigPara->m_nHeartBeatInterval = atoi(strValue.c_str()) * TIME_BASE_SECOND;
	}

	iter = m_mapValuePair.find(defHeartBeatTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		// the input unit from config is seconds
		pConfigPara->m_nHeartBeatTimeOut = atoi(strValue.c_str()) * TIME_BASE_SECOND;
	}
	
	//add defConfig_EnqTimeOut
	iter = m_mapValuePair.find(defConfig_EnqTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nEnqTimeOut = atoi(strValue.c_str());
	}	
	
	//add defConfig_DeqTimeOut
	iter = m_mapValuePair.find(defConfig_DeqTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nDeqTimeOut = atoi(strValue.c_str());
	}
		
	//add defConfig_DeqGetACKTimeOut
	iter = m_mapValuePair.find(defConfig_DeqGetACKTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nDeqGetACKTimeOut = atoi(strValue.c_str());
	}
		
	//add defConfig_HandSharkTimeOut
	iter = m_mapValuePair.find(defConfig_HandSharkTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nHandShareTimeOut = atoi(strValue.c_str());
	}
		
	//add defConfig_ReleaseActorTimeOut
	iter = m_mapValuePair.find(defConfig_ReleaseActorTimeOut);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nReleaseActorTimeOut = atoi(strValue.c_str());
	}
	
	
	
	iter = m_mapValuePair.find(defConfig_EnqBatchSize);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nEnqBatchSize = atoi(strValue.c_str());
	}
	
	//add defConfig_ReleaseActorTimeOut	
	iter = m_mapValuePair.find(defConfig_DeqFailOverTime);
	if (iter != m_mapValuePair.end())
	{
		strValue = iter->second;
		pConfigPara->m_nDeqFailOverTime = atoi(strValue.c_str());
		
		if (pConfigPara->m_nDeqFailOverTime < TIME_SECOND_FIVE)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "DeqFailOverTime must bigger than 5 seconds");
 			pConfigPara->m_nDeqFailOverTime = TIME_SECOND_FOUR;			
		}
		pConfigPara->m_nDeqUpdateTokenTime = pConfigPara->m_nDeqFailOverTime / 2;

	}
	
	m_mapValuePair.clear();

	return nRet;
}


int  CConfigInfoHelper::_GetServerPortFromDB(const std::string& strHostName, pg_config_parameter pConfigPara)
{
	int  nRet = Ret_Success;
	int  nRowNums = 0;
	IDbData*  pData = NULL;

	DEF_ERROR_CHECK(m_pDatabase);

	//std::string strSql   = m_pDatabase->prepareSQLStatement(DB_GET_SERVER_PORT_5002, strHostName);
	std::string strSql;
	m_pDatabase->prepareSQLStatement(strSql, DB_GET_SERVER_PORT_5002);
	
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
		"CConfigInfoHelper::_GetServerPortFromDB()[DB_GET_SERVER_PORT_5002:strSql=%s]",
		strSql.c_str());
	/*	
	select address from recipient_info  where upper(name) = upper(database()) AND upper(hostname)=upper('%s')
	*/
	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("ADDRESS");


	try
	{
		m_pDatabase->connect();
		pData = m_pDatabase->executeQuery(strSql, columnNames);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get MysqlServerPort error! [strSql=%s]", 
			strSql.c_str());
		DEF_DELETE(pData);	
		nRet = Ret_Failed;
		return nRet;
	}
	
	nRowNums = 	pData->getNumRows();
	if (1 > nRowNums)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get MysqlServerPort error! [strSql=%s][nRowNums=%d]", 
			strSql.c_str(), nRowNums);
		DEF_DELETE(pData);	
		nRet = Ret_Failed;
		return nRet;
	}
	

	std::string strAddr = pData->getStringData(0, columnNames[0]);
	std::string::size_type nPosTmp = strAddr.find_first_of(defConfig_srcIpDelimitEx);

	if (std::string::npos != nPosTmp)
	{
		strAddr = strAddr.substr(0, nPosTmp);
	}
		
	std::string::size_type nPos = strAddr.find_first_of(defConfig_DbConStrTag);
	if ( std::string::npos == nPos )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get MysqlServerPort error![strAddr=%s]", strAddr.c_str());
		nRet = Ret_Failed;
		return nRet;	
	}
	std::string strSubTemp(strAddr.substr(0, nPos));
	std::string strIp     = trim(strSubTemp);
	strSubTemp =  strAddr.substr(nPos + 1);
	std::string strPort   = trim(strSubTemp);	

	memcpy(pConfigPara->m_szPort, strPort.c_str(), strPort.size());
	pConfigPara->m_LocalAgent.setAddress(strAddr);

	DEF_DELETE(pData);

	return nRet;
}

int  CConfigInfoHelper::_GetAgentNameFromDB(pg_config_parameter pConfigPara)
{
	int  nRet = Ret_Success;
	int  nRowNums = 0;
	IDbData*  pData = NULL;

	DEF_ERROR_CHECK(m_pDatabase);

	std::string strSql;
	m_pDatabase->prepareSQLStatement(strSql, ORACLESYSTABLE_SELECT_5000);
	
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
		"CConfigInfoHelper::_GetAgentNameFromDB()[ORACLESYSTABLE_SELECT_5000:strSql=%s]",
		strSql.c_str());
	/*	
	SELECT DATABASE() AS NAME
	*/
	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("NAME");

	try
	{
		m_pDatabase->connect();
		pData = m_pDatabase->executeQuery(strSql, columnNames);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get Agent name error! [strSql=%s]", 
			strSql.c_str()); 
		DEF_DELETE(pData);	
		nRet = Ret_Failed;
		return nRet;
	}
	
	nRowNums = 	pData->getNumRows();
	if (1 > nRowNums)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"error! get Agent name error! [strSql=%s][nRowNums=%d]", 
			strSql.c_str(), nRowNums);
		DEF_DELETE(pData);	
		nRet = Ret_Failed;
		return nRet;
	}		

	std::string strAgentName = pData->getStringData(0, columnNames[0]);	
	pConfigPara->m_LocalAgent.setName(strAgentName);

	DEF_DELETE(pData);

	return nRet;
}



int  CConfigInfoHelper::_GetAQSysQueueManagerInfoFromDB()
{
	int  nRet = Ret_Success;
	if (NULL == m_pDatabase)
	{
		nRet = Ret_Failed;
		return nRet;
	}
	
	nRet = CTcpConnectionPool::getInstance()->initMapQNameAQManager(m_pDatabase);
	if (0 != nRet)
	{
		nRet = Ret_Failed;
	}
	
	return nRet;
}

int  CConfigInfoHelper::_FillMapData(const vecLineT& vecLines)
{
	int nRet = Ret_Success;
	vecLinecIterT iterAllLine = vecLines.begin();
	m_mapValuePair.clear(); // before fill data, clean it first

	while ( iterAllLine != vecLines.end() )
	{
		std::string strLine(*iterAllLine);
		strLine = trim(strLine);

		// comment line, ignore it.
		if ( (!strLine.empty()) && strLine[0] == ';')  
		{
			iterAllLine++;
			continue;
		}

		if ( (!strLine.empty()) &&
			(strLine.find("[") == std::string::npos) &&
			(strLine.find("]") == std::string::npos) )
		{
			size_t uPos = strLine.find_first_of("=");
			std::string strTag     = strLine.substr(0, uPos);
			std::string strValue   = strLine.substr(uPos + 1);
			strTag   = trim( strTag );
			strValue = trim( strValue );
			strTag   = CUtilityFun::strtolwr((char*)strTag.c_str());

			m_mapValuePair.insert(mapConfigDataPairValueT(strTag, strValue));
			
		}
		// next line
		iterAllLine++;
	}

	nRet = (m_mapValuePair.size() > 0  ? Ret_Success : Ret_Failed );

	return nRet;

}


int  CConfigInfoHelper::_GetHostName(std::string& strHostName)
{
   int nFunRes = 0;	 

   const int maxNameSize = 1025;
   char unqualifiedHostName[ maxNameSize ];
   int lvError = gethostname( unqualifiedHostName, sizeof( unqualifiedHostName ) );
#if defined( WIN32 )
   if( WSANOTINITIALISED == WSAGetLastError() )
   {
	   //
	   // The Windows sockets library is not smart enough to figure out
	   // that it should be automatically started, therefore we must start
	   // Windows sockets and then try again.
	   //
	   WORD wVersionRequested;
	   WSADATA wsaData;
	   int err;
	   wVersionRequested = MAKEWORD( 2, 2 );
	   err = WSAStartup( wVersionRequested, &wsaData );
	   if( 0 != err )
	   {
		   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			   "Error Code: %d, Error message: %s", Err_Failed_GetHostName, defErrMsg(Err_Failed_GetHostName));
	   }

	   // Try again.
	   lvError = gethostname( unqualifiedHostName, sizeof( unqualifiedHostName ) );
   }
#endif // defined( WIN32 )
   if( 0 != lvError )
   {
	   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
		   "Error Code: %d, Error message: %s", Err_Failed_GetHostName, defErrMsg(Err_Failed_GetHostName));
   }
    strHostName = unqualifiedHostName;  
   
   return nFunRes; 
}


int  CConfigInfoHelper::_GetQSubCntFromDB(mapQSubCntT& rMapSubCnt)
{
	int nRetCode = Ret_Success;
	int  nRowNums = 0;

	DEF_ERROR_CHECK(m_pDatabase);

	std::string strSql;
	m_pDatabase->prepareSQLStatement(strSql, AQSYSMTABLE_SELECT_4050);
	
	/*	
	SELECT Q_NAME, SUB_COUNT, PAYLOAD_TYPE FROM aq_sys_queue_m
	*/
	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("Q_NAME");
	columnNames.push_back("SUB_COUNT");
	columnNames.push_back("PAYLOAD_TYPE");

	IDbData*  pData = NULL;

	try
	{	
		m_pDatabase->connect();
		pData = m_pDatabase->executeQuery(strSql, columnNames);
		
		do
		{
			for (unsigned long i = 0; i < pData->getNumRows(); i++)
			{
				try
				{
					std::string strQName  = pData->getStringData(i, columnNames[0]);				
					uint32 uSubCnt        = pData->getIntegerData(i, columnNames[1]);
					int nPayloadType      = pData->getIntegerData(i, columnNames[2]);

					AQMINFO aqInfo;
					aqInfo.uiSubCnt = uSubCnt;
					aqInfo.nPayloadType = nPayloadType;

					rMapSubCnt.insert(mapQSubCntValueT(strQName, aqInfo));

				}
				catch (TA_Base_Core::DataException& e)
				{
					DEF_DELETE(pData);
					throw e;
				}
			}
			DEF_DELETE(pData);
		}while ( m_pDatabase->moreData(pData) );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
			"The queue count in the AQ system is: %u", rMapSubCnt.size() );

		nRetCode = (rMapSubCnt.size() ? Ret_Success : Ret_Failed);
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		if (ER_MYSQL_GONE_AWAY == e.getNativeCode())
			_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}	
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

// 1,2,3,4
void  CConfigInfoHelper::_GetExistSubIDs(mapQThreadPoolT* pWorkMap, std::string& strExistSubID)
{
	mapQThreadPoolIterT iter = pWorkMap->begin();
	while (iter != pWorkMap->end())
	{
		strExistSubID += CUtilityFun::int2str(iter->first);
		iter++;
		if ( iter != pWorkMap->end())
		{
			strExistSubID += ",";
		}
	}
}

//
int  CConfigInfoHelper::_GetNewThreadInfoFromDB(const std::string& strQName, mapQSubCntT& rQsubMap, mapQThreadPoolT*& rPMapPool)
{
	int nRetCode = Ret_Success;	

	DEF_ERROR_CHECK(m_pDatabase);

	std::string strTable = ("aq_" + strQName + "_t_s");

	std::string strSql;
	m_pDatabase->prepareSQLStatement(strSql, AQSYSSTABLE_SELECT_4051, strTable);
	
	/*	
	SELECT SUB_ID, Q_NAME, SUB_NAME, SUB_ADDRESS FROM %s
	*/
	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("SUB_ID");
	columnNames.push_back("SUB_NAME");
	columnNames.push_back("SUB_ADDRESS");

	mapQSubCntIterT iterFound = rQsubMap.find(strQName);
	int nPayloadType = iterFound->second.nPayloadType;

	IDbData*  pData = NULL;

	try
	{
		m_pDatabase->connect();
		pData = m_pDatabase->executeQuery(strSql, columnNames);
		
		do
		{
			for (unsigned long i = 0; i < pData->getNumRows(); i++)
			{
				try
				{
					uint32 uiSubID         = pData->getIntegerData(i, columnNames[0]);
					std::string strSubName = pData->getStringData(i, columnNames[1]);
					std::string strSubAddr = pData->getStringData(i, columnNames[2]);

					PRO_THD* pThd = new PRO_THD();
					memset(pThd->m_szQName, 0, defMaxNameLen);

					pThd->m_nPayloadType = nPayloadType;
					pThd->m_nSubID = uiSubID;				
					memcpy(pThd->m_szQName, strQName.c_str(), strQName.size());
					_ParseRcvAddr(pThd, strSubName, strSubAddr);					
					pThd->m_Sender = g_system_variables.m_LocalAgent;

					rPMapPool->insert(mapQThreadPoolValueT(uiSubID, pThd));

				}
				catch (TA_Base_Core::DataException& e)
				{
					DEF_DELETE(pData);
					throw e;
				}
			}
			DEF_DELETE(pData);
		}while ( m_pDatabase->moreData(pData) );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
			"The sub count in the queue: %u", rPMapPool->size() );

		/*
		if no data in table aq_AUDIT_DATA_QUEUE_t_s return	Ret_Failed=0+1
		*/
		nRetCode = (rPMapPool->size() ? Ret_Success : Ret_Failed);
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		if (ER_MYSQL_GONE_AWAY == e.getNativeCode())
			_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}	
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}

	return nRetCode;
}

int  CConfigInfoHelper::_GetNewThreadInfoFromDB(const std::string& strQName, std::string& strExistSubID, 
												 mapQSubCntT& rQsubMap, mapQThreadPoolT*& rPMapPool)
{
	int nRetCode = Ret_Success;

	DEF_ERROR_CHECK(m_pDatabase);
	std::string strTable = ("aq_" + strQName + "_t_s");

	if (strExistSubID.empty())
	{
		strExistSubID = "0";
	}

	std::string strSql;
	m_pDatabase->prepareSQLStatement(strSql, AQSYSSTABLE_SELECT_4052, strTable, strExistSubID);
	
	/*	
	SELECT SUB_ID, SUB_NAME, SUB_ADDRESS FROM %s WHERE SUB_ID NOT IN ( %s )
	*/
	// Set up the columnNames vector to be passed to executeQuery()
	std::vector<std::string> columnNames;
	columnNames.push_back("SUB_ID");
	columnNames.push_back("SUB_NAME");
	columnNames.push_back("SUB_ADDRESS");

	mapQSubCntIterT iterFound = rQsubMap.find(strQName);
	int nPayloadType = iterFound->second.nPayloadType;

	IDbData*  pData = NULL;

	try
	{
		m_pDatabase->connect();
		pData = m_pDatabase->executeQuery(strSql, columnNames);
		
		do
		{
			for (unsigned long i = 0; i < pData->getNumRows(); i++)
			{
				try
				{
					uint32 uiSubID         = pData->getIntegerData(i, columnNames[0]);
					std::string strSubName = pData->getStringData(i, columnNames[1]);
					std::string strSubAddr = pData->getStringData(i, columnNames[2]);

					PRO_THD* pThd = new PRO_THD();
					memset(pThd->m_szQName, 0, defMaxNameLen);

					pThd->m_nPayloadType = nPayloadType;
					pThd->m_nSubID = uiSubID;				
					memcpy(pThd->m_szQName, strQName.c_str(), defMaxNameLen -1);
					_ParseRcvAddr(pThd, strSubName, strSubAddr);					
					pThd->m_Sender = g_system_variables.m_LocalAgent;

					rPMapPool->insert(mapQThreadPoolValueT(uiSubID, pThd));

				}
				catch (TA_Base_Core::DataException& e)
				{
					DEF_DELETE(pData);
					throw e;
				}
			}
			DEF_DELETE(pData);
		}while ( m_pDatabase->moreData(pData) );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
			"The sub count in the queue: %u", rPMapPool->size() );

		nRetCode = (rPMapPool->size() ? Ret_Success : Ret_Failed);
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
		if (ER_MYSQL_GONE_AWAY == e.getNativeCode())
			_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}	
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		_InitDB();

		DEF_DELETE(pData);
		nRetCode = Ret_Failed;
	}

	return nRetCode;

}

void  CConfigInfoHelper::_InitDB()
{
	DEF_DELETE(m_pDatabase);
	m_pDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
}

void  CConfigInfoHelper::_ParseRcvAddr(PRO_THD*& pThd, const std::string& strSubName, const std::string& strAddr)
{
	if (strAddr.empty() || strSubName.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Server_IP_Addr_Not_Found, defErrMsg(Err_Server_IP_Addr_Not_Found));
		return;
	}
	
	CAgent* pAgent = NULL;
	std::string strSrvAddr(strAddr);
	std::string::size_type nPos = strAddr.find_first_of(defConfig_srcIpDelimitEx);
	while (std::string::npos != nPos)
	{
		std::string strSubTemp(strSrvAddr.substr(0, nPos));
		strSubTemp     = trim(strSubTemp);
		strSrvAddr     = strSrvAddr.substr(nPos + 1);	
		pAgent = new CAgent(strSubName, strSubTemp);
		pThd->m_lstRecver.push_back(pAgent);

		nPos = strSrvAddr.find_first_of(defConfig_srcIpDelimitEx);
	}

	pAgent = new CAgent(strSubName, strSrvAddr);
	pThd->m_lstRecver.push_back(pAgent);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Subscriber: %s, Address: %s, Receiver list size: %d", strSubName.c_str(), strAddr.c_str(), pThd->m_lstRecver.size());
}


// define the only one global config system variables
g_config_parameter g_system_variables;
// define the global system state code flags
g_sys_code_flags g_sys_status_mask;


NS_END(TA_DB_Sync)



