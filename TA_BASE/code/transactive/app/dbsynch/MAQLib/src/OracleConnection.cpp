#include "OracleConnection.h"

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4996 )
#endif // defined( WIN32 )

#include <algorithm> 
#include "Message.h"		   
#include "IDbData.h"
#include "UtilityFun.h"
#include "DBException.h"
#include "OracleQueryAdapter.h"

#include "COracleEnv.h"	

#if defined(_WIN32)
#include <Windows.h>
#define aq_sleep     Sleep
#else
#include <unistd.h>
#define aq_sleep     sleep
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_AQ_Lib)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static const std::string	s_strKeyMessageQualifier = "MESSAGE_QUALIFIER";
static const std::string	s_strKeyAuditDataOracle = "AUDIT_DATA_ORACLE";
static const std::string	s_strKeyAuditDataMySQL = "AUDIT_DATA_MYSQL";


static const std::string	s_strENQSQLFormat("begin prc_enq_audit_data('%s','%s','%s');end;");

static const std::string	s_strGetDataTypeSQLFormat("select t.data_type from user_tab_columns t where \
t.TABLE_NAME = (select queue_table from user_queues where name = upper('%s')) and t.COLUMN_NAME='USER_DATA'");

static const std::string	s_strSQLPrcEnqBindSQLFormat("begin prc_enq_audit_data(:MessageQualifierValue, :AuditDataOracleValue, :AuditDataMySQLValue);end;");

static const std::string	s_strBindMessageQualifier(":MessageQualifierValue");
static const std::string	s_strBindKeyAuditDataOracle(":AuditDataOracleValue");
static const std::string	s_strBindKeyAuditDataMySQL(":AuditDataMySQLValue");


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

std::string COracleConnection::_OCIMsgGetIDEx(OCI_Msg *msg, unsigned int *len)
{
	std::string strRetID;
	u_char  szMsgID[16] = {0};

	if (NULL == msg || NULL == len)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"_OCIMsgGetIDEx() NULL == msg || NULL == len");
		return strRetID;
	}

	OCI_MsgGetID(msg, szMsgID, len);

	for (size_t i = 0; i < 16; i++)
	{
		strRetID += CUtilityFun::sysDecToHex(szMsgID[i]);
	}

	*len = strRetID.size();

	return strRetID;
}

int COracleConnection::_GetDbConnectionStr(
	const std::string& strDbName, 
	const std::string& strUserName, 
	const std::string& strPwd, 
	const std::string& strHost,
	std::string &strDbConnectionStr)
{
	//SrcDbConStr = Oracle:TRA_OCC:IT271350_5:IT271350_5
	int nFunRes = 0;

	strDbConnectionStr.clear();

	strDbConnectionStr = "Oracle:";
	strDbConnectionStr += strDbName;
	strDbConnectionStr += ":";
	strDbConnectionStr += strUserName;
	strDbConnectionStr += ":";
	strDbConnectionStr += strPwd;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"[strDbConnectionStr=%s]", strDbConnectionStr.c_str());

	return nFunRes;
}

int COracleConnection::_CreateConnection()
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	bool  bContinueTryCreate = true;
	std::string strOCIError;
	std::string strSchema(m_strSchema);
	std::string	strUserName(m_strUserName);
	std::string	strPwd(m_strPwd);
	int nTryIndex = 0;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"_CreateConnection() [DbConStr=%s][CloseSet=%d]", m_strDbConStr.c_str(), m_nCloseSet);


	if (NULL != m_connection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "FAILOVERDBLOG: connection closed in _CreateConnection() function.");
		
		if (NULL != m_pStatement)
		{
			OCI_StatementFree(m_pStatement);
			m_pStatement = NULL;
		}
		
		/* disconnect from DB */
		nOCIFunRes = OCI_ConnectionFree(m_connection);
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OCI_ConnectionFree() error! [nOCIFunRes=%d]", nOCIFunRes);
			nFunRes = -1;
			return nFunRes;	
		}
		m_connection = NULL; 
	}	

	//create one oracle db connection
	bContinueTryCreate = true;
	nTryIndex = 0;					 
	while (bContinueTryCreate && 0 == m_nCloseSet)
	{
		nTryIndex++;
		if (NULL == m_connection)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[nTryIndex=%d]OCI_ConnectionCreate begin [strSchema=%s][strUserName=%s][strPwd=%s] OCI_SESSION_DEFAULT", 
				nTryIndex,
				strSchema.c_str(), 
				strUserName.c_str(),
				strPwd.c_str());	

			m_connection = OCI_ConnectionCreate(
				strSchema.c_str(), 
				strUserName.c_str(),
				strPwd.c_str(), 
				OCI_SESSION_DEFAULT);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[nTryIndex=%d]OCI_ConnectionCreate end [strSchema=%s][strUserName=%s][strPwd=%s] OCI_SESSION_DEFAULT", 
				nTryIndex,
				strSchema.c_str(), 
				strUserName.c_str(),
				strPwd.c_str());	

		}

		//check ConnectionCreate res
		if (NULL == m_connection)
		{
			//error code eg.ER_ORACLE_NotAvailable
			strOCIError = "OCI_ConnectionCreate() error!";	
			_LOGOracleErrMsg(strOCIError, 1);	
			nFunRes = -1;
			bContinueTryCreate = true;
#if defined(_WIN32)
			aq_sleep(10000);
#else
			aq_sleep(10);
#endif
		}
		else
		{
			//set bCreateConnectionRes true to break while()
			nFunRes = 0;
			bContinueTryCreate = false;
		}
		
		if (nTryIndex >= MAX_RETRY_TIMES)
		{
			nFunRes = -1;
			bContinueTryCreate = false;
			CDBException db("ORACLE_Connection_ERROR", "unknown database exception", 0);
			throw db;		
		}
			
	}//while

	return nFunRes;

}

int COracleConnection::_LOGOracleErrMsg(std::string strLogInfo, int nWriteLog)
{
	int			nErrorCode = 0;
	std::string strError;

	_GetOracleErrMsg(strError, nErrorCode);	

	if (1 == nWriteLog)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"[strLogInfo=%s][nErrorCode=%d][strError=%s]", 
			strLogInfo.c_str(), nErrorCode, strError.c_str());
	} 	

	return nErrorCode;
}

int COracleConnection::_GetOracleErrMsg(std::string &strError, int &nError)
{
	int			nFunRes = 0; 
	char*		pstrErrorBuf = NULL;
	int			nErrorCode = 0;
	std::string strOCIError;

	//if Oracle connection not created
	if (NULL == m_connection)
	{
		strOCIError = OCI_Get_G_ErrorBuff();
		nErrorCode = 0;

		pstrErrorBuf = NULL;
		strError = strOCIError;
		nError = nErrorCode;

		return nFunRes;
	}

	//if Oracle connection  created
	if (NULL != m_connection)
	{
		pstrErrorBuf = NULL;	 
		nErrorCode = OCI_Get_ErrorStrBuffer(m_connection, &pstrErrorBuf);
	}

	if (NULL != pstrErrorBuf)
	{
		strOCIError = pstrErrorBuf;
	}
	else
	{
		strOCIError = OCI_Get_G_ErrorBuff();
		nErrorCode = 0;
	}

	DEF_DELETE_ARRAY(pstrErrorBuf);

	//eg. ORA-25207: enqueue failed, queue IT271350_5.AUDIT_DATA_QUEUE is disabled from enqueueing
	strError = strOCIError;
	nError = nErrorCode;

	return nFunRes;
} 


int COracleConnection::_CheckDBConnection()
{
	int nFunRes = 0;
	
	if (m_strDbConStr.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//check 	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"_CheckDBConnection false == isOpen()"); 

		//free AQ handle
		_OracleAQUnInit();
		
		//create db connection
		open();
		nFunRes = 1;
	}	
	else
	{
		nFunRes = 0;
	}			 	
	
	return nFunRes;
}


int	COracleConnection::_OracleAQUnInit()
{
	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
		"_OracleAQUnInit()[EnqSet=%d][EnqInit=%d][DeqSet=%d][DeqInit=%d]",
		m_bEnqSet, m_bEnqInit, m_bDeqSet, m_bDeqInit);
		
	int nFunRes = 0;
	if (true == m_bEnqSet)
	{
		if (true == m_bEnqInit)
		{
			_FreeMapColumNameType(m_mapAQDataTypeInfoColumn);
			_EnqUnInit();
			m_bEnqInit = false;
			m_strEnqQueueName.clear();
			m_strEnqAgentArray.clear();
			m_lstEnqAgentArray.clear();
			m_ppEnqAgentArray = NULL;
			m_nEnqAgentArraySize = 0;
			m_pEnqEnqueueHandle = NULL;
			m_pEngMsgHandle = NULL;
			m_pEnqObjectHandle = NULL;	

		}
	}

	if (true == m_bDeqSet)
	{
		if (true == m_bDeqInit)
		{
			_FreeMapColumNameType(m_mapAQDataTypeInfoColumn);
			_DeqUnInit();
			m_bDeqInit = false;
			m_pDeqDequeue = NULL;
			m_strDeqQueueName.clear();
			m_strDeqClientAgentName.clear();
			m_nDeqMode = OCI_ADM_REMOVE_NODATA;
			m_nDeqNav =	 OCI_ADN_FIRST_MSG;
			m_nDeqFetchSize = 0; //200kb  // the row of message for MySQL, the size of message for Oracle
			m_strDeqMsgID.clear();
			m_strDeqAgentArray.clear();
			m_lstDeqAgentArray.clear();
			m_ppDeqAgentArray = NULL;
			m_nDeqAgentArraySize = 0; 


		}
	}



	return nFunRes;
}


int COracleConnection::_CheckEnqMsgInputParam(CMessage* pEnqMsg)
{
	int nFunRes = 0;
			
	if (NULL == pEnqMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"_CheckEnqMsgInputParam() NULL == pEnqMsg");
		nFunRes = -1;
		return nFunRes;
	}

	else if (pEnqMsg->getQName().empty())//true == pEnqMsg->getQName()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"_CheckEnqMsgInputParam() true == pEnqMsg->getQName().empty()");
		nFunRes = -1;
		return nFunRes;
	}
	else if (pEnqMsg->getRecipientList().empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"_CheckEnqMsgInputParam() true == pEnqMsg->getRecipientList().empty()");
		nFunRes = -1;
		return nFunRes;
	}
	else
	{
		//check ok
		nFunRes = 0;
	
	}

	return nFunRes;
}

int COracleConnection::_GetEnqQName(CMessage* pEnqMsg, std::string& strQueueName)
{
	int nFunRes = 0;

	//check enqMsg input param
	if (0 != _CheckEnqMsgInputParam(pEnqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}	

	strQueueName =  pEnqMsg->getQName();	

	return nFunRes;
}

int COracleConnection::_GetStrEnqAgentArray(CMessage* pEnqMsg, std::string& strEnqAgentArray)
{
	int nFunRes = 0;

	//check enqMsg input param
	if (0 != _CheckEnqMsgInputParam(pEnqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}	

	//TT,TT;TT,TT;
	//TT,
	strEnqAgentArray =  pEnqMsg->getRecipientList();

	return nFunRes;
}

int COracleConnection::_GetAgtNameFromNameAddress(const std::string& strNameAddress, std::string& strAgtName)
{
	int nFunRes = 0;
	std::string::size_type nFind = std::string::npos;
	//Name,Address;

	if (strNameAddress.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	nFind = strNameAddress.find(",");
	strAgtName = strNameAddress.substr(0, nFind);
	
	return nFunRes;
}
int COracleConnection::_GetLstEnqAgentArray(const std::string& strRecipientLst, vecStrConT& lstEnqAgentArray)
{
	int nFunRes = 0;
	std::string strSubString;
	std::string::size_type nFind = std::string::npos;

	//Name,Address;Name,Address;
	if (strRecipientLst.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	strSubString = strRecipientLst;
	nFind = strSubString.find(";");

	while (nFind != std::string::npos)
	{
		std::string strOneAgtNameAddress;
		std::string strOneAgtName;
		strOneAgtNameAddress =  strSubString.substr(0, nFind);
		if (strSubString.length() > nFind + 1)
		{
			strSubString = strSubString.substr(nFind + 1);
		}
		else
		{
			strSubString.clear();
		}
		_GetAgtNameFromNameAddress(strOneAgtNameAddress, strOneAgtName);
		lstEnqAgentArray.push_back(strOneAgtName);
		
		nFind = std::string::npos;
		nFind = strSubString.find(";");
	}
	
	if (false == strSubString.empty() && strSubString.size() > 2)
	{
		std::string strOneAgtName;
		_GetAgtNameFromNameAddress(strSubString, strOneAgtName);

		lstEnqAgentArray.push_back(strOneAgtName);
	}
	   	
	return nFunRes;	
}


int COracleConnection::_CreateEnqAgentArray(const vecStrConT& lstEnqAgentArray)
{
	int nFunRes = 0;
	std::string strEachAgentName;
	
	if ( NULL != m_ppEnqAgentArray )
	{
		_FreeEnqAgentArray();
	}

	m_nEnqAgentArraySize = lstEnqAgentArray.size();
	m_ppEnqAgentArray = (OCI_Agent**)malloc(m_nEnqAgentArraySize * sizeof(OCI_Agent*));
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"m_ppEnqAgentArray malloc [EnqAgentArraySize=%d]", 
				m_nEnqAgentArraySize); 

	for (int nIndex = 0; nIndex < m_nEnqAgentArraySize; nIndex++)
	{
		m_ppEnqAgentArray[nIndex] = NULL;
	} 
	for (int nIndex = 0; nIndex < m_nEnqAgentArraySize; nIndex++)
	{
		strEachAgentName = m_lstEnqAgentArray[nIndex];
		m_ppEnqAgentArray[nIndex] = OCI_AgentCreate(m_connection, strEachAgentName.c_str(), m_strEnqQueueName.c_str());
		if (NULL == m_ppEnqAgentArray[nIndex])
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_AgentCreate()"); 
			nFunRes = -1;
			return nFunRes;
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"[strEachAgentName=%s]", strEachAgentName.c_str()); 
		}

	} 		

	return nFunRes;
}

int COracleConnection::_FreeEnqAgentArray()
{
	int nFunRes = 0;
	int nOCIFunRes = -1;

	if (NULL == m_ppEnqAgentArray)
	{
		nFunRes = 0;
		return nFunRes;
	}
	m_nEnqAgentArraySize = m_lstEnqAgentArray.size();
	for (int nIndex = 0; nIndex < m_nEnqAgentArraySize; nIndex++)
	{
		nOCIFunRes = OCI_AgentFree(m_ppEnqAgentArray[nIndex]);
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OCI_AgentFree");
			m_ppEnqAgentArray[nIndex] = NULL; 
		}

	} 
	
	if (NULL != m_ppEnqAgentArray)
	{
		free(m_ppEnqAgentArray);
		m_ppEnqAgentArray = NULL;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_ppEnqAgentArray free [EnqAgentArraySize=%d]", 
			m_nEnqAgentArraySize); 			
	}

	return nFunRes;
}

int COracleConnection::_GetAQDataTypeNameByAQQName(const std::string& strQueueName, std::string& strAQDataTypeName)
{
	int nFunRes = 0;
	OCI_Resultset* pResultset = NULL;
	int nOCIFunRes = 0;
	char* pszGetStr = NULL;

	if (false == isOpen())
	{
		open();
	}

	if (strQueueName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	if (NULL == m_pStatement)
	{
		m_pStatement = OCI_StatementCreate(m_connection);
	}

	if (NULL == m_pStatement)
	{
		nFunRes = -1;
		return nFunRes;
	}

	//pDataBase = new CDatabaseImpl(m_strDbConStr.c_str());
	//select t.data_type from user_tab_columns t where t.TABLE_NAME = 
	//(select queue_table from user_queues where name = upper('%s')) and t.COLUMN_NAME='USER_DATA'
	  	
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Prepare SQL");
	  
	sprintf(m_pBufferSQL, s_strGetDataTypeSQLFormat.c_str(), strQueueName.c_str());
 
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Prepare SQL");

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Execute SQL");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Execute [strSQL=%s]", m_pBufferSQL);

	nOCIFunRes = OCI_ExecuteStmt(m_pStatement, m_pBufferSQL);
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_ExecuteStmt()", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement); 
		m_pStatement = NULL;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"error! OCI_ExecuteStmt() statement=[%s]", m_pBufferSQL);
		nFunRes = -1;	
		return nFunRes;
	}

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Execute SQL");

	if (NULL != m_pStatement)
	{
		pResultset = OCI_GetResultset(m_pStatement);
	}
	
	nOCIFunRes = -1;
	if (NULL != pResultset)
	{
		nOCIFunRes = OCI_FetchNext(pResultset);
	}
	
	if (1 == nOCIFunRes)
	{
		pszGetStr = (char *)OCI_GetString(pResultset, 1);
	}

	//get data
	if (NULL != pszGetStr)
	{
		strAQDataTypeName = pszGetStr;
	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"error! _GetAQDataTypeNameByAQQName() [pszGetStr=NULL] [strSQL=%s]", m_pBufferSQL);	
	} 	
	
	if (NULL != m_pStatement)
	{
		nOCIFunRes = OCI_StatementReset(m_pStatement);
	}
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_StatementReset()", 1);
	}
	pResultset = NULL;
	pszGetStr = NULL; 	
	
	return nFunRes;
}




int COracleConnection::_GetLstAQDataTypeInfoColumn(const std::string& strAQDataTypeName, mapColumNTConT& mapAQDataTypeInfoColumn)
{
	int nFunRes = 0;
	int  nTypeInfoColumnCount = 0;
	OCI_Column* pEachColumn = NULL;
	char* pGetStr = NULL;

	if (true == strAQDataTypeName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	_FreeMapColumNameType(mapAQDataTypeInfoColumn);
			
	//"BOLB"--oracle RAW Type--
	//others--oracle object
	if ("BOLB" == strAQDataTypeName)
	{
		m_nInfoGetType =  OCI_TIF_TYPE;
		m_nPayLoadType =  PAYLOAD_TYPE_RAW;
	}
	else
	{
		m_nInfoGetType =  OCI_TIF_TYPE;
		m_nPayLoadType =  PAYLOAD_TYPE_OBJ;
	}

	m_pAQDataTypeInfo = OCI_TypeInfoGet(m_connection, strAQDataTypeName.c_str(), m_nInfoGetType);
	if (NULL == m_pAQDataTypeInfo)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error ! OCI_TypeInfoGet()  "); 
		nFunRes = -1;
		return nFunRes;
	} 

	nTypeInfoColumnCount = (int)OCI_TypeInfoGetColumnCount(m_pAQDataTypeInfo);

	for (int nIndex = 0; nIndex < nTypeInfoColumnCount; nIndex++)
	{
		pEachColumn =  OCI_TypeInfoGetColumn(m_pAQDataTypeInfo, nIndex + 1);
		pGetStr = NULL;
		pGetStr = (char *)OCI_ColumnGetName(pEachColumn);
		
		if (NULL != pGetStr)
		{	
			std::string strColumnName;
			strColumnName =	 pGetStr;
			PColumnNameType pColumnInfo= new ColumnNameType();
			pColumnInfo->m_strColumnName = strColumnName;
			pColumnInfo->m_nColumnType = OCI_ColumnGetType(pEachColumn);
			pColumnInfo->m_nColumnSubType = OCI_ColumnGetSubType(pEachColumn);
			pColumnInfo->m_nColumnSize = OCI_ColumnGetSize(pEachColumn);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
							"[ColumnName=%s][ColumnType=%d][ColumnSubType=%d][ColumnSize=%d]",
							pColumnInfo->m_strColumnName.c_str(), pColumnInfo->m_nColumnType,
							pColumnInfo->m_nColumnSubType, pColumnInfo->m_nColumnSize); 

			mapAQDataTypeInfoColumn.insert(mapColumNTConVT(strColumnName, pColumnInfo));
			pColumnInfo = NULL;
		}

	}

	m_nLstAQDataTypeInfoColumCount = mapAQDataTypeInfoColumn.size();   	
	return nFunRes;
}	



int	COracleConnection::_CreateEnqHandle(const std::string& strQueueName, OCI_TypeInfo* pAQDataTypeInfo)
{
	int nFunRes = 0;
	
	if (strQueueName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (NULL == pAQDataTypeInfo)
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	_FreeEnqHandle();

	m_pEnqObjectHandle = NULL;

	if (NULL == m_pEnqEnqueueHandle)
	{
		m_pEnqEnqueueHandle = OCI_EnqueueCreate(pAQDataTypeInfo, strQueueName.c_str());
		if (NULL == m_pEnqEnqueueHandle)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_EnqueueCreate()"); 
			nFunRes = -1;
			return nFunRes;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_pEnqEnqueueHandle OCI_EnqueueCreate");
	}
	

	if (NULL == m_pEngMsgHandle)
	{
		m_pEngMsgHandle = OCI_MsgCreate(pAQDataTypeInfo);
		if (NULL == m_pEngMsgHandle)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgCreate()"); 
			nFunRes = -1;
			return nFunRes;
		} 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_pEngMsgHandle OCI_MsgCreate");
	
	}
	

	return nFunRes;
}
int	COracleConnection::_FreeEnqHandle()
{
	int nFunRes = 0;
	int nOCIFunRes = 1;

	nFunRes = 0;
	nOCIFunRes = 1;


	if (NULL != m_pEngMsgHandle)
	{
		nOCIFunRes = OCI_MsgFree(m_pEngMsgHandle);
		m_pEngMsgHandle = NULL;
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgFree()");
			nFunRes = -1; 
		} 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_pEngMsgHandle OCI_MsgFree");

	} 	


	if (NULL != m_pEnqEnqueueHandle)
	{
		nOCIFunRes = OCI_EnqueueFree(m_pEnqEnqueueHandle);
		m_pEnqEnqueueHandle = NULL;
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_EnqueueFree()"); 
			nFunRes = -1;
		}

	}
	

	return nFunRes;
}

//************************************
// Method:    _CreateEnqObjetHandle
// FullName:  TA_AQ_Lib::COracleConnection::_CreateEnqObjetHandle
// Access:    private 
// Returns:   int
//		0
//		Err_EnqReqACkCode_PLTypeInvalid
//		Err_EnqReqACkCode_OCILibErr
//		Err_EnqReqACkCode_DataSrcErr
//      		
// Qualifier:
// Parameter: CMessage * pEnqMsg
//************************************
int	COracleConnection::_CreateEnqObjetHandle(CMessage* pEnqMsg)
{
	int				nFunRes = 0;
	int				nOCIFunRes = 0;
	std::string		strEachColumnName;
	int             nEachColumnType = 0;
	int				nGetRes = 0;
	int				nColumnCount = 0;
	mapColumNTConIterT iterMapColumn;
	ColumnNameType* pColumnNT = NULL;
	ulong64         nBigIntValue = 0;
	std::string		strPayLoadOneLine;
	const char*     pcPayload = NULL;
				 
						 	
	if (NULL == pEnqMsg)
	{
		nFunRes = Err_EnqReqACkCode_PLTypeInvalid;
		return nFunRes;
	}
	
	nColumnCount = m_mapAQDataTypeInfoColumn.size(); 
	if (nColumnCount != pEnqMsg->getSize())
	{
		nFunRes = Err_EnqReqACkCode_PLTypeInvalid;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"[mapAQDataTypeInfoColumn.size()=%d]!=[pEnqMsg->getSize()=%d]",
			m_mapAQDataTypeInfoColumn.size(), pEnqMsg->getSize()); 

		return nFunRes;
	}
	 
	//if enq have error, Object handle will not be free, reuse again 
	if (NULL == m_pEnqObjectHandle)
	{
		m_pEnqObjectHandle = OCI_ObjectCreate(m_connection, m_pAQDataTypeInfo);
		if (NULL == m_pEnqObjectHandle)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_ObjectCreate()"); 
			nOCIFunRes = -1;
			nFunRes = Err_EnqReqACkCode_OCILibErr;
			return nFunRes; 
		}  
	} 
	
	iterMapColumn = m_mapAQDataTypeInfoColumn.begin();
	while (iterMapColumn != m_mapAQDataTypeInfoColumn.end())
	{
		pColumnNT = iterMapColumn->second;
		nEachColumnType = pColumnNT->m_nColumnType;
		strEachColumnName = pColumnNT->m_strColumnName;
		
		switch (nEachColumnType)
		{
		case OCI_CDT_TEXT:      //  : dtext *:
			{
				nGetRes = pEnqMsg->getPayload(strEachColumnName, pcPayload);
				if (0 != nGetRes)
				{
					nGetRes = pEnqMsg->getPayload(strPayLoadOneLine);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"error! pEnqMsg->getPayload() [strEachColumnName=%s][nEachColumnType=%d][strEachColumnValue=%s]", 
						strEachColumnName.c_str(), nEachColumnType, pcPayload); 
					LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strPayLoadOneLine);		
					nFunRes = Err_EnqReqACkCode_DataSrcErr;//-9995;
					return nFunRes;
				} 				
				/* setup and enqueue message for consumer 1 */
				nOCIFunRes = OCI_ObjectSetString(m_pEnqObjectHandle, strEachColumnName.c_str(), pcPayload);
				LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, 
					"ObjectSetString ColumnName=%s, Value=%s", strEachColumnName.c_str(), pcPayload);				

				break;
			}
		case OCI_CDT_NUMERIC:   //  : short, int, long long, double
			{
				nGetRes = pEnqMsg->getPayload(strEachColumnName, nBigIntValue);
				if (0 != nGetRes)
				{
					nGetRes = pEnqMsg->getPayload(strPayLoadOneLine);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"error! pEnqMsg->getPayload() [strEachColumnName=%s][nEachColumnType=%d][nBigIntValue=%lu]", 
						strEachColumnName.c_str(), nEachColumnType, nBigIntValue); 
					LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strPayLoadOneLine);		
					nFunRes = Err_EnqReqACkCode_DataSrcErr;//-9995;
					return nFunRes;
				} 				
				/* setup and enqueue message for consumer 1 */
				nOCIFunRes = OCI_ObjectSetUnsignedBigInt(m_pEnqObjectHandle, strEachColumnName.c_str(), nBigIntValue);
				LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, 
					"ObjectSetString ColumnName=%s, Value=%d", strEachColumnName.c_str(), nBigIntValue);				

				break;
			}
		default:
			{	
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Enq UNKNOW DataType [strEachColumnName=%s][nEachColumnType=%d]", 
					strEachColumnName.c_str(), nEachColumnType);				
				break;
			}
		}//switch
		
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_ObjectSetString()"); 
			nFunRes = Err_EnqReqACkCode_OCILibErr;
			return nFunRes; 
		}	
		iterMapColumn++;
	}//while

	nOCIFunRes = OCI_MsgSetObject(m_pEngMsgHandle, m_pEnqObjectHandle);
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgSetObject()"); 
		nFunRes = Err_EnqReqACkCode_OCILibErr;
		return nFunRes; 
	} 
	
	nOCIFunRes = OCI_MsgSetConsumers(m_pEngMsgHandle, m_ppEnqAgentArray, m_nEnqAgentArraySize);
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgSetConsumers()"); 
		nFunRes = Err_EnqReqACkCode_OCILibErr;
		return nFunRes; 
	}
		
	return nFunRes;
}

int	COracleConnection::_FreeEnqObjetHandle()
{
	int nFunRes = 0;
	int nOCIFunRes = 0;

	nOCIFunRes = 1;
	if (NULL != m_pEnqObjectHandle)
	{
		nOCIFunRes = OCI_ObjectFree(m_pEnqObjectHandle);
		m_pEnqObjectHandle = NULL;
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_ObjectFree()"); 
			nFunRes = -1;
			return nFunRes;
		}
	}	

	return nFunRes;
}



int COracleConnection::_CreateDeqAgentArray()
{
	int nFunRes = 0;
	std::string strEachAgentName;

	if ( NULL != m_ppDeqAgentArray )
	{
		_FreeDeqAgentArray();
	}

	m_nDeqAgentArraySize = m_lstDeqAgentArray.size();
	m_ppDeqAgentArray = (OCI_Agent**)malloc(m_nDeqAgentArraySize * sizeof(OCI_Agent*));
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_ppDeqAgentArray malloc [DeqAgentArraySize=%d]",
			m_nDeqAgentArraySize); 

	for (int nIndex = 0; nIndex < m_nDeqAgentArraySize; nIndex++)
	{
		m_ppDeqAgentArray[nIndex] = NULL;
	} 		

	for (int nIndex = 0; nIndex < m_nDeqAgentArraySize; nIndex++)
	{
		strEachAgentName = m_lstDeqAgentArray[nIndex];
		m_ppDeqAgentArray[nIndex] = OCI_AgentCreate(m_connection, strEachAgentName.c_str(), m_strDeqQueueName.c_str());
		if (NULL == m_ppDeqAgentArray[nIndex])
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_AgentCreate()"); 
			nFunRes = -1;
			return nFunRes;
		}
	} 		

	return nFunRes;
}

int COracleConnection::_FreeDeqAgentArray()
{
	int nFunRes = 0;
	int nOCIFunRes = -1;  

	if (NULL == m_ppDeqAgentArray)
	{
		nFunRes = 0;
		return nFunRes;
	}
	m_nDeqAgentArraySize = m_lstDeqAgentArray.size();
	for (int nIndex = 0; nIndex < m_nDeqAgentArraySize; nIndex++)
	{
		nOCIFunRes = OCI_AgentFree(m_ppDeqAgentArray[nIndex]);
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OCI_AgentFree");
			m_ppDeqAgentArray[nIndex] = NULL; 
		}
	} 

	if (NULL != m_ppDeqAgentArray)
	{
		free(m_ppDeqAgentArray);
		m_ppDeqAgentArray = NULL;	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_ppDeqAgentArray free [DeqAgentArraySize=%d]",
			m_nDeqAgentArraySize);		
	}

	return nFunRes;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




int COracleConnection::_CheckDeqMsgInputParam( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg)
{
	int nFunRes = 0;
	
	nFunRes = 0;
	if (strQName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (NULL == pDeqMsg)
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (pDeqMsg->m_strConsumerName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	
	return	nFunRes;
}

   
int COracleConnection::_GetDeqQueueName(const std::string& strQNameSrc, std::string& strQNameGet)
{
	int nFunRes = 0;
	
	strQNameGet.clear();
	if (strQNameSrc.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	strQNameGet = strQNameSrc;
	
	return nFunRes;
}

int COracleConnection::_GetDeqBaseInfo(PMAQ_DEQ_OPT pDeqMsg, 
									   int& nDeqMode, int& nDeqNav, int& nDeqFetchSize, std::string& strDeqClientAgentName)
{
	int nFunRes = 0;
	if (NULL == pDeqMsg)
	{
		nFunRes = -1;
		return nFunRes;
	}

	nDeqFetchSize = pDeqMsg->m_nDeqFetchSize; //200kb  // the row of message for MySQL, the size of message for Oracle

	strDeqClientAgentName = pDeqMsg->m_strConsumerName;

	//get deqMode
	if (DEQ_REMOVE == pDeqMsg->m_nDeqMode)
	{
		nDeqMode = OCI_ADM_REMOVE;
	}
	else if (DEQ_BROWSE == pDeqMsg->m_nDeqMode)
	{
		nDeqMode = OCI_ADM_BROWSE;
	}
	else
	{
		nDeqMode = OCI_ADM_REMOVE;
	}

	//get deqNav
	if (DEQ_FIRST_MSG == pDeqMsg->m_nDeqNav)
	{
		nDeqNav = OCI_ADN_FIRST_MSG;
	}
	else if (DEQ_NEXT_MSG == pDeqMsg->m_nDeqNav)
	{
		nDeqNav = OCI_ADN_NEXT_MSG;
	}
	else
	{
		nDeqNav = OCI_ADN_FIRST_MSG;
	}


	return nFunRes;
}

int COracleConnection::_GetDeqAgentArrayInfo(PMAQ_DEQ_OPT pDeqMsg)
{
	int nFunRes = 0;
	std::string strDeqClientAgentName;
	if (NULL == pDeqMsg)
	{
		nFunRes = -1;
		return nFunRes;
	}

	m_nDeqFetchSize = pDeqMsg->m_nDeqFetchSize; //200kb  // the row of message for MySQL, the size of message for Oracle

	strDeqClientAgentName = pDeqMsg->m_strConsumerName;
	m_strDeqClientAgentName = pDeqMsg->m_strConsumerName;

	m_strDeqAgentArray = strDeqClientAgentName;
	m_lstDeqAgentArray.push_back(strDeqClientAgentName);
	m_nDeqAgentArraySize = m_lstDeqAgentArray.size();


	return nFunRes;
}

int	COracleConnection::_CreateDeqHandle(const std::string& strQueueName, OCI_TypeInfo* pAQDataTypeInfo)
{
	int nFunRes = 0;
	int nOCIFunRes = 0;

	if (strQueueName.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (NULL == pAQDataTypeInfo)
	{
		nFunRes = -1;
		return nFunRes;
	}


	if (NULL == m_pDeqDequeue)
	{
		m_pDeqDequeue = OCI_DequeueCreate(pAQDataTypeInfo, strQueueName.c_str());
		if (NULL == m_pDeqDequeue)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "erro! OCI_DequeueCreate()"); 
			nFunRes = -1;
			return nFunRes;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"m_pDeqDequeue OCI_DequeueCreate"); 
 
	}


	/* set list of consumers to wait message for */
	nOCIFunRes = OCI_DequeueSetAgentList(m_pDeqDequeue, m_ppDeqAgentArray, m_nDeqAgentArraySize);
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetAgentList()");
		nFunRes = -1;
		return nFunRes;
	}


	//m_nDeqMode = OCI_ADM_BROWSE  OCI_ADM_REMOVE	OCI_ADM_REMOVE_NODATA
	nOCIFunRes = OCI_DequeueSetMode(m_pDeqDequeue, m_nDeqMode); 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetMode()"); 
		nFunRes = -1;
		return nFunRes;
	}

	//m_nDeqNav = OCI_ADN_FIRST_MSG OCI_ADN_NEXT_MSG
	nOCIFunRes = OCI_DequeueSetNavigation(m_pDeqDequeue, m_nDeqNav); 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetNavigation()"); 
		nFunRes = -1;
		return nFunRes;
	}


	nOCIFunRes = OCI_DequeueSetWaitTime(m_pDeqDequeue, defDefault_Deq_WaitTime);//wait Time is 2 seconds 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetWaitTime()"); 
		nFunRes = -1;
		return nFunRes;
	}

	/* inform Oracle we want to dequeue message for the given consumer */
	nOCIFunRes = OCI_DequeueSetConsumer(m_pDeqDequeue, m_strDeqClientAgentName.c_str()); 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetConsumer()"); 
		nFunRes = -1;
		return nFunRes;
	}

	return nFunRes;
}


int COracleConnection::_GetSetDeqModeNav(PMAQ_DEQ_OPT pDeqMsg)
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	
	
	if (NULL == m_pDeqDequeue || NULL == pDeqMsg)
	{
		nFunRes = -1;
		return nFunRes;
	}
	

	_GetDeqBaseInfo(pDeqMsg, m_nDeqMode, m_nDeqNav, m_nDeqFetchSize, m_strDeqClientAgentName);
	
	
	//m_nDeqMode = OCI_ADM_BROWSE  OCI_ADM_REMOVE   OCI_ADM_REMOVE_NODATA
	nOCIFunRes = OCI_DequeueSetMode(m_pDeqDequeue, m_nDeqMode); 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetMode()"); 
		nFunRes = -1;
		return nFunRes;
	}

	//m_nDeqNav = OCI_ADN_FIRST_MSG OCI_ADN_NEXT_MSG
	nOCIFunRes = OCI_DequeueSetNavigation(m_pDeqDequeue, m_nDeqNav); 
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueSetNavigation()"); 
		nFunRes = -1;
		return nFunRes;
	}
	
	return nFunRes;
}

int	COracleConnection::_FreeDeqHandle()
{
	int nFunRes = 0;
	int nOCIFunRes = 0;

	nOCIFunRes = OCI_DequeueFree(m_pDeqDequeue);
	m_pDeqDequeue = NULL;
	if (1 != nOCIFunRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_DequeueFree()"); 
		nFunRes = -1;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"m_pDeqDequeue OCI_DequeueFree"); 


	return nFunRes;
}


int  COracleConnection::_GetDeqPayload(const std::string& strColumnName, const std::string& strColumnValue, std::string& strPayload)
{
	int nFunRes = 0;
	std::string strValueSize;
	strPayload.clear();

	strValueSize = CUtilityFun::int2str(strColumnValue.size());
	strPayload  = (strValueSize + strColumnName + "=" + strColumnValue);

	return nFunRes;
}
int COracleConnection::_DeqOneMagRemove(setStrConT& setDeqGetMsgId, LstMsgConT& rLstMsg, int &nOneRowDataLen)
{
	int				nFunRes = 0;
	OCI_Msg*		deqMsgHandle = NULL;
	std::string		strMsgIdValue;
	unsigned int	nMsgLen = 64;
	int				nErrorCode = 0;
	std::string     strError;

	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "_DeqOneMagRemove() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}	

	nOneRowDataLen = -1;
	//"m_strDeqQueueName,"",strMsgIdValue,"1",111MESSAGE_QUALIFIER=PUBLIC:TRANSACT333AUDIT_DATA_ORACLE=oracle_sql444AUDIT_DATA_MYSQL=mysql_sql"

	/* get messages from queue */
	/*
	* in WIN32, if Oracle DB shutdown or network have error, 
	*OCI_DequeueGet will hang for a long time then crash
	*/
	/*
	* in SOLAIRS, if Oracle DB shutdown or network have error, 
	*OCI_DequeueGet will hang 
	* hang < 5 min and network ok, OCI_DequeueGet still hang, but when free connection,	OCI_DequeueGet will out
	* hang > 5 min and network still error, OCI_DequeueGet still hang, when free connection, free will hang too; then exit(0)
	*/
	deqMsgHandle = OCI_DequeueGet(m_pDeqDequeue);
	if (NULL == deqMsgHandle)
	{
		//deq error, get error code
		nErrorCode = 0;
		strError.clear();
		_GetOracleErrMsg(strError, nErrorCode);	

		if (0 == nErrorCode)
		{
			//if nErrorCode is 0, deq timeout not get data
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Err:_DeqOneMagRemove OCI_DequeueGet cann't deq data from Oracle queue table");	 
			nFunRes = ER_ORACLE_AQ_DEQUEUE_TIMEOUT;
			return nFunRes; 
		}
		
		else if (ER_ORACLE_DequeueFailed == nErrorCode)
		{
			/*other error, print log and return ErrorCode
			* eg.ER_ORACLE_DequeueFailed  25226
			*/
			nFunRes = nErrorCode;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"err: OCI_DequeueGet have error! [nErrorCode=%d][strError=%s]", nErrorCode, strError.c_str());
			return nFunRes; 
	
		}
		else  
		{
			nFunRes = -1;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"err: OCI_DequeueGet have error! [nErrorCode=%d][strError=%s]", nErrorCode, strError.c_str());
			return nFunRes; 
		} 		

	}//if (NULL == deqMsgHandle)
	
	
	//deq get data
	strMsgIdValue = _OCIMsgGetIDEx(deqMsgHandle, &nMsgLen);
	if (strMsgIdValue.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! _OCIMsgGetIDEx() message id is empty"); 
		nFunRes = -1;
		return nFunRes; 
	}
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "Deq [strMsgIdValue=%s]", strMsgIdValue.c_str());

	//check MsgID
	if (setDeqGetMsgId.find(strMsgIdValue) != setDeqGetMsgId.end())
	{
		//find this msg in remove list
		nFunRes = 0;
	}
	else
	{			
		nFunRes = OCI_Rollback(m_connection);
		if (1 != nFunRes)
		{
			_LOGOracleErrMsg("error ! OCI_Rollback() ");	
		}
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"_DeqOneMagRemove OCI_Rollback[strMsgIdValue=%s]", strMsgIdValue.c_str());
		nFunRes = Err_DEQACkCode_NOTFIND;
	}

	deqMsgHandle = NULL;
	return nFunRes;	
	   
}


int COracleConnection::_DeqOneMagBrowser(setStrConT& setDeqGetMsgId, LstMsgConT& rLstMsg, int &nOneRowDataLen)
{
	unsigned int	nMsgLen = 64;
	int				nFunRes = 0;
	OCI_Msg*		deqMsgHandle = NULL;
	OCI_Object*		deqObjHandle = NULL;
	std::string		strMsgIdValue;
	std::string     strEachColumnName;
	int             nEachColumnType = 0;
	std::string		strEachColumnValue;
	std::string		strMsgSid;
	int             nPriority = 50;//MSGITEM_PR_NORMAL
	CMessage*       pDeqMsg = NULL;
	mapColumNTConIterT iterMapColumn;
	ColumnNameType* pColumnNT = NULL;	
	char*			pGetStrValue = NULL;
	ulong64         nGetBigIntValue = 0;
	std::string		strError;
	int             nErrorCode = 0;
	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_DeqOneMagBrowser() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}

	nOneRowDataLen = -1;
	//"m_strDeqQueueName,"",strMsgIdValue,"1",111MESSAGE_QUALIFIER=PUBLIC:TRANSACT333AUDIT_DATA_ORACLE=oracle_sql444AUDIT_DATA_MYSQL=mysql_sql"

	/* get messages from queue */
	/*
	* in WIN32, if Oracle DB shutdown or network have error, 
	*OCI_DequeueGet will hang for a long time then crash
	*/
	/*
	* in SOLAIRS, if Oracle DB shutdown or network have error, 
	*OCI_DequeueGet will hang 
	* hang < 5 min and network ok, OCI_DequeueGet still hang, but when free connection,	OCI_DequeueGet will out
	* hang > 5 min and network still error, OCI_DequeueGet still hang, when free connection, free will hang too; then exit(0)
	*/
	deqMsgHandle = OCI_DequeueGet(m_pDeqDequeue);
	if (NULL == deqMsgHandle)
	{
		//deq error, get error code
		nErrorCode = 0;
		strError.clear();
		_GetOracleErrMsg(strError, nErrorCode);	

		if (0 == nErrorCode)
		{
			//if nErrorCode is 0, deq timeout not get data
			nFunRes = ER_ORACLE_AQ_DEQUEUE_TIMEOUT;
			return nFunRes; 
		} 			 		
		else if (ER_ORACLE_DequeueFailed == nErrorCode)
		{
			/*other error, print log and return ErrorCode
			* eg.ER_ORACLE_DequeueFailed  25226
			*/
			nFunRes = nErrorCode;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"err: OCI_DequeueGet have error! [nErrorCode=%d][strError=%s]", nErrorCode, strError.c_str());
			return nFunRes; 
		} 
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"err: OCI_DequeueGet have error! [nErrorCode=%d][strError=%s]", nErrorCode, strError.c_str());
			nFunRes = -1;
			return nFunRes; 
		}
		

	}//if (NULL == deqMsgHandle)
	
	//deq ok and get data
	strMsgIdValue = _OCIMsgGetIDEx(deqMsgHandle, &nMsgLen);
	if (strMsgIdValue.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! _OCIMsgGetIDEx() message id is empty"); 
		nFunRes = -1;
		return nFunRes; 
	}
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "Deq [strMsgIdValue=%s]", strMsgIdValue.c_str());
	
	//check if have same msgid	
	if (setDeqGetMsgId.find(strMsgIdValue) != setDeqGetMsgId.end())
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
				"[_DeqOneMagBrowser][setDeqGetMsgId.find(strMsgIdValue) != setDeqGetMsgId.end()]"); 
		nFunRes = ER_ORACLE_AQ_DEQUEUE_TIMEOUT;
		return nFunRes; 		
	}
	else
	{
		setDeqGetMsgId.insert(strMsgIdValue);
	}
	
	nPriority = OCI_MsgGetPriority(deqMsgHandle);
	//strPriority = CUtilityFun::int2str(nPriority);		  
	/* get message object payload */
	deqObjHandle = OCI_MsgGetObject(deqMsgHandle);
	if (NULL == deqObjHandle)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgGetObject()"); 
		nFunRes = -1;
		return nFunRes; 
	}

	// at.q_name, at.msg_sid, at.msgid, at.priority, at.user_data
	pDeqMsg = new CMessage();
	pDeqMsg->setMsgID(strMsgIdValue);
	nOneRowDataLen += strMsgIdValue.length();

	pDeqMsg->setQName(m_strDeqQueueName);
	nOneRowDataLen += m_strDeqQueueName.length();

	pDeqMsg->setMsgPriority(nPriority);
	nOneRowDataLen += 4;

	pDeqMsg->setPayloadType(m_nPayLoadType);
							
	iterMapColumn = m_mapAQDataTypeInfoColumn.begin();
	while (iterMapColumn!= m_mapAQDataTypeInfoColumn.end())
	{	
		pColumnNT = iterMapColumn->second;

		strEachColumnName = pColumnNT->m_strColumnName;
		nEachColumnType = pColumnNT->m_nColumnType;
		switch (nEachColumnType)
		{
		case OCI_CDT_TEXT:      //  : dtext *:
			{
				pGetStrValue = NULL;
				pGetStrValue = (char *)OCI_ObjectGetString(deqObjHandle, strEachColumnName.c_str());
				if (NULL != pGetStrValue)
				{
					strEachColumnValue = pGetStrValue; 
				}
				else
				{
					// clean the previous value and set it to NULL
					strEachColumnValue.clear();

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
						"Deq [strMsgIdValue=%s][strEachColumnName=%s][nEachColumnType=%d][strEachColumnValue=NULL]",
						strMsgIdValue.c_str(), strEachColumnName.c_str(), nEachColumnType);
				}
				pDeqMsg->setPayload(strEachColumnName, strEachColumnValue);
				nOneRowDataLen += strEachColumnName.length();
				nOneRowDataLen += strEachColumnValue.length();
				LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
					"Deq [strMsgIdValue=%s][strEachColumnName=%s][nEachColumnType=%d][strEachColumnValue=%s]",
					strMsgIdValue.c_str(), strEachColumnName.c_str(), nEachColumnType, strEachColumnValue.c_str());
				break;
			}
		case OCI_CDT_NUMERIC:   //  : short, int, long long, double
			{
				nGetBigIntValue = OCI_ObjectGetUnsignedBigInt(deqObjHandle, strEachColumnName.c_str());
				pDeqMsg->setPayload(strEachColumnName, nGetBigIntValue);
				nOneRowDataLen += strEachColumnName.length();
				nOneRowDataLen += 4;
				LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
					"Deq [strMsgIdValue=%s][strEachColumnName=%s][nEachColumnType=%d][nGetBigIntValue=%llu]",
					strMsgIdValue.c_str(), strEachColumnName.c_str(), nEachColumnType, nGetBigIntValue);

				break;
			}
		default:
			{	
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Deq UNKNOW DataType [strMsgIdValue=%s][strEachColumnName=%s][nEachColumnType=%d]",
					strMsgIdValue.c_str(), strEachColumnName.c_str(), nEachColumnType);				
				break;
			}

		}//switch

		iterMapColumn++;
	}//while 

	if (NULL != pDeqMsg)
	{
		pDeqMsg->setOServerID(0);//oracle always 0

		rLstMsg.push_back(pDeqMsg);
		pDeqMsg = NULL;	
	}

	deqMsgHandle = NULL;
	deqObjHandle = NULL;
	pGetStrValue = NULL;
	pDeqMsg = NULL;	

	return nFunRes;	

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int COracleConnection::_EnqInit( CMessage* pEnqMsg )
{
	int nFunRes = 0;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"_EnqInit()");
	m_bEnqInit = false;
	m_strEnqQueueName.clear();
	m_strEnqAgentArray.clear();
	m_lstEnqAgentArray.clear();
	m_ppEnqAgentArray = NULL;
	m_nEnqAgentArraySize = 0;
	m_pEnqEnqueueHandle = NULL;
	m_pEngMsgHandle = NULL;
	m_pEnqObjectHandle = NULL;	

	if (NULL == m_connection)
	{
		nFunRes = -1;
		return nFunRes;
	}		
	//check enqMsg input param
	else if (0 != _CheckEnqMsgInputParam(pEnqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}	

	m_lstEnqAgentArray.clear();

	
	//get AQ info from input param
	if (0 != _GetEnqQName(pEnqMsg, m_strEnqQueueName))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _GetStrEnqAgentArray(pEnqMsg, m_strEnqAgentArray))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _GetLstEnqAgentArray(m_strEnqAgentArray, m_lstEnqAgentArray))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _GetAQDataTypeNameByAQQName(m_strEnqQueueName, m_strAQDataTypeName))
	{
		nFunRes = -1;
		return nFunRes;
	}  
	else if (0 != _GetLstAQDataTypeInfoColumn(m_strAQDataTypeName, m_mapAQDataTypeInfoColumn))
	{
		nFunRes = -1;
		return nFunRes;
	}

	m_nEnqAgentArraySize =  m_lstEnqAgentArray.size();

	//prepare enq handle	
	if (0 != _CreateEnqAgentArray(m_lstEnqAgentArray))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _CreateEnqHandle(m_strEnqQueueName, m_pAQDataTypeInfo))
	{
		nFunRes = -1;
		return nFunRes;	
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"_EnqInit() ok! [strEnqQueueName=%s][strEnqAgentArray=%s]",
		m_strEnqQueueName.c_str(), m_strEnqAgentArray.c_str());

	
	return nFunRes;	


}

int COracleConnection::_EnqUnInit()
{
	int nFunRes = 0;
	int nOCIFunRes = 0;	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_EnqUnInit()");

	/*
	this OCI_MsgFree may hang in function	OCIObjectFree	
	so not run
	TODO. 
	*/
	// 	if (NULL != m_pEngMsgHandle)
	// 	{
	// 		nOCIFunRes = OCI_MsgFree(m_pEngMsgHandle);
	// 		m_pEngMsgHandle = NULL;
	// 		if (1 != nOCIFunRes)
	// 		{
	// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_MsgFree()");
	// 			nFunRes = -1; 
	// 		} 
	// 	}
	
	
	
	//m_pEnqObjectHandle will be reuse in enq msg and is inited use param: OCI_DURATION_TRANS,
	//free m_pEnqObjectHandle when disconnect to server
	if (0 != _FreeEnqObjetHandle())
	{
		nFunRes = Err_EnqReqACkCode_OCILibErr;
		return nFunRes;	
	}
 

	if (NULL != m_pEnqEnqueueHandle)
	{
		nOCIFunRes = OCI_EnqueueFree(m_pEnqEnqueueHandle);
		m_pEnqEnqueueHandle = NULL;
		if (1 != nOCIFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! OCI_EnqueueFree()"); 
			nFunRes = -1;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_pEnqEnqueueHandle OCI_EnqueueFree");

	}

	if (0 != _FreeEnqAgentArray())
	{
		nFunRes = -1;
		return nFunRes;	
	}
	m_bEnqInit = false;
	return nFunRes;
}

bool COracleConnection::_CheckEnqParamSame(CMessage* pEnqMsg)
{
	bool            bCheckEnqParamSameRes = false;             

	int				nFunRes = 0;
	std::string		strEnqQueueNameNeedCheck;  //m_strEnqQueueName
	std::string		strEnqAgentArrayNeedCheck;//m_strEnqAgentArray


	//get info from input param
	if (0 != _GetEnqQName(pEnqMsg, strEnqQueueNameNeedCheck))
	{	
		nFunRes = -1;
		bCheckEnqParamSameRes = false;
		return bCheckEnqParamSameRes;
	}
	else if (0 != _GetStrEnqAgentArray(pEnqMsg, strEnqAgentArrayNeedCheck))
	{
		nFunRes = -1;
		bCheckEnqParamSameRes = false;
		return bCheckEnqParamSameRes;
	}
		 
	//check
	bCheckEnqParamSameRes = false; 	
	if (strEnqQueueNameNeedCheck != m_strEnqQueueName)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[strEnqQueueNameNeedCheck=%s] != [strEnqQueueName=%s]",
			strEnqQueueNameNeedCheck.c_str(), m_strEnqQueueName.c_str());

		bCheckEnqParamSameRes = false;
		return bCheckEnqParamSameRes;
	}
	else if (strEnqAgentArrayNeedCheck != m_strEnqAgentArray)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[strEnqAgentArrayNeedCheck=%s] != [strEnqAgentArray=%s]",
			strEnqAgentArrayNeedCheck.c_str(), m_strEnqAgentArray.c_str());

		bCheckEnqParamSameRes = false;
		return bCheckEnqParamSameRes;
	}
	else
	{
		bCheckEnqParamSameRes = true;
	}
	

	return bCheckEnqParamSameRes;
}

int COracleConnection::_DeqInit(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg)
{
	int nFunRes = 0;
	
	//check enqMsg input param
    if (0 != _GetDeqQueueName(strQName, m_strDeqQueueName))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _GetDeqBaseInfo(pDeqMsg, m_nDeqMode, m_nDeqNav, m_nDeqFetchSize, m_strDeqClientAgentName))
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _GetDeqAgentArrayInfo(pDeqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}
	//m_strQueueName
	else if (0 != _GetAQDataTypeNameByAQQName(m_strDeqQueueName, m_strAQDataTypeName))
	{
		nFunRes = -1;
		return nFunRes;
	}  
	else if (0 != _GetLstAQDataTypeInfoColumn(m_strAQDataTypeName, m_mapAQDataTypeInfoColumn))
	{
		nFunRes = -1;
		return nFunRes;
	}			   

	//prepare enq handle	 m_lstDeqAgentArray
	if (0 != _CreateDeqAgentArray())
	{
		nFunRes = -1;
		return nFunRes;
	}
	else if (0 != _CreateDeqHandle(m_strDeqQueueName, m_pAQDataTypeInfo))
	{
		nFunRes = -1;
		return nFunRes;	
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"_DeqInit() ok! [strDeqQueueName=%s][strDeqClientAgentName=%s]",
		m_strDeqQueueName.c_str(), m_strDeqClientAgentName.c_str());

	return nFunRes;
}
int COracleConnection::_DeqUnInit()
{
	int nFunRes = 0;

	//prepare enq handle	
	if (0 != _FreeDeqAgentArray())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//TODO.
	//_FreeDeqHandle will use	OCI_MsgFree()
	//function OCI_MsgFree() mybe hang in function	OCIObjectFree
	// when cs Oracle tcp diconnect memery problem	
	//if (0 != _FreeDeqHandle())
	//{
	//	nFunRes = -1;
	//	return nFunRes;	
	//}

	m_pDeqDequeue = NULL;	   
	m_bDeqInit = false;

	return nFunRes;
}
bool COracleConnection::_CheckDeqParamSame(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg)
{
	bool bCheckDeqParamSameRes = false;
	int nFunRes = 0;
	std::string strDeqQNameNeedCheck;//m_strDeqQueueName
	std::string strDeqClientAgentNameNeedCheck;//m_strDeqClientAgentName
	int	nDeqModeNeedCheck; 
	int	nDeqNavNeedCheck;
	int	nDeqFetchSize; //200kb 
	vecStrConT	lstDeqAgentArray;
								 
	//check enqMsg input param
	if (0 != _GetDeqQueueName(strQName, strDeqQNameNeedCheck))
	{
		nFunRes = -1;
		bCheckDeqParamSameRes = false;
		return bCheckDeqParamSameRes;
	}
	else if (0 != _GetDeqBaseInfo(pDeqMsg, nDeqModeNeedCheck, nDeqNavNeedCheck, 
		nDeqFetchSize, strDeqClientAgentNameNeedCheck))
	{
		nFunRes = -1;
		bCheckDeqParamSameRes = false;
		return bCheckDeqParamSameRes;
	}


	//check
	bCheckDeqParamSameRes = false;
	if (nDeqModeNeedCheck != m_nDeqMode)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,	
			"[DeqModeNeedCheck=%d] != [DeqMode=%d]",
			nDeqModeNeedCheck, m_nDeqMode);
		bCheckDeqParamSameRes = false;
		return bCheckDeqParamSameRes;
	}
	else if (nDeqNavNeedCheck != m_nDeqNav)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,	
			"[DeqNavNeedCheck=%d] != [DeqNav=%d]",
			nDeqNavNeedCheck, m_nDeqNav);
		bCheckDeqParamSameRes = false;
		return bCheckDeqParamSameRes;
	}
	else if (strDeqQNameNeedCheck != m_strDeqQueueName)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,	
				"[strDeqQNameNeedCheck=%s] != [DeqQueueName=%s]",
				strDeqQNameNeedCheck.c_str(), m_strDeqQueueName.c_str());

		bCheckDeqParamSameRes = false;
		return bCheckDeqParamSameRes;
	}
	else
	{
		bCheckDeqParamSameRes = true;
	}
	m_nDeqFetchSize = nDeqFetchSize;
	
	lstDeqAgentArray.clear();
	return bCheckDeqParamSameRes;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


int COracleConnection::enqMsg( CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt )
{
	int nFunRes = 0;
	
	m_strEnqQueueName =  pEnqMsg->getQName();	
 
	if (ENQ_AQ_API == pEnqOpt->m_nEnqMode)
	{
		//use Ocilib fun to enq msg
		nFunRes = _OcilibEnqMsg(pEnqMsg);
	}
	else if (ENQ_SQL_PRC_NoPrepare == pEnqOpt->m_nEnqMode)
	{
		//use sql to call package enq msg
		nFunRes = _SqlEnqMsg(pEnqMsg);
	}
	else if (ENQ_SQL_PRC_Prepare_One == pEnqOpt->m_nEnqMode)
	{
		//use sql to call package enq msg
		nFunRes = _SqlEnqMsgPrepare(pEnqMsg);
	}
	else
	{
		//default use sql to call package enq msg
		nFunRes = _SqlEnqMsg(pEnqMsg);
	
	}
	return nFunRes;	
}




int COracleConnection::enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt )
{
	int nFunRes = 0; 

	if (ENQ_SQL_PRC_Prepare_Batch == pEnqOpt->m_nEnqMode)
	{
		//use sql to call package enq msg
		nFunRes = _SqlEnqMsgPrepareArray(EnqMsgList);
	}
	else
	{
		nFunRes = -1;		
	}
	return nFunRes;	
}


int COracleConnection::_SqlEnqMsg( CMessage* pEnqMsg )
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	std::string strError;
	int nErrorCode = 0;
	
	const char* pMessageQualifierValue = NULL;
	const char* pAuditDataOracleValue  = NULL;
	const char* pAuditDataMySQLValue   = NULL;
	
	char*  pEscapedPayloadOra = NULL;
	char*  pEscapedPayloadMy  = NULL;

	m_bEnqSet = true;
	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "_SqlEnqMsg() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}
	
	if (NULL == m_pStatement)
	{
		m_pStatement = 	OCI_StatementCreate(m_connection);
	}
	if (NULL == m_pStatement)
	{
		_LOGOracleErrMsg("OCI_StatementCreate()", 1);
		nFunRes = -1;
		return nFunRes;
	}

	nFunRes = pEnqMsg->getPayload(s_strKeyMessageQualifier, pMessageQualifierValue);
	nFunRes = pEnqMsg->getPayload(s_strKeyAuditDataOracle, pAuditDataOracleValue);
	nFunRes = pEnqMsg->getPayload(s_strKeyAuditDataMySQL, pAuditDataMySQLValue);
	
	
	sprintf(m_pBufferSQL, "[QueueName=%s][%s=%s][%s=%s][%s=%s]",
		m_strEnqQueueName.c_str(),
		s_strKeyMessageQualifier.c_str(), pMessageQualifierValue,
		s_strKeyAuditDataOracle.c_str(), pAuditDataOracleValue,
		s_strKeyAuditDataMySQL.c_str(), pAuditDataMySQLValue);
	LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, m_pBufferSQL);


	pEscapedPayloadOra = CUtilityFun::escapeSQLStr((char*)pAuditDataOracleValue, "'", "''");
	pEscapedPayloadMy  = CUtilityFun::escapeSQLStr((char*)pAuditDataMySQLValue, "'", "''");

	//m_pDataBaseForSqlStr->prepareSQLStatement(strSql, ENQ_SQL_PRC_ENQ_AUDIT_DATA_8001, 
	//				pMessageQualifierValue, pEscapedPayloadOra, pEscapedPayloadMy);

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Prepare SQL");
	
	//"begin prc_enq_audit_data('%s','%s','%s');end;"
	sprintf(m_pBufferSQL, s_strENQSQLFormat.c_str(),
		pMessageQualifierValue, pEscapedPayloadOra, pEscapedPayloadMy);

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Prepare SQL");

	if (NULL != pEscapedPayloadOra)
	{
		free(pEscapedPayloadOra); 
		pEscapedPayloadOra = NULL;
	}

	if (NULL != pEscapedPayloadMy)
	{
		free(pEscapedPayloadMy); 
		pEscapedPayloadMy = NULL;
	}
	
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Execute SQL  strSQL=");
 	LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, m_pBufferSQL);

	nOCIFunRes = OCI_ExecuteStmt(m_pStatement, m_pBufferSQL);
	if (1 != nOCIFunRes)
	{ 
		_GetOracleErrMsg(strError, nErrorCode);	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"err: prc_enq_audit_data have error! [nErrorCode=%d][strError=%s][statement=%s]", 
			nErrorCode, strError.c_str(), m_pBufferSQL);	
						
		if (ER_ORACLE_EnqueueFailed == nErrorCode)
		{
			nFunRes = nErrorCode;
		}
		else
		{
			nFunRes = -1;
		}	
	}
	
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Execute SQL");
	
	if (NULL != m_pStatement)
	{
		nOCIFunRes = OCI_StatementReset(m_pStatement);
	}
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_StatementReset()", 1);
		nFunRes = -1;
	} 

	return nFunRes;	
}


int COracleConnection::_SqlEnqMsgPrepare( CMessage* pEnqMsg )
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	int nErrorCode = 0;	
	const char* pMessageQualifierValue = NULL;
	const char* pAuditDataOracleValue  = NULL;
	const char* pAuditDataMySQLValue   = NULL;

	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}
	
	if (NULL == m_pStatement)
	{
		m_pStatement = 	OCI_StatementCreate(m_connection);
	}
	if (NULL == m_pStatement)
	{
		_LOGOracleErrMsg("OCI_StatementCreate()", 1);
		nFunRes = -1;
		return nFunRes;
	}
	  
	nFunRes = pEnqMsg->getPayload(s_strKeyMessageQualifier, pMessageQualifierValue);
	nFunRes = pEnqMsg->getPayload(s_strKeyAuditDataOracle, pAuditDataOracleValue);
	nFunRes = pEnqMsg->getPayload(s_strKeyAuditDataMySQL, pAuditDataMySQLValue);
			
	sprintf(m_pBufferSQL, "[QueueName=%s][%s=%s][%s=%s][%s=%s]",
		m_strEnqQueueName.c_str(),
		s_strKeyMessageQualifier.c_str(), pMessageQualifierValue,
		s_strKeyAuditDataOracle.c_str(), pAuditDataOracleValue,
		s_strKeyAuditDataMySQL.c_str(), pAuditDataMySQLValue);
	LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, m_pBufferSQL);

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Prepare SQL");

	LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "OCI_Prepare [strSQL=%s]", s_strSQLPrcEnqBindSQLFormat.c_str());

	//prepare
	nOCIFunRes = OCI_Prepare(m_pStatement, s_strSQLPrcEnqBindSQLFormat.c_str());
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_Prepare()", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;
	}


	//bind data
	nOCIFunRes = OCI_BindString(m_pStatement, s_strBindMessageQualifier.c_str(), (char*)pMessageQualifierValue, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() :MessageQualifierValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;	
	}

	nOCIFunRes = OCI_BindString(m_pStatement, s_strBindKeyAuditDataOracle.c_str(), (char*)pAuditDataOracleValue, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() :AuditDataOracleValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;

	}

	nOCIFunRes = OCI_BindString(m_pStatement, s_strBindKeyAuditDataMySQL.c_str(), (char*)pAuditDataMySQLValue, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() :AuditDataMySQLValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;
	}
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Prepare SQL");
	

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Execute SQL  ");

	//execute sql
	nOCIFunRes = OCI_Execute(m_pStatement);
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_Execute()", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;
	} 
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Execute SQL");

	if (NULL != m_pStatement)
	{
		nOCIFunRes = OCI_StatementReset(m_pStatement);
	}
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_StatementReset()", 1);
		nFunRes = -1;
	}

	return nFunRes;	
}

int COracleConnection::_PrepareBatchInsertionMemory(int nBatchSize)
{
	int nFunRes = 0;

	if (false == m_bBufferInitd)
	{
		m_ppBuffMessageQualifierValue =	 new char[nBatchSize][101];
		m_ppBuffAuditDataOracleValue =	 new char[nBatchSize][4001];
		m_ppBuffAuditDataMySQLValue =	 new char[nBatchSize][4001];
		m_bBufferInitd = true;
		m_nBatchSize = nBatchSize;
		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Batch Memory[BatchSize=%d]", m_nBatchSize);

	}

	//if old memory is not enough, then create new memory
	if (m_nBatchSize < nBatchSize)
	{
		DEF_DELETE_ARRAY(m_ppBuffMessageQualifierValue);
		DEF_DELETE_ARRAY(m_ppBuffAuditDataOracleValue);
		DEF_DELETE_ARRAY(m_ppBuffAuditDataMySQLValue);

		m_ppBuffMessageQualifierValue =	 new char[nBatchSize][101];
		m_ppBuffAuditDataOracleValue =	 new char[nBatchSize][4001];
		m_ppBuffAuditDataMySQLValue =	 new char[nBatchSize][4001];
		m_bBufferInitd = true;
		m_nBatchSize = nBatchSize;
		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "rePrepare Batch Memory[BatchSize=%d]", m_nBatchSize);

	}

	return nFunRes; 
}

int COracleConnection::_SqlEnqMsgPrepareArray(std::list<CMessage*>& EnqMsgList)
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	int nErrorCode = 0;

	const char* pMessageQualifierValue = NULL;
	const char* pAuditDataOracleValue  = NULL;
	const char* pAuditDataMySQLValue   = NULL;
	std::list<CMessage*>::iterator iterLst;
	CMessage* pMsgGet = NULL;
	int       nBatchSize = 0;
	int       nIndex = 0;
	int       nCount = 0; 	

	if (false == isOpen())
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}
	
	if (NULL == m_pStatement)
	{
		m_pStatement = 	OCI_StatementCreate(m_connection);
	}
	if (NULL == m_pStatement)
	{
		_LOGOracleErrMsg("OCI_StatementCreate()", 1);
		nFunRes = -1;
		return nFunRes;
	}

	  
	nBatchSize = EnqMsgList.size();	//50

	//prepare batch insertion memory
	//char (*ppBuffMessageQualifierValue)[101] = new char[nBatchSize][101];
	//char (*ppBuffAuditDataOracleValue)[4001] = new char[nBatchSize][4001];
	//char (*ppBuffAuditDataMySQLValue)[4001] = new char[nBatchSize][4001];
	_PrepareBatchInsertionMemory(nBatchSize);
	 
	LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_SqlEnqMsgPrepareArray begin copy datalist to buffer");

	nIndex = 0;
	iterLst = EnqMsgList.begin();
	while (iterLst != EnqMsgList.end())
	{
		pMsgGet = *iterLst;
		pMessageQualifierValue = NULL;
		pAuditDataOracleValue = NULL;
		pAuditDataMySQLValue = NULL;
		m_strEnqQueueName =  pMsgGet->getQName();	


		nFunRes = pMsgGet->getPayload(s_strKeyMessageQualifier, pMessageQualifierValue);
		nFunRes = pMsgGet->getPayload(s_strKeyAuditDataOracle, pAuditDataOracleValue);
		nFunRes = pMsgGet->getPayload(s_strKeyAuditDataMySQL, pAuditDataMySQLValue);
		
		
		if (NULL != pMessageQualifierValue)
		{
			sprintf(m_ppBuffMessageQualifierValue[nIndex], "%s", pMessageQualifierValue);
		} 

		if (NULL != pAuditDataOracleValue)
		{
			sprintf(m_ppBuffAuditDataOracleValue[nIndex], "%s", pAuditDataOracleValue);
		}

		if (NULL != pAuditDataMySQLValue)
		{
			sprintf(m_ppBuffAuditDataMySQLValue[nIndex], "%s", pAuditDataMySQLValue);
		}
		
		//for sql log
		sprintf(m_pBufferSQL, "[QueueName=%s][%s=%s][%s=%s][%s=%s]",
			m_strEnqQueueName.c_str(),
			s_strKeyMessageQualifier.c_str(), pMessageQualifierValue,
			s_strKeyAuditDataOracle.c_str(), pAuditDataOracleValue,
			s_strKeyAuditDataMySQL.c_str(), pAuditDataMySQLValue);
		LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, m_pBufferSQL);
	
		

		iterLst++;
		nIndex++;
	}//while
	
	nCount = nIndex;

	LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_SqlEnqMsgPrepareArray End copy datalist to buffer");

	
	
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Prepare SQL");

	LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "OCI_Prepare [strSQL=%s]", s_strSQLPrcEnqBindSQLFormat.c_str());
	
	//prepare
	nOCIFunRes = OCI_Prepare(m_pStatement, s_strSQLPrcEnqBindSQLFormat.c_str()); 
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_Prepare()", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;
	}

	nOCIFunRes = OCI_BindArraySetSize(m_pStatement, nCount);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindArraySetSize()", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;
	}  
	 
		
	//bind data
	nOCIFunRes = OCI_BindArrayOfStrings(m_pStatement, 
		s_strBindMessageQualifier.c_str(), (char*)m_ppBuffMessageQualifierValue, 100, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() pMessageQualifierValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;	
	}

	nOCIFunRes = OCI_BindArrayOfStrings(m_pStatement, 
		s_strBindKeyAuditDataOracle.c_str(), (char*)m_ppBuffAuditDataOracleValue, 4000, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() AuditDataOracleValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;	
	}

	nOCIFunRes = OCI_BindArrayOfStrings(m_pStatement, 
		s_strBindKeyAuditDataMySQL.c_str(), (char*)m_ppBuffAuditDataMySQLValue, 4000, 0);
	if(1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_BindString() AuditDataMySQLValue", 1);
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;
		nFunRes = -1;
		return nFunRes;	
	}

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Prepare SQL");

	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "Begin Execute SQL");

	//Execute sql
	nOCIFunRes = OCI_Execute(m_pStatement);
	if (1 != nOCIFunRes)
	{
		//OCI_Error* pErr = NULL; 
		//pErr = OCI_GetBatchError(pStatement);  
		_LOGOracleErrMsg("OCI_Execute()", 1);
		
		nOCIFunRes = OCI_StatementFree(m_pStatement);
		m_pStatement = NULL;

		nFunRes = -1;
		return nFunRes;
	}
	LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, "End Execute SQL");


	if (NULL != m_pStatement)
	{
		nOCIFunRes = OCI_StatementReset(m_pStatement);
	}
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_StatementReset()", 1);
		nFunRes = -1;
	}

	pMsgGet = NULL;	

	return nFunRes;
}






//************************************
// Method:    enqMsg
// FullName:  TA_AQ_Lib::COracleConnection::enqMsg
// Access:    public 
// Returns:   int
//		0
//		Err_EnqReqACkCode_PLTypeInvalid
//		Err_EnqReqACkCode_OCILibErr
//		ER_ORA_EnqueueFailed = 25207
// Qualifier:
// Parameter: CMessage * pEnqMsg
//************************************
int COracleConnection::_OcilibEnqMsg( CMessage* pEnqMsg )
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	int nErrorCode = 0;
	std::string strError;

	m_bEnqSet = true;
	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"COracleConnection::enqMsg() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}	
	
		
	if (false == m_bEnqInit)
	{
		nFunRes = _EnqInit(pEnqMsg);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! _EnqInit()"); 
			nFunRes = -1;
			return nFunRes;
		}
		m_bEnqInit = true;
	}
		
	/*
	*_CreateEnqObjetHandle Return Value:
	*Return	0: Create EnqObject Ok
	*Return	Err_EnqReqACkCode_PLTypeInvalid, Err_EnqReqACkCode_OCILibErr, Err_EnqReqACkCode_DataSrcErr: Create EnqObject Error
	*EnqMsg handle is NULL, nErrorCode value: Err_EnqReqACkCode_PLTypeInvalid
	*EnqMsg's object handle is NULL, nErrorCode value:Err_EnqReqACkCode_OCILibErr
	*msg data have error, nErrorCode value: Err_EnqReqACkCode_DataSrcErr
	*/   	
	nErrorCode = _CreateEnqObjetHandle(pEnqMsg);
	if (0 != nErrorCode)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"error! _CreateEnqObjetHandle()[nErrorCode=%d]", nErrorCode); 
   		nFunRes = nErrorCode;
		return nFunRes;	
	}
	  
	//enq data 	
	/*
	* in WIN32, if Oracle DB shutdown or network have error, 
	*OCI_EnqueuePut will hang for a long time then crash
	*/
	/*
	* in SOLAIRS, if Oracle DB shutdown or network have error, 
	*OCI_EnqueuePut will hang 
	* hang < 5 min and network ok, OCI_EnqueuePut still hang, but when free connection,	OCI_EnqueuePut will out
	* hang > 5 min and network still error, OCI_EnqueuePut still hang, when free connection, free will hang too; then exit(0)
	*/
	nOCIFunRes = OCI_EnqueuePut(m_pEnqEnqueueHandle, m_pEngMsgHandle);
	if (1 != nOCIFunRes)
	{
		_GetOracleErrMsg(strError, nErrorCode);	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err: OCI_EnqueuePut have error! [nErrorCode=%d][strError=%s]", nErrorCode, strError.c_str());
		
		if (ER_ORACLE_EnqueueFailed == nErrorCode)
		{
			nFunRes = nErrorCode;
		}
		else
		{
			nFunRes = -1;
		}
		//return here  m_pEnqObjectHandle still have data  will be reuse next time
		//_FreeEnqObjetHandle may hang here so just have memory problem
		//TODO. this one ObjectHandle don't free, still have a litter memory problem	
		return nFunRes;		
	} 
	else
	{
		nFunRes = 0;
	}
	
	//TODO. m_pEnqObjectHandle still have data, but m_pEnqObjectHandle handle can be reuse next time,
	//don't need to new handle again, enq speed will be more quickly
	//Note enq speed:
	//2713     for each msg step:  newobject  setData   EnqToDB  freeobject   
	//	4098    each msg will reuse one ObjectHandle,    for each msg step:  setData   EnqToDB  
	//	9615    each msg will reuse one ObjectHandle,    set same data,  for each msg step:    EnqToDB 
	 
	//if (0 != _FreeEnqObjetHandle())
	//{
	//	nFunRes = Err_EnqReqACkCode_OCILibErr;
	//	return nFunRes;	
	//}

	return nFunRes;	
}

int COracleConnection::deqMsg( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg )
{
	int nFunRes = 0;

	if (NULL == pDeqMsg)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL == pDeqMsg");
		nFunRes = -1;
		return nFunRes;
	}
	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"COracleConnection::deqMsg() false == isOpen() sleep 10 seconds");
			
		nFunRes = -1;
		return nFunRes;
	}

	if (DEQ_BROWSE == pDeqMsg->m_nDeqMode && rLstMsg.empty())
	{
		nFunRes = _DeqMsgBrowser(strQName, pDeqMsg, rLstMsg);//
	}
	else if (DEQ_REMOVE == pDeqMsg->m_nDeqMode && !rLstMsg.empty())
	{
		nFunRes = _DeqMsgRemove(strQName, pDeqMsg, rLstMsg);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Err: not support DEQ Mode = %d", pDeqMsg->m_nDeqMode);
		nFunRes = -1;
	}

	return nFunRes;	
}

int COracleConnection::_GetMsgIDSetFromList( LstMsgConT& lstMsgSrc, setStrConT&  setMsgIdDest )
{
	int nFunRes = 0;
	LstMsgConT::iterator  iterLst = lstMsgSrc.begin();
	CMessage* pMsgGet = NULL;
	std::string strMsgIdGet;
	
	while (iterLst != lstMsgSrc.end())
	{
		pMsgGet = *iterLst;
		strMsgIdGet = pMsgGet->getMsgID();
		setMsgIdDest.insert(strMsgIdGet);		
		iterLst++;
	}
	
	return nFunRes;
}

int COracleConnection::_DeqMsgRemove( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg )
{
	int nFunRes = 0;
	int nOneRowLen = 0;
	int nDeqRes = -1;
	int nContinueDeq = 1;
	int nDeqDataTotalLen = 0;
	setStrConT  setDeqRemoveMsgId;
	int         nDeqRemoveIndex = 0;

	//check db con
	m_bDeqSet = true;
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_DeqMsgRemove() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}	

	//check enqMsg input param
	if (0 != _CheckDeqMsgInputParam(strQName, pDeqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	//deq handle init
	if (false == m_bDeqInit)
	{
		nFunRes = _DeqInit(strQName, pDeqMsg);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error! _DeqInit()"); 
			nFunRes = -1;
			return nFunRes;
		}
		m_bDeqInit = true;
	}

	//deq handle 
	_GetSetDeqModeNav(pDeqMsg);	
	
	_GetMsgIDSetFromList(rLstMsg, setDeqRemoveMsgId);	

	
	nContinueDeq = 1;
	nDeqRemoveIndex = 0;
	while (nContinueDeq)
	{
		nOneRowLen = -1;
		nDeqRes = -1; 
		nDeqRes = _DeqOneMagRemove(setDeqRemoveMsgId, rLstMsg, nOneRowLen);

		if (ER_ORACLE_AQ_DEQUEUE_TIMEOUT == nDeqRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"_DeqOneMagRemove() [nFunRes=%d(ER_ORACLE_AQ_DEQUEUE_TIMEOUT)][nDeqRemoveIndex=%d][setDeqRemoveMsgId.size()=%d]", 
				nFunRes, nDeqRemoveIndex, setDeqRemoveMsgId.size());
			nContinueDeq = 0;
			nFunRes = ER_ORACLE_AQ_DEQUEUE_TIMEOUT;
			break;
		}
		else if (Err_DEQACkCode_NOTFIND == nDeqRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"_DeqOneMagRemove() [nFunRes=%d(Err_DEQACkCode_NOTFIND)][nDeqRemoveIndex=%d][setDeqRemoveMsgId.size()=%d]", 
				nFunRes, nDeqRemoveIndex, setDeqRemoveMsgId.size());
			nContinueDeq = 0;
			nFunRes = Err_DEQACkCode_NOTFIND;
			break;
		}
		else if (0 != nDeqRes)
		{
			//deq have error!
			nContinueDeq = 0;
			nFunRes = -1;
			break;
		}
		
		nDeqRemoveIndex++;
		if (nDeqRemoveIndex >= setDeqRemoveMsgId.size())
		{
			nContinueDeq = 0;
			nFunRes = 0;
		}
	
	}//while
	
	setDeqRemoveMsgId.clear();

	return nFunRes;		
}

int COracleConnection::_DeqMsgBrowser( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg )
{
	int nFunRes = 0;
	int nOneRowLen = 0;
	int nDeqRes = -1;
	int nContinueDeq = 1;
	int nDeqDataTotalLen = 0;
	setStrConT  setDeqGetMsgId;

	//check db con
	m_bDeqSet = true;
	
	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "_DeqMsgBrowser() false == isOpen()");
		nFunRes = -1;
		return nFunRes;
	}
	
	//check enqMsg input param
	if (0 != _CheckDeqMsgInputParam(strQName, pDeqMsg))
	{
		nFunRes = -1;
		return nFunRes;
	}	

	//deq handle init
	if (false == m_bDeqInit)
	{
		nFunRes = _DeqInit(strQName, pDeqMsg);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"error! _DeqInit()"); 
			nFunRes = -1;
			return nFunRes;
		}
		m_bDeqInit = true;
	}
	_GetSetDeqModeNav(pDeqMsg);

	nDeqDataTotalLen = 0;
	nContinueDeq = 1;
	while (nContinueDeq)
	{
		nOneRowLen = -1;
		nDeqRes = -1;

		// at.q_name, at.msg_sid, at.msgid, at.priority, at.user_data 
		nDeqRes = _DeqOneMagBrowser(setDeqGetMsgId, rLstMsg, nOneRowLen);

		if (ER_ORACLE_AQ_DEQUEUE_TIMEOUT == nDeqRes)
		{
			//there have no data for deq 
			nContinueDeq = 0;
			nFunRes = nDeqRes;
			break;
		}
		else if (ER_ORACLE_DequeueFailed == nDeqRes)
		{
			nContinueDeq = 0;
			nFunRes = nDeqRes;
			break;
		}
		else if (0 != nDeqRes)
		{
			//deq have error 
			//get error code 
			nContinueDeq = 0;
			nFunRes = -1;
			break;
		}


		if (rLstMsg.empty() || nOneRowLen <= 0)
		{
			//there have no data for deq 
			nContinueDeq = 0;
			nFunRes = 0;
			break;
		}

		//if get max data then stop deq
		nDeqDataTotalLen +=	nOneRowLen;
		if (nDeqDataTotalLen >= pDeqMsg->m_nDeqFetchSize)
		{
			nContinueDeq = 0;
			nFunRes = 0;
			break;
		}

	}
	setDeqGetMsgId.clear();
	
	return nFunRes;	
}

bool COracleConnection::listen( const std::string& strQName, u_int nSubID )
{
	bool nFunRes = 0;
	return nFunRes;	
}

bool COracleConnection::listen( const std::string& strQName, const std::string& strSubName )
{
	bool nFunRes = 0;
	return nFunRes;	
}


int COracleConnection::_DBCommit()
{
	int nFunRes = 0;
	int  nErrorCode = 0;

	if (NULL == m_connection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error ! OCI_Commit() NULL == m_connection");
		nFunRes = -1;
		return nFunRes;
	}

	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "aqCommit() false == isOpen()");
		aq_sleep(5);	 
		nFunRes = -1;
		return nFunRes;
	}

	//m_pEnqObjectHandle is inited use param: OCI_DURATION_TRANS
	//before commit, we need to free m_pEnqObjectHandle	
	if (0 != _FreeEnqObjetHandle())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "aqCommit() false == _FreeEnqObjetHandle()");
		nFunRes = -1;
		return nFunRes;	
	}

	nFunRes = OCI_Commit(m_connection);
	if (1 != nFunRes)
	{
		nFunRes = -1;
		nErrorCode = _LOGOracleErrMsg("error ! OCI_Commit() ");	
		aq_sleep(5);	
	}
	else
	{
		nFunRes = 0;
	}

	return nFunRes;	
}


int COracleConnection::aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair)
{
	int nFunRes = 0;
	
	nFunRes = _DBCommit();
	
	return nFunRes;	
}


int COracleConnection::aqCommit(const std::string& strQName, const std::string& strMsgID)
{
	int nFunRes = 0;

	nFunRes = _DBCommit();

	return nFunRes;	
	
}














//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

COracleConnection::COracleConnection( 
									 const std::string& strDbName, 
									 const std::string& strUserName, 
									 const std::string& strPwd, 
									 const std::string& strHost) : IDbConnection(
									 strDbName
									 ,strUserName
									 ,strPwd
									 ,strHost)
{		   	
	m_connection = NULL;
	m_bIsConnected = false;
	m_strDbConStr.clear();//SrcDbConStr = Oracle:TRA_OCC:IT271350_5:IT271350_5

	m_strAQDataTypeName.clear();
	m_pAQDataTypeInfo = NULL;
	m_mapAQDataTypeInfoColumn.clear();
	m_nLstAQDataTypeInfoColumCount = 0;
	m_nLogNmberCount = 0;
	//enq
	m_bEnqSet = false;
	m_bEnqInit = false;
	m_strEnqQueueName.clear();
	m_strEnqAgentArray.clear();
	m_lstEnqAgentArray.clear();
	m_ppEnqAgentArray = NULL;
	m_nEnqAgentArraySize = 0;
	m_pEnqEnqueueHandle = NULL;
	m_pEngMsgHandle = NULL;
	m_pEnqObjectHandle = NULL;	

	//deq
	m_bDeqSet = false;
	m_bDeqInit = false;
	m_pDeqDequeue = NULL;
	m_strDeqQueueName.clear();
	m_strDeqClientAgentName.clear();
	m_nDeqMode = OCI_ADM_REMOVE_NODATA;
	m_nDeqNav =	 OCI_ADN_FIRST_MSG;
	m_nDeqFetchSize = 0; //200kb  // the row of message for MySQL, the size of message for Oracle
	m_strDeqMsgID.clear();
	m_strDeqAgentArray.clear();
	m_lstDeqAgentArray.clear();
	m_ppDeqAgentArray = NULL;
	m_nDeqAgentArraySize = 0; 
	m_nCloseSet = 0;
	m_bBufferInitd = false;
	m_nBatchSize = 0;
	m_nBufferSQLSize = 0;				  	
	m_pBufferSQL = NULL;
	m_pStatement = NULL;

	_GetDbConnectionStr(strDbName, strUserName, strPwd, strHost, m_strDbConStr);
	  
	COracleEnv::getInstance();
	
	//if create connection failed , will throw CDBException 
	_CreateConnection(); 
	
	//fix memery problem
	//create connection ok, then creat memory, remmeber free memory in ~COracleConnection() 
	m_nBufferSQLSize = defMaxSQLStringLength;
	m_pBufferSQL = new char[m_nBufferSQLSize];
	memset(m_pBufferSQL, 0, m_nBufferSQLSize);

}

COracleConnection::~COracleConnection()
{
		
	if (isOpen())
	{
		for ( std::map<IQueryAdapter*, long>::iterator itLoop = m_mapActiveQuery.begin();
			m_mapActiveQuery.end() != itLoop; ++itLoop )
		{
			try
			{
				if ( NULL == itLoop->first )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
					continue;
				}

				if ( !itLoop->first->cleanQuery() )
				{
					delete (itLoop->first);
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "COcilibConnection: 0x%p is deleted by ConnectionReaper in destructor of OCIConnection", itLoop->first );
				}
				// else leave it to SimpleDb for deleting
			}
			catch (...)
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "cleanQuery: Unknown exception" );
			}
		}

		m_mapActiveQuery.clear();
	}

	// The connection is closed when the server is detached.
	try
	{
		close();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "FAILOVERDBLOG: connection closed in destruction function.");

	}	
	catch (...)
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "~COcilibConnection(): Unknown exception" );
	}

	m_bEnqSet = false;
	m_bDeqSet = false;

	
	if (m_bBufferInitd)
	{
		DEF_DELETE_ARRAY(m_ppBuffMessageQualifierValue);
		DEF_DELETE_ARRAY(m_ppBuffAuditDataOracleValue);
		DEF_DELETE_ARRAY(m_ppBuffAuditDataMySQLValue);
		m_bBufferInitd = false;
		m_nBatchSize = 0;
	}
	
	DEF_DELETE_ARRAY(m_pBufferSQL);
	m_nBufferSQLSize = 0;
	
	m_pStatement = NULL;


	
}

bool COracleConnection::isAnyQueryActive()
{
	FUNCTION_ENTRY("isAnyQueryActive");
	bool bFunRes = true;

	for ( std::map<IQueryAdapter*, long>::iterator itLoop = m_mapActiveQuery.begin(); 
		m_mapActiveQuery.end() != itLoop; ++itLoop )
	{
		if ( NULL == itLoop->first )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
			continue;
		}

		if ( (itLoop->first)->isActive())
		{
			FUNCTION_EXIT;
			bFunRes = true;
			return bFunRes;
		}
	}

	FUNCTION_EXIT;
	bFunRes = false;
	return bFunRes;
}

void COracleConnection::open()
{
	FUNCTION_ENTRY("open");
	
	bool nConnected = false;
	int  nFunRes = 0;

	nConnected = isOpen();
	if (true == nConnected)
	{
		nFunRes = 0;
		return;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[isOpen()==false]");  			
		
		_OracleAQUnInit();

		//when progame in runing  sereer lost connect with DB
		_CreateConnection();
	}


	FUNCTION_EXIT;
	return;
}

bool COracleConnection::isOpen()
{
    bool bFunRes = false;
	int nOCIFunRes = 0; 

	//FUNCTION_ENTRY("isOpen");
	
	if (NULL == m_connection)
	{
		LOG0(SourceInfo, DebugUtil::DebugWarn,  "connection is NULL!");
	
		//FUNCTION_EXIT;
		bFunRes = false;
		return bFunRes;
	}
	   
	nOCIFunRes = OCI_IsConnected(m_connection);
	
	if (1 == nOCIFunRes)
	{
		m_bIsConnected = true;
		bFunRes = true;	
	}	
	else
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database is closed");	

		m_bIsConnected = false;
		bFunRes = false;	
	}

	
	//FUNCTION_EXIT;
	return bFunRes;	
}

void COracleConnection::close()
{
	int nFunRes = 0;
	int nOCIFunRes = 0;
	
	FUNCTION_ENTRY("close");
	m_nCloseSet = 1;
	if (NULL == m_connection)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "connection is NULL!");
		FUNCTION_EXIT;
		nFunRes = -1;
		return;
	}

	nOCIFunRes = OCI_IsConnected(m_connection);

	if (0 == nOCIFunRes)
	{
		_OracleAQUnInit();
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
			"COcilibConnection::close(): Database is already closed" );
			
		m_bIsConnected = false;
		//FUNCTION_EXIT;
		nFunRes = -1;
		//return;
	}
	else
	{
		m_bIsConnected = true;
		_OracleAQUnInit();
	}
	
	if (NULL != m_connection)
	{
		/* disconnect from DB */
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"OCI_ConnectionFree begin [strSchema=%s][UserName=%s][strPwd=%s]",
			m_strSchema.c_str(), m_strUserName.c_str(), m_strPwd.c_str());

		if (NULL != m_pStatement)
		{
			OCI_StatementFree(m_pStatement);
			m_pStatement = NULL;
		}
		nOCIFunRes = OCI_ConnectionFree(m_connection);
		m_connection = NULL;

		if (1 != nOCIFunRes)
		{
			nFunRes = -1;
			_LOGOracleErrMsg("OCI_ConnectionFree() error!", 1);	
		}
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"OCI_ConnectionFree end [Schema=%s][UserName=%s][Pwd=%s]",
			m_strSchema.c_str(), m_strUserName.c_str(), m_strPwd.c_str());

	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"FAILOVERDBLOG: connection closed in close() function.");

	FUNCTION_EXIT;
	return;
}

void COracleConnection::closeAll()
{
	FUNCTION_ENTRY("closeAll");

	close();
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"FAILOVERDBLOG: connection closed in closeAll() function.");
		
	FUNCTION_EXIT;
	return;
}

void COracleConnection::setAutoCommit( bool bAutoCommit )
{
    return;
}

bool COracleConnection::getAutoCommit()
{
	bool bFunRes = true;

	return bFunRes;
}

void COracleConnection::rollback()
{
	int nFunRes = 0;
	int  nErrorCode = 0;

	if (NULL == m_connection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error ! rollback() NULL == m_connection");
		nFunRes = -1;
		return;
	}

	if (false == isOpen())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "rollback() false == isOpen()");
		nFunRes = -1;
		return;
	}

	
	nFunRes = OCI_Rollback(m_connection); 
	if (1 != nFunRes)
	{
		_LOGOracleErrMsg("error ! OCI_Rollback() ");
		nFunRes = -1;	
	}
	else
	{
		nFunRes = 0;
	}

	return;
}

void COracleConnection::commit()
{
	int nFunRes = 0;

	nFunRes = _DBCommit();

	return;
}

void COracleConnection::exec( const std::string& statement, IQueryAdapter*& pQueryAdapter, bool isQuery /*= false*/, int prefetch /*= 10 */ )
{ 
	OCI_Statement* pStatement = NULL;
	int            nOCIFunRes = 0;

	FUNCTION_ENTRY("exec");

	if (NULL == m_connection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"COracleConnection::exec() connection is NULL!");
		FUNCTION_EXIT;
		return;
	}
	
	// If the database is not already open, open it.
	if (!isOpen())
	{
		open();
	}
	
	//check the db connection is connected
	if (!isOpen())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"COracleConnection::exec() [false == isOpen()]");
		return;
	}

	if ( NULL != pQueryAdapter )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "pQueryAdapter should not be other value except NULL" );
		cleanQuery( pQueryAdapter );
	}
	
	pStatement = OCI_StatementCreate(m_connection);
	if (NULL == pStatement)
	{
		_LOGOracleErrMsg("OCI_StatementCreate() error!", 1);	 
		FUNCTION_EXIT;
		return;	
	}

	LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugSQL, 
			"OCI_ExecuteStmt [strSQL=%s]", statement.c_str());


	nOCIFunRes = OCI_ExecuteStmt(pStatement, statement.c_str());	
	if (1 != nOCIFunRes)
	{
		_LOGOracleErrMsg("OCI_ExecuteStmt() error!", 1);	 

		nOCIFunRes = OCI_StatementFree(pStatement); 
		pStatement = NULL;

		if (1 != nOCIFunRes)
		{
			_LOGOracleErrMsg("OCI_StatementFree() error!", 1);	 
		}
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"error! OCI_ExecuteStmt() statement=[%s]", statement.c_str());

		FUNCTION_EXIT;
		return;
	}

	if (isQuery) //select *
	{
		//do nothing
	}
	else //like the insert command
	{
		nOCIFunRes = OCI_StatementFree(pStatement); 
		pStatement = NULL;
		if (1 != nOCIFunRes)
		{
			_LOGOracleErrMsg("OCI_StatementFree() error!", 1);	 
		} 
	}



	//just save the pointer, here
	if ( NULL != pStatement )
	{
		pQueryAdapter = new COracleQueryAdapter(pStatement);
		m_mapActiveQuery[pQueryAdapter] = QUERY_EXIST;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
			"COracleQueryAdapter: 0x%p is created for connection: 0x%p", pQueryAdapter, this );
	}

	FUNCTION_EXIT;
}

void COracleConnection::fetchBLOB( const std::string &strSQL, const std::string &colomnName, DataClobArray& data )
{
	FUNCTION_ENTRY("fetchBLOB");

	

	FUNCTION_EXIT;
}

void COracleConnection::updateBLOB( const std::string &strTableName, const std::string &colomnName, const std::vector<unsigned char>& data, const std::string &strWhereClause )
{
	FUNCTION_ENTRY("updateBLOB");


	FUNCTION_EXIT;
}

void COracleConnection::cleanQuery( IQueryAdapter*& pQuery )
{
	FUNCTION_ENTRY("cleanQuery");

	if ( NULL == pQuery )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid input parameter: NULL pointer of OCIQueryAdapter" );
		return;
	}
	
	if (!isOpen())
	{
		return;
	}
		  
	std::map<IQueryAdapter*, long>::iterator itExistQuery = m_mapActiveQuery.find( pQuery );
	if ( m_mapActiveQuery.end() != itExistQuery )
	{
		if ( NULL == itExistQuery->first )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
			return;
		}


		// Destructor will clean query
		delete (itExistQuery->first);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is deleted by SimpleDb", pQuery );

		m_mapActiveQuery.erase( itExistQuery );
	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Specified query is not found in this connection, the old connection has been reaped" );
		if ( !pQuery->cleanQuery() )
		{
			delete pQuery;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is deleted by SimpleDb through it has been reaped", pQuery );
		}
		// else leave it to ConnectionReaper for deleting
	}

	// Set pointer to NULL
	pQuery = NULL;

	FUNCTION_EXIT;
}


int  COracleConnection::_FreeMapColumNameType(mapColumNTConT& mapColumNameType)
{
	int nFunRes = 0; 	
	mapColumNTConIterT  iterMap;
	PColumnNameType	pColumNT = NULL;
	
	iterMap = mapColumNameType.begin();
	while (iterMap != mapColumNameType.end())
	{
		pColumNT = iterMap->second;			
		DEF_DELETE(pColumNT);
		iterMap++;
	}
	
	mapColumNameType.clear();
	
	return nFunRes;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



NS_END(TA_AQ_Lib)









