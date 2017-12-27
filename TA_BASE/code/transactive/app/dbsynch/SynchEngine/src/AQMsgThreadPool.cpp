//////////////////////////////////////////////////////////////////////
///		@file		MonitorAQData.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-10 14:44:41
///
///		@brief	    implementation of class CMonitorAQData. 
///                 it gather the data changes from the source database
///                 in its queue table.
///                 
//////////////////////////////////////////////////////////////////////
#include "AQMsgThreadPool.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "PropagateMsg.h"
#include "DeqMsgJob.h"
#include "ConfigInfoHelper.h"
#include "TcpConnectionListen.h"
#include "CMysqlEnv.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DbSyncException.h"

NS_BEGIN(TA_DB_Sync)

CAQMsgThreadsPool::CAQMsgThreadsPool()
: m_bTerminate(false)
, m_pConfigHelper(NULL)
, m_nPoolState(STATE_CODE_INITIAL_ENV)
, m_pMysqlTcpServer(NULL)
{
	m_mapAllDeqJobs.clear();
	m_mapNewRecipients.clear();
	m_mapAllThreads.clear();
	m_mapAllJobs.clear();
	m_bThreadPoolFlag = false;
}

CAQMsgThreadsPool::~CAQMsgThreadsPool()
{
	_ClearAll();
	DEF_DELETE(m_pConfigHelper);
	DEF_DELETE(m_pMysqlTcpServer);
}

void CAQMsgThreadsPool::run()
{
	while ( !m_bTerminate )
	{	

		// check running active or passive mode (control or monitor mode)
		if (g_sys_status_mask.isCodeBitSet(STATE_CODE_RUNNING_MODE))
		{
			//monitor mode
			if (m_nPoolState < STATE_CODE_STOP_JOBS)
			{
				m_nPoolState = STATE_CODE_STOP_JOBS;
			}			
		}
		else
		{	
			//control mode
			if (m_nPoolState >= STATE_CODE_STOP_JOBS)
			{
				m_nPoolState = STATE_CODE_INITIAL_ENV;
			}
		}

		_ThreadJob();
		if ((STATE_CODE_MONITOR_AQ_SYS_EVENT == m_nPoolState) &&
			 !m_bThreadPoolFlag && !m_bTerminate)
		{
			sleep( TIME_SECOND_FOUR );
		}
		else
		{
			sleep( defDefaultListenTime );
		}		
	}

	if (m_bTerminate)
	{	
		// end of the thread, need release the mysql resource.
		CMysqlEnv::getInstance().connThreadEnd();
		_ClearAll();				
	}
}

void CAQMsgThreadsPool::terminate()
{
	m_bTerminate = true;
}


void  CAQMsgThreadsPool::_ThreadJob()
{
	switch (m_nPoolState)
	{
	case STATE_CODE_INITIAL_ENV:
		_ProcessInitialEnv();
		break;
	case STATE_CODE_INITIAL_POOL:
		_ProcessInitialPool();
		break;
	case STATE_CODE_MONITOR_AQ_SYS_EVENT:
		_ProcessMonitorAQEvent();
		break;
	case STATE_CODE_START_UP_NEW_JOBS:
		_ProcessStartNewJob();
		break;
	case STATE_CODE_STOP_JOBS:
		_ProcessStopJobs();
		break;
	case STATE_CODE_WAITFOR_ACTIVE_EVENT:
		_ProcessWaitAQActiveEvent();
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));
		break;
	}
}


void  CAQMsgThreadsPool::_ProcessInitialEnv()
{
	_ClearAll();
	
	if (!m_pMysqlTcpServer)
	{
		std::string strSrvPort = g_system_variables.m_szPort;
		m_pMysqlTcpServer = new CTcpConnectionListen(strSrvPort);
		m_pMysqlTcpServer->start();
	}	  	

	m_nPoolState = STATE_CODE_INITIAL_POOL;
}


void  CAQMsgThreadsPool::_ProcessInitialPool()
{
	if (!m_pConfigHelper)
		m_pConfigHelper = new CConfigInfoHelper();
	m_pConfigHelper->setDBConStr(g_system_variables.m_SrcDbConStr);

	m_bThreadPoolFlag = true;  // speed up the monitor event
	m_nPoolState = STATE_CODE_MONITOR_AQ_SYS_EVENT;
}


void  CAQMsgThreadsPool::_ProcessMonitorAQEvent()
{
	if (!m_mapNewRecipients.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_NewThread_Not_Start, defErrMsg(Err_NewThread_Not_Start));
	}
	int nRetCode = Ret_Failed;

	m_pConfigHelper->monitorDBConfig(m_mapAllThreads, m_mapNewRecipients);
	if (!m_mapNewRecipients.empty())
	{
		//create dequeue jobs and start the jobs, and put the new job into m_mapAllDeqJobs
		_CheckDeqJob(m_mapAllDeqJobs, m_mapNewRecipients);
		
		nRetCode = _TransformRecipientInfos(m_mapNewRecipients, m_mapAllThreads);

		if (Ret_Success == nRetCode)
		    //go to create new propagate thread
			m_nPoolState = STATE_CODE_START_UP_NEW_JOBS;			
		else
		    //release map and monitor again
			_ClearRecipientInfos(m_mapNewRecipients);			
	}
	else
	{
		m_nPoolState = STATE_CODE_MONITOR_AQ_SYS_EVENT;
	}

	m_bThreadPoolFlag = false;  // set back the monitor interval
}

//process STATE_CODE_START_UP_NEW_JOBS status
void  CAQMsgThreadsPool::_ProcessStartNewJob()
{
	if (m_mapNewRecipients.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_NewThread_Not_Start, defErrMsg(Err_NewThread_Not_Start));
         //go to monitor again 
		m_nPoolState = STATE_CODE_MONITOR_AQ_SYS_EVENT;
		return;
	}	

 	_StartUpThreads();

	_ClearRecipientInfos(m_mapNewRecipients);

	m_nPoolState = STATE_CODE_MONITOR_AQ_SYS_EVENT;
}


void  CAQMsgThreadsPool::_ProcessStopJobs()
{
	_ClearAll();	

	m_nPoolState = STATE_CODE_WAITFOR_ACTIVE_EVENT;
}

void  CAQMsgThreadsPool::_ProcessWaitAQActiveEvent()
{
	// just sleep.....TODO...
	sleep( TIME_BASE_SECOND );
}


void  CAQMsgThreadsPool::_ClearAll()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"CAQMsgThreadsPool::_ClearAll()"); 

	_FreeRecipientInfos(m_mapAllJobs);
	_FreeRecipientInfos(m_mapAllDeqJobs);
	_FreeRecipientInfos(m_mapNewRecipients);
	_FreeRecipientInfos(m_mapAllThreads);	

	if (m_pMysqlTcpServer)
	{
		m_pMysqlTcpServer->terminateAndWait();
		DEF_DELETE(m_pMysqlTcpServer);
	}
}

void  CAQMsgThreadsPool::_CheckDeqJob(mapAllDeqJobT& rExistDeqJobPool, mapAllQRecipientInfoT& rNew)
{
	mapAllQRecipientInfoIterT iterNew = rNew.begin();
	while (iterNew != rNew.end())
	{
		mapAllDeqJobIterT iterFound = rExistDeqJobPool.find(iterNew->first);
		if (iterFound == rExistDeqJobPool.end())
		{
			CDeqMsgJob* pDeqJob = new CDeqMsgJob(g_system_variables.m_LocalAgent.getName());
			rExistDeqJobPool.insert(mapAllDeqJobValueT(iterNew->first, pDeqJob));

			pDeqJob->setQName(iterNew->first);
			pDeqJob->start();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[start Deq job for Queue: %s]", iterNew->first.c_str()); 
		}				

		iterNew++;
	}
}

int  CAQMsgThreadsPool::_TransformRecipientInfos(mapAllQRecipientInfoT& rNew, mapAllQRecipientInfoT& rAll)
{
	int nRetCode  = Ret_Failed; 
	int nRetInner = Ret_Failed;

	mapAllQRecipientInfoIterT iterNew = rNew.begin();
	while (iterNew != rNew.end())
	{
		mapAllQRecipientInfoIterT iterFound = rAll.find(iterNew->first);
		if (iterFound != rAll.end())
		{
			mapQRecipientInfoT* rAllMap = iterFound->second;
			mapQRecipientInfoT* rNewMap = iterNew->second;
			//merge new into all
			nRetInner = _TransformRecipient(rNewMap, rAllMap);	
			if ( Ret_Success == nRetInner )  nRetCode = Ret_Success;

		}
		else
		{
			//create a new and insert into all
			std::string strQName = iterNew->first;
			mapQRecipientInfoT* pMapNew = new mapQRecipientInfoT();
			mapQRecipientInfoT* rOriMap = iterNew->second;
			nRetInner = _TransformRecipient(rOriMap, pMapNew);	
			if ( Ret_Success == nRetInner )  nRetCode = Ret_Success;

			rAll.insert(mapAllQRecipientInfoValueT(strQName, pMapNew));			
		}
		
		iterNew++;
	}		
	return nRetCode;
}


int  CAQMsgThreadsPool::_TransformRecipient(mapQRecipientInfoT* rOri, mapQRecipientInfoT*& rDes)
{
	if (rOri->empty()) return Ret_Failed;
	
	mapQRecipientInfoIterT iterOri = rOri->begin();
	while ( iterOri != rOri->end() )
	{
		rDes->insert(mapQRecipientInfoValueT(iterOri->first, iterOri->second));

		iterOri++;
	}

	return Ret_Success;
}


void  CAQMsgThreadsPool::_StartUpThreads()
{
	mapAllQRecipientInfoIterT iter = m_mapNewRecipients.begin();
	while (iter != m_mapNewRecipients.end())
	{
		/*find Q_Name  eg."AUDIT_DATA_QUEUE"*/
		mapAllQThreadPoolIterT iterFound = m_mapAllJobs.find(iter->first);
		
		if (iterFound != m_mapAllJobs.end())
		{  
			/*find the queue same*/
			mapQRecipientInfoT* pRecipientInfo = iter->second;
			mapQThreadPoolT* pOneQJobs = iterFound->second;
			_StartUpNewJobs(pRecipientInfo,  pOneQJobs);
		}
		else
		{
			/*not find the queue same*/
			std::string strQName = iter->first;
			mapQThreadPoolT* pOneQJobs = new mapQThreadPoolT();
			mapQRecipientInfoT* pRecipientInfo = iter->second;
			_StartUpNewJobs(pRecipientInfo, pOneQJobs);

			m_mapAllJobs.insert(mapAllQThreadPoolValueT(strQName, pOneQJobs));
		}

		iter++;
	}
}


void  CAQMsgThreadsPool::_StartUpNewJobs(mapQRecipientInfoT* rOri, mapQThreadPoolT*& rPoolJobs)
{
	uint32 uiSubID = 0;
	CPropagateMsg* pPropagateMsg = NULL;

	mapQRecipientInfoIterT iterOri = rOri->begin();
	while ( iterOri != rOri->end() )
	{
		uiSubID = iterOri->first;
		PRO_THD*& pProThd = iterOri->second;
		pPropagateMsg = new CPropagateMsg();
		pPropagateMsg->setPropagateTHD(pProThd);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"Create propagate channel, SubID=%d,QName=%s", 
			uiSubID, pProThd->m_szQName);
		pPropagateMsg->start();	  
		rPoolJobs->insert(mapQThreadPoolValueT(uiSubID, pPropagateMsg));

		iterOri++;
	}
}


void  CAQMsgThreadsPool::_ClearRecipientInfos(mapAllQRecipientInfoT& rMapThread)
{
	mapAllQRecipientInfoIterT iter = rMapThread.begin();
	while (iter != rMapThread.end())
	{
		iter->second->clear();

		DEF_DELETE(iter->second);
		
		iter++;
	}	

	rMapThread.clear();
}


void  CAQMsgThreadsPool::_FreeRecipientInfos(mapAllQRecipientInfoT& rMapThread)
{
	mapAllQRecipientInfoIterT iter = rMapThread.begin();
	while (iter != rMapThread.end())
	{
		mapQRecipientInfoIterT iterInner = iter->second->begin();
		while (iterInner != iter->second->end())
		{
			PRO_THD*& pThd= iterInner->second;
			lstAgentContIterT iterThdRcv = pThd->m_lstRecver.begin();
			while (iterThdRcv != pThd->m_lstRecver.end())
			{
				DEF_DELETE(*iterThdRcv);
				iterThdRcv++;
			}

			pThd->m_lstRecver.clear();
			DEF_DELETE( pThd );

			iterInner++;
		}

		iter->second->clear();

		DEF_DELETE(iter->second);

		iter++;
	}

	rMapThread.clear();
}


void  CAQMsgThreadsPool::_FreeRecipientInfos(mapAllQThreadPoolT& rMapPool)
{
	mapAllQThreadPoolIterT iter = rMapPool.begin();
	while (iter != rMapPool.end())
	{
		mapQThreadPoolIterT iterInner = iter->second->begin();
		while (iterInner != iter->second->end())
		{
			CPropagateMsg *& pThreadJob = iterInner->second;
			pThreadJob->terminateAndWait();

			DEF_DELETE(pThreadJob);

			iterInner++;
		}

		iter->second->clear();

		DEF_DELETE(iter->second);

		iter++;
	}

	rMapPool.clear();
}

void  CAQMsgThreadsPool::_FreeRecipientInfos(mapAllDeqJobT& rMapDeqJobPool)
{
	mapAllDeqJobIterT iter = rMapDeqJobPool.begin();
	while (iter != rMapDeqJobPool.end())
	{
		CDeqMsgJob*& pDeqJob = iter->second;
		pDeqJob->terminateAndWait();

		DEF_DELETE(pDeqJob);

		iter++;
	}

	rMapDeqJobPool.clear();
}


NS_END(TA_DB_Sync)