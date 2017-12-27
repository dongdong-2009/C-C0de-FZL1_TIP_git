//////////////////////////////////////////////////////////////////////
///		@file		SyncAQMsgManager.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-09 14:44:41
///
///		@brief	    implementation of class CSyncAQMsgManager 
///                 database synchronization routine.
///                 
//////////////////////////////////////////////////////////////////////
#include "SyncAQMsgManager.h"
#include "ConfigInfoHelper.h"
#include "AQMsgThreadPool.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/utilities/src/DebugUtil.h"
#include "UtilityFun.h"
#include "TcpConnectionListen.h"
#include "COracleEnv.h"
#include "AQCommonData.h"
#include "ConnectionActorReleaser.h"
#include "MonitorWorker.h"
#include "CMysqlEnv.h"

NS_BEGIN(TA_DB_Sync)

using TA_Base_Core::Thread;

CSyncAQMsgManager* CSyncAQMsgManager::m_pInstance = 0;
NonReEntrantThreadLockable CSyncAQMsgManager::m_instanceLock;

CSyncAQMsgManager& CSyncAQMsgManager::getInstance()
{	
	ThreadGuard guard( m_instanceLock );
	if (NULL == m_pInstance)
	{
		m_pInstance = new CSyncAQMsgManager();
	}

	return (*m_pInstance);
}

void CSyncAQMsgManager::removeInstance()
{
	DEF_DELETE(m_pInstance);
	
	DebugUtil::removeInstance();	
}

CSyncAQMsgManager::CSyncAQMsgManager()
: m_pAQSynchPool(NULL)
, m_pOracleTcpServer(NULL)
{	
	
}

CSyncAQMsgManager::~CSyncAQMsgManager()
{
	DEF_DELETE(m_pAQSynchPool);
	if (NULL != m_pOracleTcpServer)
	{
		m_pOracleTcpServer->terminateAndWait();
		DEF_DELETE(m_pOracleTcpServer);
		COracleEnv::removeInstance(); 			
	}
	TA_AQ_Lib::CMysqlEnv::removeInstance();
}

void CSyncAQMsgManager::startSynchMsg(const std::string& strFileName)
{
	int nFunRes = Ret_Failed;	

	nFunRes = _InitDefaultVal(&g_system_variables);
	nFunRes = _InitSystemVaribles(strFileName, &g_system_variables);
	if (Ret_Success == nFunRes)
	{
		_StartSynchThread(); 
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(DbSyncException("Initialize config failed!"));
	}

}

void CSyncAQMsgManager::stopSynchMsg()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"CSyncAQMsgManager::stopSynchMsg() begin");	

	switch (g_system_variables.m_nRunType)
	{
	case RunType_Client:			
		if (m_pAQSynchPool)
			m_pAQSynchPool->terminateAndWait();	

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"set system state code for exit");
		// set system state code for exit
		g_sys_status_mask.setCodeFlag(STATE_CODE_STATUS);

		break;
	case RunType_Server: // TODO: server side stop
		if (m_pOracleTcpServer)
		{
			m_pOracleTcpServer->terminateAndWait();
			DEF_DELETE(m_pOracleTcpServer);
			COracleEnv::removeInstance(); 			
		}
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"set system state code for exit");

		// set system state code for exit
		g_sys_status_mask.setCodeFlag(STATE_CODE_STATUS);

		break;

	default:
		break;
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,	"terminate ConnectionActorReleaser");
	ConnectionActorReleaser::getInstance()->terminateAndWait();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,	"CSyncAQMsgManager::stopSynchMsg() end");


}

void  CSyncAQMsgManager::setSynchMode(int nMode)
{
	switch (nMode)
	{
	case SYNCH_MODE_ACTIVE:
		g_sys_status_mask.unSetCodeFlag(STATE_CODE_RUNNING_MODE);
		break;
	case SYNCH_MODE_PASSIVE:
		g_sys_status_mask.setCodeFlag(STATE_CODE_RUNNING_MODE);
		break;
	default:
		break;
	}		
}

void  CSyncAQMsgManager::setSynchMode(const std::string& strMode)
{
	if (!CUtilityFun::stricmp_(strMode.c_str(), defCmdLine_Mode_Active))
	{
		setSynchMode(SYNCH_MODE_ACTIVE);
	}
	else if (!CUtilityFun::stricmp_(strMode.c_str(), defCmdLine_Mode_Passive))
	{
		setSynchMode(SYNCH_MODE_PASSIVE);
	}	
	else if (!CUtilityFun::stricmp_(strMode.c_str(), defCmdLine_Mode_Stop))
	{
		stopSynchMsg();
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error message: %s, Mode is: %s", "Input runing mode not support.", strMode.c_str());
	}
}

int  CSyncAQMsgManager::getSynchMode()const
{
	return (g_sys_status_mask.isCodeBitSet(STATE_CODE_RUNNING_MODE)) ? SYNCH_MODE_PASSIVE : SYNCH_MODE_ACTIVE;
}


int  CSyncAQMsgManager::_InitDefaultVal(pg_config_parameter pSystemVar)
{
	if ( !pSystemVar )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));

		return (Ret_Failed);
	}

	pSystemVar->m_MsgCompress = true;
	pSystemVar->m_LogLevel = TA_Base_Core::DebugUtil::DebugDebug;
	pSystemVar->m_LogFileCount = defDefault_Config_LogFileCount;
	pSystemVar->m_LogFileSize  = defDefault_Config_LogFileSize;

	memset(pSystemVar->m_SrcDbConStr, 0, defConfig_OneLineMaxLen);
	memset(pSystemVar->m_szCharSet, 0, defMaxNameLen);
	memset(pSystemVar->m_SkipErrors, 0, defMaxNameLen);
	memset(pSystemVar->m_szPort, 0, defServerPortLen);
	memset(pSystemVar->m_CenterDBName, 0, defMaxNameLen);
	memset(pSystemVar->m_LogFileName, 0, defMaxFileLen);
	memset(pSystemVar->m_DebugPidFilenames, 0, defConfig_OneLineMaxLen);

	memcpy(pSystemVar->m_LogFileName, defDefault_LogFileName, strlen(defDefault_LogFileName));
	memcpy(pSystemVar->m_szPort, defDefault_config_OracleServerPort, strlen(defDefault_config_OracleServerPort));
	memcpy(pSystemVar->m_DebugPidFilenames, defDefault_config_DebugPidFilenames, strlen(defDefault_config_DebugPidFilenames));
	
	pSystemVar->m_nDBType  = enumMysqlDb;
	pSystemVar->m_nRunType = RunType_NotSet;
	pSystemVar->m_nConFailedMonSwitch = Tcp_ConFailMon_Switch_ON;
	pSystemVar->m_nHeartBeatSwitch    = HeartBeat_Switch_ON;
	pSystemVar->m_nHeartBeatInterval  = TIME_MIN_TWO;
	pSystemVar->m_nHeartBeatTimeOut   = TIME_MIN_FIVE;
	pSystemVar->m_nBackSlashSwitch    = CON_BACKSLASH_OFF;
		
	pSystemVar->m_nEnqTimeOut = TIME_MIN_FIVE;	
	pSystemVar->m_nDeqTimeOut = TIME_MIN_FIVE;		
	pSystemVar->m_nDeqGetACKTimeOut = TIME_MIN_FIFTEEN;	
	pSystemVar->m_nHandShareTimeOut = TIME_SECOND_TEN;		
	pSystemVar->m_nReleaseActorTimeOut = TIME_MIN_FIVE;	
	pSystemVar->m_nEnqBatchSize = defEnqBatchSize;
	pSystemVar->m_nDeqFailOverTime = TIME_SECOND_FIVE;
	pSystemVar->m_nDeqUpdateTokenTime = pSystemVar->m_nDeqFailOverTime / 2;
	return (Ret_Success);
}

int  CSyncAQMsgManager::_InitSystemVaribles(const std::string& strFileName, pg_config_parameter pSystemVar)
{
	int  nFunRes = -1;
	
	if ( !pSystemVar )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));

		return (Ret_Failed);
	}
	CConfigInfoHelper* pConfigHelper = new CConfigInfoHelper(strFileName);
	if( pConfigHelper )
	{
		nFunRes = pConfigHelper->initConfigParameter(pSystemVar);
	} 
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Out_Of_Mem, defErrMsg(Err_Out_Of_Mem));
		nFunRes = Ret_Failed;
	}

	// after initializing, free the config helper
	DEF_DELETE(pConfigHelper);

	return nFunRes;
}

void CSyncAQMsgManager::_StartSynchThread()
{
	int nFunRes = 0;
	int nRunType = 0;
	std::string strServerPort;
	strServerPort = g_system_variables.m_szPort;
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"CSyncAQMsgManager::_StartSynchThread() begin");	

	switch (g_system_variables.m_nRunType)
	{
	case RunType_Client:
		m_pAQSynchPool = new CAQMsgThreadsPool();
		m_pAQSynchPool->start(); 		

		DEF_WAITFOR_SIGNAL(g_sys_status_mask.isCodeBitSet(STATE_CODE_STATUS));
		break;
	case RunType_Server:
	
		//start Release Actor thread
		ConnectionActorReleaser::getInstance();
		
		//start Monitor Threads workTime thread
		CMonitorWorker::getInstance();
		
		m_pOracleTcpServer = new CTcpConnectionListen(strServerPort);
		m_pOracleTcpServer->start();
		
		DEF_WAITFOR_SIGNAL(g_sys_status_mask.isCodeBitSet(STATE_CODE_STATUS));			
			
		//stop Monitor Threads workTime thread
		CMonitorWorker::getInstance()->terminate();

		//stop Release Actor thread
		ConnectionActorReleaser::getInstance()->terminate();

	
		break;

	default:
		break;
	}  //switch
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"CSyncAQMsgManager::_StartSynchThread() end");
}


NS_END(TA_DB_Sync)
