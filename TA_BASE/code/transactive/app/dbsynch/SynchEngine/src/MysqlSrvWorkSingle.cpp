#include "MysqlSrvWorkSingle.h"
#include "CommonDef.h"
#include "CommonData.h"

#include "IMsgItem.h"
#include "MsgItemImp.h"
#include "IMsgPackage.h"
#include "MsgPackageImp.h" 

#include "IDbData.h"
#include "IDatabase.h"
#include "DatabaseImpl.h"

#include "SrvComWorker.h"
#include "SrvInitWorker.h"

#include "MsgDispatcher.h"
#include "UtilityFun.h"

#include "MysqlSrvEnqWorker.h"

#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)

CMysqlSrvWorkSingle::CMysqlSrvWorkSingle( TcpSocket* pNewTcpSocket):IServerTcpComType(pNewTcpSocket)
{
	m_nClientIndex = 0;
	m_pSrvEnqWorker = NULL;
	m_nThreadIDSrvEnqWorker = 0;		

}


CMysqlSrvWorkSingle::~CMysqlSrvWorkSingle()
{
	serverTerminateAndWait(CurrentClientDestroy_Normal);
	DEF_DELETE(m_pSrvInitWorker);		


	if (NULL != m_pMsgPackageDispatch)
	{
		m_pMsgPackageDispatch->removeAllHandler();
		DEF_DELETE(m_pMsgPackageDispatch);
	}

}

 
 

bool CMysqlSrvWorkSingle::serverTerminateAndWait(int nMode)
{ 
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"begin CMysqlSrvWorkSingle::serverTerminateAndWait() nMode=%d", nMode); 

	if (NULL != m_pSrvComWorker)
	{	 
		m_pSrvComWorker->removeObserver();	
	}		

	if (NULL != m_pSrvEnqWorker)
	{		 
		m_pSrvEnqWorker->serverTerminateAndWait(nMode);		 
		DEF_DELETE(m_pSrvEnqWorker);
	}

	if (NULL != m_pSrvInitWorker)
	{		 
		DEF_DELETE(m_pSrvInitWorker);
	}

	if (NULL != m_pSrvComWorker)
	{	 		 
		m_pSrvComWorker->terminateAndWait();
		DEF_DELETE(m_pSrvComWorker);
	} 

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"end CMysqlSrvWorkSingle::serverTerminateAndWait() nMode=%d", nMode); 

	return true;
}

int	CMysqlSrvWorkSingle::getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet)
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

	workTimeInfoGet.m_nDeqNotWork = 0;
	workTimeInfoGet.m_nHandSharkNotGet = 0;

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
bool CMysqlSrvWorkSingle::serverStartDeqWork(int nConReqACk)
{
	bool bFunRes = true;
	return bFunRes;
} 
bool CMysqlSrvWorkSingle::serverStartEnqWork(int nConReqACk)
{
	bool bFunRes = true;
	PAQWorkInfo pDeqEnqInfo = NULL;
	
	pDeqEnqInfo = new AQWorkInfo();
	pDeqEnqInfo->m_strQName	 = m_strQName;
	pDeqEnqInfo->m_strClientAgtName	 = m_strClientAgtName;
	pDeqEnqInfo->m_strServerAgtName	 = m_strServerAgtName;
	pDeqEnqInfo->m_nINSRVID = m_nINSRVID;
	pDeqEnqInfo->m_strClientID = m_strClientID;

	if (NULL == m_pSrvEnqWorker)
	{
		m_pSrvEnqWorker = new CMysqlSrvEnqWorker(m_pSrvComWorker,pDeqEnqInfo, m_ID );
	}	 
	m_pSrvEnqWorker->serverStart();  
	m_nThreadIDSrvEnqWorker = m_pSrvEnqWorker->getId();
	m_pMsgPackageDispatch->registerHandler(defDefault_ServerIDName, ComProtocol_CTOS_ENQ_REQ, m_pSrvEnqWorker);

	LOG2(SourceInfo, DebugUtil::DebugInfo, 
		"serverStartEnqWork()[strClientID=%s][ThreadIDSrvEnqWorker=%d]", 
		m_strClientID.c_str(), m_nThreadIDSrvEnqWorker);
		
	DEF_DELETE(pDeqEnqInfo);
 
	return bFunRes;
} 



 
NS_END(TA_DB_Sync)


























