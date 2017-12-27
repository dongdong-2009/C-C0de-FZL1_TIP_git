#include "SrvWorkMutil.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"

#include "SrvDeqWorker.h"
#include "SrvEnqWorker.h"
#include "SrvComWorker.h"
#include "SrvInitWorker.h"
#include "IObserverRcvMsg.h"


#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)

CSrvWorkMutil::CSrvWorkMutil( TcpSocket* pNewTcpSocket):IServerTcpComType(pNewTcpSocket)
{	
	m_nClientIndex = 0;
	m_pSrvDeqWorker = NULL;
	m_pSrvEnqWorker = NULL;

	m_nThreadIDSrvEnqWorker = 0;
	m_nThreadIDSrvDeqWorker = 0;

}


CSrvWorkMutil::~CSrvWorkMutil()
{
	serverTerminateAndWait(CurrentClientDestroy_Normal);
	DEF_DELETE(m_pSrvInitWorker);		
 	if (NULL != m_pMsgPackageDispatch)
	{
		m_pMsgPackageDispatch->removeAllHandler();
		DEF_DELETE(m_pMsgPackageDispatch);
	}

}





 

int	CSrvWorkMutil::getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet)
{
	int nFunRes = 0;
	
	workTimeInfoGet.m_strClientID = m_strClientID;
	if (NULL != m_pSrvInitWorker)
	{
		m_pSrvInitWorker->getWorkTimeInfo(workTimeInfoGet);
	}
	else
	{
		workTimeInfoGet.m_nHandSharkNotGet = 0;
	}

	
	if (NULL != m_pSrvDeqWorker)
	{
		m_pSrvDeqWorker->getWorkTimeInfo(workTimeInfoGet);

	}
	else
	{
		workTimeInfoGet.m_nDeqNotWork = 0;
	}

	
	if (NULL != m_pSrvEnqWorker)
	{
		m_pSrvEnqWorker->getWorkTimeInfo(workTimeInfoGet);

	}
	else
	{
		workTimeInfoGet.m_nEnqNotWork = 0;
	}



	return nFunRes;
}






bool CSrvWorkMutil::serverTerminateAndWait(int nMode)
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"begin CSrvWorkMutil::serverTerminateAndWait() ClientID=%s nMode=%d ", m_strClientID.c_str(),nMode) ; 
	
	//if client send request to this server, not process request	
	if (NULL != m_pSrvComWorker)
	{	 
		m_pSrvComWorker->removeObserver();	
	}		
	
	if (NULL != m_pSrvDeqWorker)
	{
		m_pSrvDeqWorker->serverTerminateAndWait(nMode);		 
	}

	if (NULL != m_pSrvEnqWorker)
	{
		m_pSrvEnqWorker->serverTerminateAndWait(nMode);		 
	}

	if (NULL != m_pSrvInitWorker)
	{
        DEF_DELETE(m_pSrvInitWorker);		
	} 

	if (NULL != m_pSrvDeqWorker)
	{		 
		DEF_DELETE(m_pSrvDeqWorker);
	}

	if (NULL != m_pSrvEnqWorker)
	{		 
		DEF_DELETE(m_pSrvEnqWorker);
	}

	 
	if (NULL != m_pSrvComWorker)
	{	 		 
		m_pSrvComWorker->terminateAndWait();
		DEF_DELETE(m_pSrvComWorker);
	} 

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"end CSrvWorkMutil::serverTerminateAndWait() ClientID=%s nMode=%d ", m_strClientID.c_str(),nMode) ; 
  
	return true;
}
 
bool CSrvWorkMutil::serverStartDeqWork(int nConReqACk)
{
	bool bFunRes = true;
	PAQWorkInfo pDeqInfo = NULL;

	pDeqInfo = new AQWorkInfo();
	pDeqInfo->m_strQName = m_strQName;
	pDeqInfo->m_strClientAgtName = m_strClientAgtName;
	pDeqInfo->m_strServerAgtName = m_strServerAgtName;
	pDeqInfo->m_nINSRVID = m_nINSRVID;
	pDeqInfo->m_strClientID = m_strClientID;

	m_pSrvDeqWorker = new CSrvDeqWorker(m_pSrvComWorker, pDeqInfo, m_ID);
			  
	m_pSrvDeqWorker->serverStart();
	m_nThreadIDSrvDeqWorker = m_pSrvDeqWorker->getId();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "new Deq worker, thread id=%d, ClientID=%s", m_nThreadIDSrvDeqWorker,m_strClientID.c_str()); 
 
	//registerHandler   ComProtocol_CTOS_ENQ_ACK with m_pSrvDeqWorker
	m_pMsgPackageDispatch->registerHandler(defDefault_ServerIDName, ComProtocol_CTOS_ENQ_ACK, m_pSrvDeqWorker);
	
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"serverStartDeqWork[ClientID=%s][ThreadIDSrvDeqWorker=%d]", 
		m_strClientID.c_str(), m_nThreadIDSrvDeqWorker); 

	DEF_DELETE(pDeqInfo);

	return bFunRes;
}

bool CSrvWorkMutil::serverStartEnqWork(int nConReqACk)
{
	bool bFunRes = true;
	PAQWorkInfo pEnqInfo = NULL;
 
	pEnqInfo = new AQWorkInfo();
	pEnqInfo->m_strQName = m_strQName;
	pEnqInfo->m_strClientAgtName = m_strClientAgtName;
	pEnqInfo->m_strServerAgtName = m_strServerAgtName;
	pEnqInfo->m_nINSRVID = m_nINSRVID;
	pEnqInfo->m_strClientID = m_strClientID;
	pEnqInfo->m_nEnqMode = m_nEnqMode;
 
	m_pSrvEnqWorker = new CSrvEnqWorker(m_pSrvComWorker, pEnqInfo, m_ID);
	m_pSrvEnqWorker->serverStart();
	m_nThreadIDSrvEnqWorker = m_pSrvEnqWorker->getId();

	//registerHandler   ComProtocol_CTOS_ENQ_REQ with m_pSrvEnqWorker
	m_pMsgPackageDispatch->registerHandler(defDefault_ServerIDName, ComProtocol_CTOS_ENQ_REQ, m_pSrvEnqWorker);

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"serverStartEnqWork[ClientID=%s][ThreadIDSrvEnqWorker=%d]", 
		m_strClientID.c_str(), m_nThreadIDSrvEnqWorker); 

	DEF_DELETE(pEnqInfo);

	return bFunRes;
}





NS_END(TA_DB_Sync)


























