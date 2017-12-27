//////////////////////////////////////////////////////////////////////
///		@file		DeqMsgJob.cpp
///		@author		zhilin,ouyang
///		@date		2012-02-19 14:44:41
///
///		@brief	    to implement the class for dequeue message for the 
///                 local database, it's same for Oracle dequeue job
///                 database synchronization routine.
///                 
//////////////////////////////////////////////////////////////////////
#include "DeqMsgJob.h"
#include "DatabaseImpl.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "IDatabase.h"
#include "CMysqlEnv.h"

NS_BEGIN(TA_DB_Sync)

CDeqMsgJob::CDeqMsgJob(const char *pzLocation)
: m_bTerminate(false)
, m_strLocation(pzLocation)
, m_pDatabase(NULL)
{
	m_strDeqSQL.clear();
	m_pDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
}

CDeqMsgJob::CDeqMsgJob(const std::string& strLocation)
: m_bTerminate(false)
, m_strLocation(strLocation)
, m_pDatabase(NULL)
{
	m_strDeqSQL.clear();
	m_pDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);	
}

CDeqMsgJob::~CDeqMsgJob()
{
	DEF_DELETE(m_pDatabase);
}

void  CDeqMsgJob::run()
{
	unsigned long  ulMsgPrint = 0;
	int nRetCode = 0;
	while (!m_bTerminate)
	{
		nRetCode = _DeqMsg();

		ulMsgPrint++;
		if (ulMsgPrint > defDefaultMsgPrintTimes)
		{
			ulMsgPrint = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Deq message for location: %s", m_strLocation.c_str());
		}
		switch ( nRetCode )
		{
		case ER_MYSQL_PRC_NOEXIST:
			sleep(TIME_SECOND_TWO);
			_Terminate();
			break;
		case ER_MYSQL_GONE_AWAY:
			_InitEnv();
			if(!m_bTerminate)
				sleep(TIME_BASE_SECOND);
			break;
		default:
			sleep(defDefaultDeqTime);
			break;
		}		
	}

	// end of the thread, need release the mysql resource.
	CMysqlEnv::getInstance().connThreadEnd();
}

void  CDeqMsgJob::terminate()
{
	m_bTerminate = true;
}

void  CDeqMsgJob::setQName(const std::string& strQName)
{
	m_strDeqJobName = ("prc_deq_" + strQName);
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"CDeqMsgJob::setQName[DeqJobName=%s]", m_strDeqJobName.c_str());	

}

int  CDeqMsgJob::_DeqMsg()
{
	int nRetCode = Ret_Success;

	try
	{
		if (m_strDeqSQL.empty())
		{
			m_pDatabase->prepareSQLStatement(m_strDeqSQL, AQSYSDEQJOB_INSERT_4053,  m_strDeqJobName, m_strLocation);
		}	

		m_pDatabase->connect();
		m_pDatabase->executeModification(m_strDeqSQL);
		
	}
	catch (const CDBException& e)
	{
		nRetCode = e.getNativeCode();
		if (ER_MYSQL_PRC_NOEXIST != nRetCode)
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Database exception, Error Code: %d; Message: %s", e.getNativeCode(), e.getReason().c_str() );
	}	
	catch(const TA_Base_Core::DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another db
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error Code: %d; Message: %s", Err_Unknown, defErrMsg(Err_Unknown) );	
		nRetCode = ER_MYSQL_GONE_AWAY;
	}

	return nRetCode;
}

void   CDeqMsgJob::_InitEnv()
{
	DEF_DELETE(m_pDatabase);

	m_pDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);	
}

void   CDeqMsgJob::_Terminate()
{

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Message: %s", "The Deq Job will be terminated as the deq procedure is not exist.");
	m_bTerminate = true;
}

NS_END(TA_DB_Sync)