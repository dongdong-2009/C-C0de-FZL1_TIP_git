//////////////////////////////////////////////////////////////////////
///		@file		MAQCmd.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-29 11:44:41
///
///		@brief	    implementation of class CMAQCmd. 
///                 
//////////////////////////////////////////////////////////////////////

#include <algorithm> 
#include "DbCommonData.h"
#include "MAQCmd.h"
#include "DBException.h"
#include "Message.h"
#include "UtilityFun.h"

#if defined(_WIN32)
#include <Windows.h>
#define aq_sleep     Sleep
#define sql_snprintf _snprintf
#else
#include <unistd.h>
#define aq_sleep     sleep
#define sql_snprintf snprintf
#endif

#include "driver/mysql_public_iface.h"
#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_AQ_Lib)

static const std::string   s_strEnqSQLFormat("CALL prc_enqueue('%s', '%s', '%s', '%s', '%s',  %d, '%s')");

static const std::string   s_strDeqDataRemoteSQLFormat1("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM  %s \
qtab, %s qitab WHERE qtab.msgid = qitab.msgid and qitab.sub_id = %d \
AND qitab.state=1 ORDER BY qtab.msg_sid ASC LIMIT %d");

static const std::string   s_strDeqDataRemoteSQLFormat2("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM  %s \
qtab join (select msg_sid from %s where sub_id = %d AND state <= 2 order by msg_sid asc limit %d) \
as tmp on qtab.msg_sid = tmp.msg_sid order by qtab.msg_sid asc");			
														
static const std::string   s_strDeqDataRemoteSQLFormat3("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM  %s \
qtab WHERE qtab.state <= 2 AND qtab.sub_cnt <> 0 ORDER BY qtab.msg_sid ASC LIMIT %d");

static const std::string   s_strDeqDataLocalSQLFormat1("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM %s \
qtab, %s qitab WHERE qtab.msgid = qitab.msgid and qitab.sub_name = UPPER('%s') \
AND qitab.state=1 ORDER BY qtab.msg_sid ASC LIMIT %d");

static const std::string   s_strDeqDataLocalSQLFormat2("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM  %s \
qtab join (select msg_sid from %s where sub_name = %s AND state <= 2 order by msg_sid asc limit %d) \
as tmp on qtab.msg_sid = tmp.msg_sid order by qtab.msg_sid asc");

static const std::string   s_strDeqDataLocalSQLFormat3("SELECT qtab.q_name, qtab.msg_sid, qtab.msgid, qtab.priority, qtab.user_data_1, qtab.user_data_2, qtab.user_data_3 FROM %s \
qtab WHERE qtab.state <= 2 ORDER BY qtab.msg_sid ASC LIMIT %d");

static const std::string   s_strDeqCommitSQLFormatMsgID("CALL prc_deqcommit_msgid('%s', '%s')");
static const std::string   s_strListenSQLFormatSubID("SELECT MSGID FROM %s WHERE SUB_ID = %u AND STATE = 1 LIMIT 1");
static const std::string   s_strListenSQLFormatSubName("SELECT MSGID FROM %s WHERE SUB_NAME = UPPER('%s') AND STATE = 1 LIMIT 1");
static const std::string   s_strListenSQLFormatMsgID("SELECT 1 AS NUM FROM %s WHERE MSGID = '%s'");
static const std::string   s_strQSubCntSQLFormat("SELECT SUB_COUNT FROM AQ_SYS_QUEUE_M WHERE Q_NAME = UPPER('%s')");
static const std::string   s_strDBSrvID("SELECT @@server_id as srv_id from dual");

static const std::string   s_strGetExcepDataRemote("SELECT MSG_SID from %s WHERE SUB_ID = %u AND MSGID = '%s'");
static const std::string   s_strGetExcepDataLocal("SELECT MSG_SID from %s WHERE UPPER(SUB_NAME) = UPPER('%s') AND msgid = '%s'");
static const std::string   s_strRemoveExceptionDataRemote("DELETE FROM %s WHERE SUB_ID = %u AND MSGID = '%s' AND STATE = 1");
static const std::string   s_strRemoveExceptionDataLocal("DELETE FROM %s WHERE UPPER(SUB_NAME) = UPPER('%s') AND MSGID = '%s' AND STATE = 1");

#if defined(_WIN32)
static const std::string   s_strDeqCommitSQLFormatSID("CALL prc_deqcommit_sid('%s', %d, '%s', %d, %I64u, %I64u)");
static const std::string   s_strRecordExcepData("INSERT INTO AQ_SYS_QUEUE_E(Q_NAME, MSG_SID, MSGID, ENQ_TIME) VALUES ('%s', %I64u, '%s', SYSDATE())");
#else
static const std::string   s_strDeqCommitSQLFormatSID("CALL prc_deqcommit_sid('%s', %d, '%s', %d, %llu, %llu)");
static const std::string   s_strRecordExcepData("INSERT INTO AQ_SYS_QUEUE_E(Q_NAME, MSG_SID, MSGID, ENQ_TIME) VALUES ('%s', %llu, '%s', SYSDATE())");
#endif


CMAQCmd::CMAQCmd(sql::Connection* pConnection)
: m_pConnection(pConnection)
, m_ulTimeOut(MSG_LISTEN_NOWAIT)
, m_nErrCount(0)
{

	if ( m_pConnection )
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}
}

CMAQCmd::~CMAQCmd()
{
	m_pConnection = NULL;
}

int CMAQCmd::mAQEnq(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt)
{
	FUNCTION_ENTRY("mAQEnq");	

	int  nRetCode   = Ret_Success;
	char*  pszSQL   = NULL;
	char*  pEscapedPayload_2 = NULL;
	char*  pEscapedPayload_3 = NULL;
	uint32 nSQLSize = 0;
	bool bIsOk = false;
	uint32  uTryTime = 0;
	if (!m_pConnection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW( CDBException("MysqlConnection_ERROR", "Mysql connection is null", 0) );
	}	
		
	std::string strQName, strMsgID, strRecipLst;
	const char* pcPayload_1 = NULL;
	const char* pcPayload_2 = NULL;
	const char* pcPayload_3 = NULL;
	int nSubCnt = pEnqMsg->getSubCnt();
	strQName    = pEnqMsg->getQName();
	strRecipLst = pEnqMsg->getRecipientList();

	nRetCode = pEnqMsg->getPayload(defPayload_1_Tag, pcPayload_1);
	nRetCode = pEnqMsg->getPayload(defPayload_2_Tag, pcPayload_2);
	nRetCode = pEnqMsg->getPayload(defPayload_3_Tag, pcPayload_3);

	pEscapedPayload_2 = CUtilityFun::escapeSQLStr((char*)pcPayload_2, "'", "''");
	pEscapedPayload_3 = CUtilityFun::escapeSQLStr((char*)pcPayload_3, "'", "''");

	nSQLSize += s_strEnqSQLFormat.size();
	nSQLSize += strQName.size();
	nSQLSize += strMsgID.size();
	nSQLSize += strlen(pcPayload_1);
	nSQLSize += strlen(pcPayload_2);
	nSQLSize += strlen(pcPayload_3);
	nSQLSize += strRecipLst.size();
	nSQLSize += defMaxNameLen;

	pszSQL = new char[nSQLSize + 1];
	memset(pszSQL, 0, nSQLSize+1);
	sql_snprintf(pszSQL, nSQLSize, s_strEnqSQLFormat.c_str(), strQName.c_str(), pcPayload_1, pEscapedPayload_2, 
				 pEscapedPayload_3, strMsgID.c_str(), nSubCnt, strRecipLst.c_str());

	free(pEscapedPayload_2); pEscapedPayload_2 = 0;
	free(pEscapedPayload_3); pEscapedPayload_3 = 0;

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
		"mAQEnq begin m_pStatement->execute( pszSQL )[pszSQL=%s]", pszSQL);

	/*std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());*/
	
	while (!bIsOk && uTryTime < MAX_RETRY_TIMES)
	{
		try
		{		
			bool bRetCode = m_pStatement->execute( pszSQL );
			if(bRetCode)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "the statement is not a insert like statement" );
			}
			bIsOk = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"mAQEnq end [bRetCode=%d]m_pStatement->execute( pszSQL )[pszSQL=%s]", bRetCode, pszSQL);

		}
		catch (sql::SQLException& e)
		{
			uTryTime++;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to execute statement: %s", pszSQL);
			if (uTryTime < MAX_RETRY_TIMES)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MySQL_ERROR Code: %d, Error Message: %s", 
					e.getErrorCode(), e.what());
			}
			else
			{
				CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
				DEF_DELETE_ARRAY(pszSQL);
				throw db;
			}			
		}
		catch (...)
		{
			uTryTime++;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to execute statement: %s", pszSQL);
			if (uTryTime < MAX_RETRY_TIMES)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MySQL_ERROR Message: %s", "unknown database exception");
			}
			else
			{
				CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
				DEF_DELETE_ARRAY(pszSQL);
				throw db;
			}
		}
	}	

	DEF_DELETE_ARRAY(pszSQL);
	FUNCTION_EXIT;
	return nRetCode;
}

int CMAQCmd::mAQDeq(const std::string& strQName, PMAQ_DEQ_OPT pDeqOpts, LstMsgConT& rLstMsg)
{
	FUNCTION_ENTRY("mAQDeq");
	int  nRetCode   = Ret_Success;
	char*  pszSQL   = NULL;
	uint32 nSubCnt  = 0;

	if (!m_pConnection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW( CDBException("MysqlConnection_ERROR", "Mysql connection is null", 0) );
	}

	std::string strQTable = strQName + "_t";
	std::string strITable = ("aq_" + strQName + "_t_i");
	pszSQL = new char[defMaxSQLLen + 1];
	memset(pszSQL, 0, defMaxSQLLen + 1);

	sql::ResultSet* pQuery = NULL;	
	try
	{		

		if ( pDeqOpts->m_nSubID > 0 )
		{
			nSubCnt = _CheckSubCnt(strQName);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"[pDeqOpts->m_nSubID=%d] > 0 [nSubCnt=%d] = _CheckSubCnt([strQName=%s])", 
				pDeqOpts->m_nSubID, nSubCnt, strQName.c_str());

			if (1 == nSubCnt)
			{
				sql_snprintf(pszSQL, defMaxSQLLen, s_strDeqDataRemoteSQLFormat3.c_str(), strQTable.c_str(), pDeqOpts->m_nDeqFetchSize );
			}
			else
			{													
				sql_snprintf(pszSQL, defMaxSQLLen, s_strDeqDataRemoteSQLFormat2.c_str(), 
							strQTable.c_str(), strITable.c_str(), pDeqOpts->m_nSubID, pDeqOpts->m_nDeqFetchSize);
																			
			}			

		}
		else if (0 == pDeqOpts->m_nSubID && !pDeqOpts->m_strConsumerName.empty())
		{	
		
			nSubCnt = _CheckSubCnt(strQName);
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"[pDeqOpts->m_nSubID=%d] <=0 [nSubCnt=%d] = _CheckSubCnt([strQName=%s]) [pDeqOpts->m_strConsumerName=%s]", 
				pDeqOpts->m_nSubID, nSubCnt, strQName.c_str(), pDeqOpts->m_strConsumerName.c_str());

			if (1 == nSubCnt)
			{
				sql_snprintf(pszSQL, defMaxSQLLen, s_strDeqDataLocalSQLFormat3.c_str(), strQTable.c_str(), pDeqOpts->m_nDeqFetchSize );
			}
			else
			{
				sql_snprintf(pszSQL, defMaxSQLLen, s_strDeqDataLocalSQLFormat2.c_str(), strQTable.c_str(), strITable.c_str(), 
					CUtilityFun::strtoupper((char*)pDeqOpts->m_strConsumerName.c_str()), pDeqOpts->m_nDeqFetchSize );
			}
			
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_DeqOption_Invalid, defErrMsg(Err_DeqOption_Invalid));
			nRetCode = Ret_Failed;
			return nRetCode;
		}	

		if (NULL == m_pStatement.get())
		{
			m_pStatement.reset(m_pConnection->createStatement());
		}
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,"mAQDeq Begin execute Deq SQL: %s", pszSQL);
		
		//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());
		pQuery = m_pStatement->executeQuery( pszSQL );

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,"mAQDeq End execute Deq SQL: %s", pszSQL);
		if (pQuery)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,"mAQDeq Begin fetch Deq data...");
			nRetCode = _FetchData(pQuery, pDeqOpts, rLstMsg);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
			nRetCode = Ret_Failed;
		}
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,"mAQDeq End fetch Deq data, size: %ld", rLstMsg.size());		

	}
	catch (sql::SQLException& e)
	{	
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		if (pQuery)
			pQuery->close();
		DEF_DELETE(pQuery);
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}
	catch (...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", Ret_Failed);
		if (pQuery)
			pQuery->close();
		DEF_DELETE(pQuery);
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}
	if ( pQuery )
		pQuery->close();
	DEF_DELETE(pQuery);
	DEF_DELETE_ARRAY(pszSQL);
	FUNCTION_EXIT;
	return nRetCode;
}

void CMAQCmd::mAQDeqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair)
{
	FUNCTION_ENTRY("mAQDeqCommit");
	char*  pszSQL   = NULL;
	uint32 nSQLSize = 0;

	if (NULL == m_pConnection)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW( CDBException("MysqlConnection_ERROR", "Mysql connection is null", 0) );
	}
	try
	{
		nSQLSize += s_strDeqCommitSQLFormatSID.size();
		nSQLSize += strQName.size();
		nSQLSize += strConsumer.size();
		nSQLSize += (DEF_64BIT_INT_LEN * 3);

		pszSQL = new char[nSQLSize + 1];
		memset(pszSQL, 0, nSQLSize+1);
		sql_snprintf(pszSQL, nSQLSize, s_strDeqCommitSQLFormatSID.c_str(), strQName.c_str(), nSubID, strConsumer.c_str(), 1, rSeqIDPair.first,  rSeqIDPair.second);

		if (NULL == m_pStatement.get())
		{
			m_pStatement.reset(m_pConnection->createStatement());
		}
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
			"mAQDeqCommit begin m_pStatement->execute( pszSQL )[pszSQL=%s]", pszSQL);

		//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());
		bool bRetCode = m_pStatement->execute( pszSQL );
		if(bRetCode)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "the statement is not a insert like statement" );
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
			"mAQDeqCommit end [bRetCode=%d] m_pStatement->execute( pszSQL )[pszSQL=%s]", bRetCode, pszSQL);


	}
	catch (sql::SQLException& e)
	{	
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}
	catch (...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}

	DEF_DELETE_ARRAY(pszSQL);
	FUNCTION_EXIT;
}

void CMAQCmd::mAQDeqCommit(const std::string& strQName, const std::string& strMsgID)
{
	FUNCTION_ENTRY("mAQDeqCommit");
	char*  pszSQL   = NULL;
	uint32 nSQLSize = 0;

	if (!m_pConnection )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW( CDBException("MysqlConnection_ERROR", "Mysql connection is null", 0) );
	}
	try
	{
		nSQLSize += s_strDeqCommitSQLFormatMsgID.size();
		nSQLSize += strQName.size();
		nSQLSize += strMsgID.size();
		nSQLSize += (DEF_64BIT_INT_LEN * 2);

		pszSQL = new char[nSQLSize + 1];
		memset(pszSQL, 0, nSQLSize+1);
		sql_snprintf(pszSQL, nSQLSize, s_strDeqCommitSQLFormatMsgID.c_str(), strQName.c_str(), strMsgID.c_str());

		if (NULL == m_pStatement.get())
		{
			m_pStatement.reset(m_pConnection->createStatement());
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
			"mAQDeqCommit begin m_pStatement->execute( pszSQL )[pszSQL=%s]", pszSQL);

		//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());
		bool bRetCode = m_pStatement->execute(pszSQL);
		if(bRetCode)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "the statement is not a insert like statement" );
		}

	}
	catch (sql::SQLException& e)
	{	
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}
	catch (...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		DEF_DELETE_ARRAY(pszSQL);
		throw db;
	}

	DEF_DELETE_ARRAY(pszSQL);
	FUNCTION_EXIT;
}

bool  CMAQCmd::mAQListen(const std::string& strQName, u_int  nSubID)
{
	bool  bRet = false;
	static u_long s_ulLogPrint = 0;
	TA_ASSERT(NULL != m_pConnection, defErrMsg(Err_Invalid_Ptr));	

	std::string strDisTable("aq_" + strQName + "_t_i");
	std::string strQTable(strQName + "_t");
	std::string strMsgID;

	sql::ResultSet* pQuery = NULL;;
	u_long   ulCount       = 0;
	u_long   ulMsgPrint    = 0;
	uint32   uSQLSize      = 0;
	char*    pszSQLSubID   = NULL;
	char*    pszSQLMsgID   = NULL;
	char*    pszRemvErr    = NULL;

	uSQLSize += s_strListenSQLFormatSubID.size();
	uSQLSize += strDisTable.size();
	uSQLSize += 12;
	pszSQLSubID = new char[uSQLSize + 1];
	memset(pszSQLSubID, 0, uSQLSize + 1);

	sql_snprintf(pszSQLSubID, uSQLSize, s_strListenSQLFormatSubID.c_str(), strDisTable.c_str(), nSubID);

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}
	//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());

	try
	{	
		while ( !bRet )
		{		
			pQuery = m_pStatement->executeQuery( pszSQLSubID );
			if (!pQuery) return false;

			ulCount = pQuery->rowsCount();
			if ( ulCount > 0 )
			{
				pQuery->first();
				strMsgID = pQuery->getString(1);
				uSQLSize = s_strListenSQLFormatMsgID.size();
				uSQLSize += strQTable.size();
				uSQLSize += strMsgID.size();
				pszSQLMsgID = new char[uSQLSize + 1];
				memset(pszSQLMsgID, 0, uSQLSize + 1);

				sql_snprintf(pszSQLMsgID, uSQLSize, s_strListenSQLFormatMsgID.c_str(), strQTable.c_str(), strMsgID.c_str());
				
				if ( pQuery )
				{
					pQuery->close();
					DEF_DELETE(pQuery);
				}
				ulCount = 0;				
				if (0 == (ulMsgPrint % defDefaultMsgPrintTimes))
				{
					ulMsgPrint = 0;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
						"mAQListen m_pStatement->executeQuery( pszSQLSubID )[pszSQLMsgID=%s]", pszSQLMsgID);

				}
				ulMsgPrint++;

				pQuery = m_pStatement->executeQuery( pszSQLMsgID );

				if( pQuery )
					ulCount = pQuery->rowsCount();
				if (ulCount > 0)
				{
					pQuery->close();
					DEF_DELETE(pQuery);
					bRet = true;			
					break;
				}
				else
				{
					m_nErrCount++;
					if (m_nErrCount > MAX_RETRY_TIMES)
					{
						_ProcessExcepData(strQName, strMsgID, nSubID);

						uSQLSize = s_strRemoveExceptionDataRemote.size();
						uSQLSize += strDisTable.size();
						uSQLSize += strMsgID.size();
						uSQLSize += defMaxNameLen;
						pszRemvErr = new char[uSQLSize + 1];
						memset(pszRemvErr, 0, uSQLSize + 1);	

						sql_snprintf(pszRemvErr, uSQLSize, s_strRemoveExceptionDataRemote.c_str(), strDisTable.c_str(), nSubID, strMsgID.c_str());
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
							"_ProcessExcepData m_pStatement->execute( pszRemvErr )[pszRemvErr=%s]", pszRemvErr);
						m_pStatement->execute( pszRemvErr );

						m_nErrCount = 0;
						DEF_DELETE_ARRAY(pszRemvErr);
					}	
					else
					{
						//if (s_ulLogPrint > defDefaultMsgPrintTimes || ulMsgPrint > defDefaultMsgPrintTimes)
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"Error Code: %d, Error Message: %s", Err_QData_NotFound, defErrMsg(Err_QData_NotFound));	
					}								
				}
			}
			if ( pQuery )
				pQuery->close();
			DEF_DELETE(pQuery);

			if (MSG_LISTEN_FOREVER == m_ulTimeOut)
			{
				aq_sleep(1);
				continue;
			}
			else if (MSG_LISTEN_NOWAIT == m_ulTimeOut)
			{
				s_ulLogPrint++;
				if (s_ulLogPrint > defDefaultMsgPrintTimes)
				{
					s_ulLogPrint = 0;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, 
						"listening for SubID: %u", nSubID);
				}
				break;
			}
			else
			{
				if (ulMsgPrint > m_ulTimeOut) break;
				else continue;
			}
		}
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszSQLSubID);
	}
	catch (sql::SQLException& e)
	{	
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		DEF_DELETE_ARRAY(pszSQLSubID);
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszRemvErr);
		throw db;
	}
	catch (...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		DEF_DELETE_ARRAY(pszSQLSubID);
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszRemvErr);
		throw db;
	}

	return bRet;
}

bool  CMAQCmd::mAQListen(const std::string& strQName, const std::string& strSubName)
{
	bool  bRet = false;
	static u_long s_ulLogPrint = 0;
	TA_ASSERT(m_pConnection, defErrMsg(Err_Invalid_Ptr));	

	std::string strDisTable("aq_" + strQName + "_t_i");
	std::string strQTable(strQName + "_t");
	std::string strMsgID;

	sql::ResultSet* pQuery  = NULL;
	u_long   ulCount        = 0;
	u_long   ulMsgPrint     = 0;
	uint32   uSQLSize       = 0;
	char*    pszSQLSubName  = NULL;
	char*    pszSQLMsgID    = NULL;
	char*    pszRemvErr    = NULL;

	uSQLSize += s_strListenSQLFormatSubName.size();
	uSQLSize += strDisTable.size();
	uSQLSize += 12;
	pszSQLSubName = new char[uSQLSize + 1];
	memset(pszSQLSubName, 0, uSQLSize + 1);

	sql_snprintf(pszSQLSubName, uSQLSize, s_strListenSQLFormatSubName.c_str(), strDisTable.c_str(), strSubName.c_str());

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}

	//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());

	try
	{
		while ( !bRet )
		{
			ulMsgPrint++;
			if (ulMsgPrint > defDefaultMsgPrintTimes)
			{
				ulMsgPrint = 0;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"listening for Subscriber: %s", strSubName.c_str());
			}

			ulCount = 0;
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"mAQListen m_pStatement->executeQuery( pszSQLSubID )[pszSQLSubName=%s]", pszSQLSubName);

			pQuery = m_pStatement->executeQuery( pszSQLSubName );
			if ( pQuery )
				ulCount = pQuery->rowsCount();
			if ( ulCount > 0 )
			{
				pQuery->first();
				strMsgID = pQuery->getString(1);
				uSQLSize = s_strListenSQLFormatMsgID.size();
				uSQLSize += strQTable.size();
				uSQLSize += strMsgID.size();
				pszSQLMsgID = new char[uSQLSize + 1];
				memset(pszSQLMsgID, 0, uSQLSize + 1);

				sql_snprintf(pszSQLMsgID, uSQLSize, s_strListenSQLFormatMsgID.c_str(), strQTable.c_str(), strMsgID.c_str());
				if ( pQuery )
					pQuery->close();
				ulCount = 0;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
					"mAQListen m_pStatement->executeQuery( pszSQLSubID )[pszSQLMsgID=%s]", pszSQLMsgID);

				pQuery = m_pStatement->executeQuery( pszSQLMsgID );
				if ( pQuery )
					ulCount = pQuery->rowsCount();
				if (ulCount > 0)
				{
					pQuery->close();
					DEF_DELETE(pQuery);
					bRet = true;			
					break;
				}
				else
				{
					m_nErrCount++;
					if (m_nErrCount > MAX_RETRY_TIMES)
					{
						_ProcessExcepData(strQName, strMsgID, 0, strSubName.c_str());

						uSQLSize = s_strRemoveExceptionDataLocal.size();
						uSQLSize += strDisTable.size();
						uSQLSize += strMsgID.size();
						uSQLSize += defMaxNameLen;
						pszRemvErr = new char[uSQLSize + 1];
						memset(pszRemvErr, 0, uSQLSize + 1);	

						sql_snprintf(pszRemvErr, uSQLSize, s_strRemoveExceptionDataLocal.c_str(), strDisTable.c_str(), strSubName.c_str(), strMsgID.c_str());
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
							"_ProcessExcepData m_pStatement->execute( pszRemvErr )[pszRemvErr=%s]", pszRemvErr);
						m_pStatement->execute( pszRemvErr );

						m_nErrCount = 0;
						DEF_DELETE_ARRAY(pszRemvErr);
					}
					else
					{
						//	if (s_ulLogPrint > defDefaultMsgPrintTimes || ulMsgPrint > defDefaultMsgPrintTimes)
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"Error Code: %d, Error Message: %s", Err_QData_NotFound, defErrMsg(Err_QData_NotFound));
					}				
				}
			}
			if ( pQuery )
				pQuery->close();
			DEF_DELETE(pQuery);						

			if (MSG_LISTEN_FOREVER == m_ulTimeOut)
			{
				aq_sleep(1);
				continue;
			}
			else if (MSG_LISTEN_NOWAIT == m_ulTimeOut)
			{
				s_ulLogPrint++;
				if (s_ulLogPrint > defDefaultMsgPrintTimes)
				{
					s_ulLogPrint = 0;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"listening for Subscriber: %s", strSubName.c_str());
				}
				break;
			}
			else
			{
				if (ulMsgPrint > m_ulTimeOut) break;
				else continue;
			}
		}
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszSQLSubName);
	}
	catch (sql::SQLException& e)
	{	
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		DEF_DELETE_ARRAY(pszSQLSubName);
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszRemvErr);
		throw db;
	}
	catch (...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		DEF_DELETE_ARRAY(pszSQLSubName);
		DEF_DELETE_ARRAY(pszSQLMsgID);
		DEF_DELETE_ARRAY(pszRemvErr);
		throw db;
	}

	return bRet;
}

void   CMAQCmd::setConnection(sql::Connection* pConnection)
{
	if (!pConnection) return;

	m_pConnection = pConnection;
	m_pStatement.reset(m_pConnection->createStatement());
}

int CMAQCmd::mAQGetSrvID()
{
	int  nRetCode = 0;	
	TA_ASSERT(NULL != m_pConnection, defErrMsg(Err_Invalid_Ptr));
	sql::ResultSet* pQuery  = NULL;
	size_t ulCount = 0;

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}

	pQuery = m_pStatement->executeQuery(s_strDBSrvID);
	if ( pQuery )
		ulCount = pQuery->rowsCount();
	if (ulCount > 0)
	{
		pQuery->first();
		nRetCode = pQuery->getInt(1);
	}
	if ( pQuery )
		pQuery->close();
	DEF_DELETE(pQuery);

	return nRetCode;
}

int  CMAQCmd::_FetchData(sql::ResultSet* pResSet, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg)
{
	FUNCTION_ENTRY("_FetchData");
	int RetCode = Ret_Success;

	LstDataBuffT  lstDBDataBuff;
	RetCode = _FetchData(pResSet, lstDBDataBuff);
	DEF_ERROR_CHECK(!RetCode);
	RetCode = _FormatDBBuff2Msg(pDeqMsg, lstDBDataBuff, rLstMsg);
	DEF_ERROR_CHECK(!RetCode);
	_FreeDBDataBuffer(lstDBDataBuff);

	FUNCTION_EXIT;
	return RetCode;
}

int  CMAQCmd::_FetchData(sql::ResultSet* pResSet, LstDataBuffT& rLstData)
{
	FUNCTION_ENTRY("_FetchData");
	int RetCode = Ret_Success;
	DEF_ERROR_CHECK(pResSet);

	rLstData.clear();
	if ( pResSet->rowsCount() > 0 )
	{
		pResSet->first();
		sql::ResultSetMetaData* pRsMetaData = pResSet->getMetaData() ; // use of auto_prt, need not delete
		TA_ASSERT(NULL != pRsMetaData, "get result set MetaData pointer failed");
		int nFieldNumber = pRsMetaData->getColumnCount();

		while( !pResSet->isAfterLast() )
		{
			RowT* pDataRow = new RowT();
			for ( int nColLoop = 0; nColLoop < nFieldNumber; ++nColLoop )
			{
				std::string strFieldName = pRsMetaData->getColumnName(nColLoop + 1);  // 1-based 
				std::string strValue = pResSet->getString(nColLoop + 1);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
					"SQL field name:[%s]; value:[%s]", strFieldName.c_str(), strValue.c_str());
				pDataRow->push_back(strValue);
			}
			rLstData.push_back(pDataRow);
			pResSet->next();
		}
	}
	else
	{
		RetCode = Ret_Failed;
		
		if (0 == (m_nLogCount % 1024))
		{
			m_nLogCount = 1;
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "No Message deq from database." );
		}
		m_nLogCount++;
	}

	FUNCTION_EXIT;
	return RetCode;
}

// at.q_name, at.msg_sid, at.msgid, at.priority, at.user_data_1, at.user_data_2, at.user_data_3
// user_data_1 == message_qualifier
// user_data_2 == audit_data_oracle
// user_data_3 == audit_data_mysql
int CMAQCmd::_FormatDBBuff2Msg(PMAQ_DEQ_OPT pDeqMsg, LstDataBuffT& crDBData, LstMsgConT& rLstMsg)
{
	int nRet = Ret_Success;
	DEF_ERROR_CHECK(!crDBData.empty()); 
	uint32 uColumnSize = crDBData.front()->size();
	DEF_ERROR_CHECK(uColumnSize == defDBMsgSize);	

	LstDataBuffT::iterator  cIter = crDBData.begin();
	while (cIter != crDBData.end())
	{
		RowT*& DataRow  = (*cIter);
		CMessage* pMsg = new CMessage();

		pMsg->setQName( DataRow->at(0) );
		pMsg->setMsgSeqID( CUtilityFun::atoui64(DataRow->at(1)) );
		pMsg->setMsgID( DataRow->at(2) );
		pMsg->setMsgPriority(atoi(DataRow->at(3).c_str()));
		pMsg->setPayloadType(pDeqMsg->m_nPayloadType);
		pMsg->setPayload(defPayload_1_Tag,  DataRow->at(4));	
		pMsg->setPayload(defPayload_2_Tag,  DataRow->at(5));
		pMsg->setPayload(defPayload_3_Tag,  DataRow->at(6));
		
		if (TA_Base_Core::DebugUtil::getInstance().getLevel() == TA_Base_Core::DebugUtil::DebugMsg)
		{
			ulong64     ulongSeqIDGet = 0;
			std::string strPayload;
			ulongSeqIDGet = pMsg->getMsgSeqID();		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
				"_FormatDBBuff2Msg [pMsg->getMsgSeqID=%llu]", ulongSeqIDGet);
			pMsg->getPayload(strPayload);
			LOGLARGESTRING(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, strPayload);
		}	

		rLstMsg.push_back(pMsg);

		cIter++;
	}
	
	return nRet;
}

void  CMAQCmd::_FreeDBDataBuffer(LstDataBuffT& crDBData)
{
	LstDataBuffT::iterator iter = crDBData.begin();
	while ( iter != crDBData.end() )
	{
		(*iter)->clear();
		DEF_DELETE(*iter);

		iter++;
	}
	crDBData.clear();
}

uint32  CMAQCmd::_CheckSubCnt(const std::string& strQName)
{
	FUNCTION_ENTRY("mAQDeqCommit");
	uint32 nSubCnt = 0;
	uint32  uSQLSize = 0;
	size_t  ulCount = 0;
	char*  pszSQL = NULL;
	sql::ResultSet* pQuery  = NULL;

	uSQLSize += s_strQSubCntSQLFormat.size();
	uSQLSize += strQName.size();

	pszSQL = new char[uSQLSize + 1];
	memset(pszSQL, 0, uSQLSize + 1);

	sql_snprintf(pszSQL, uSQLSize, s_strQSubCntSQLFormat.c_str(), strQName.c_str());

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
		"_CheckSubCnt m_pStatement->executeQuery( pszSQL )[pszSQL=%s]", pszSQL);

	//std::auto_ptr<sql::Statement> smt(m_pConnection->createStatement());
	pQuery = m_pStatement->executeQuery( pszSQL );
	if (!pQuery) return false;

	if ( pQuery )
		ulCount = pQuery->rowsCount();
	if ( ulCount > 0 )
	{
		pQuery->first();
		nSubCnt = pQuery->getUInt(1);
	}

	DEF_DELETE_ARRAY(pszSQL);
	if ( pQuery )
		pQuery->close();
	DEF_DELETE(pQuery);

	FUNCTION_EXIT;
	return nSubCnt;
}

void   CMAQCmd::_ProcessExcepData(const std::string& strQName, const std::string& strMsgID, uint32 uSubID, const char* pszSubName)
{
	TA_ASSERT(m_pConnection, defErrMsg(Err_Invalid_Ptr));	

	std::string strDisTable("aq_" + strQName + "_t_i");
	std::string strSubName(pszSubName);

	sql::ResultSet* pQuery  = NULL;
	char*   pszSQL   = NULL;
	uint32  uSQLSize = 0;
	uint32  ulCount  = 0;
	ulong64 ulMsgSID = 0;

	uSQLSize = s_strGetExcepDataRemote.size();
	uSQLSize += strDisTable.size();
	uSQLSize += strMsgID.size();
	uSQLSize += defMaxNameLen;

	pszSQL = new char[uSQLSize + 1];
	memset(pszSQL, 0, uSQLSize + 1);

	if (uSubID > 0 && strSubName.empty())
	{
		sql_snprintf(pszSQL, uSQLSize, s_strGetExcepDataRemote.c_str(), strDisTable.c_str(), uSubID, strMsgID.c_str());
	}
	else
	{
		sql_snprintf(pszSQL, uSQLSize, s_strGetExcepDataLocal.c_str(), strDisTable.c_str(), strSubName.c_str(), strMsgID.c_str());
	}

	if (NULL == m_pStatement.get())
	{
		m_pStatement.reset(m_pConnection->createStatement());
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
		"_ProcessExcepData m_pStatement->executeQuery( pszSQL )[pszSQL=%s]", pszSQL);

	pQuery = m_pStatement->executeQuery( pszSQL );
	if ( pQuery )
		ulCount = pQuery->rowsCount();
	if ( ulCount > 0 )
	{
		pQuery->first();
		ulMsgSID = pQuery->getInt64(1);
	}

	DEF_DELETE_ARRAY(pszSQL);
	if ( pQuery )
		pQuery->close();
	DEF_DELETE(pQuery);

	// insert into exception queue 
	uSQLSize =  s_strRecordExcepData.size();
	uSQLSize += strQName.size();
	uSQLSize += strMsgID.size();
	uSQLSize += defMaxNameLen;

	pszSQL = new char[uSQLSize + 1];
	memset(pszSQL, 0, uSQLSize + 1);

	sql_snprintf(pszSQL, uSQLSize, s_strRecordExcepData.c_str(), strQName.c_str(), ulMsgSID, strMsgID.c_str());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
		"_ProcessExcepData m_pStatement->executeQuery( pszSQL )[pszSQL=%s]", pszSQL);

	m_pStatement->execute( pszSQL );

	DEF_DELETE_ARRAY(pszSQL);
}

NS_END(TA_AQ_Lib)
