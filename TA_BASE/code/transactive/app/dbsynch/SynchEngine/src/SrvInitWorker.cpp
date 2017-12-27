#include "SrvInitWorker.h"

#include "MsgHelper.h"
#include "WorkTime.h"

#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)

  
CSrvInitWorker::CSrvInitWorker(CSrvComWorker* pComWorker)
{
	m_pComWorkerCopy = pComWorker;
	m_pConACK = NULL;
	m_pMsgItemHelper = NULL;
	m_strPkgUUID32.clear();
	m_strAgtName.clear();
	m_strQName.clear();
	m_strClientHostName.clear();
	m_strClientIP.clear();
	m_nINSRVID = -1000;
	m_pMonitorHandSharkTime = new CWorkTimeLock(PROPAGATE_TIMER_LOG_MonitorHandShark);
	//recv handshark begin time here
	m_pMonitorHandSharkTime->workBegin();


}

CSrvInitWorker::~CSrvInitWorker()
{
	m_pComWorkerCopy = NULL;

	DEF_DELETE(m_pConACK);
	DEF_DELETE(m_pMsgItemHelper);
	DEF_DELETE(m_pMonitorHandSharkTime);
}



std::string CSrvInitWorker::getAgtName()
{
	return m_strAgtName;
}
std::string CSrvInitWorker::getQName()
{
	return m_strQName;
}
std::string CSrvInitWorker::getClientHostName()
{
	return m_strClientHostName;
}
std::string CSrvInitWorker::getClientIP()
{
	return m_strClientIP;
}
int CSrvInitWorker::getOServerID()
{
	return m_nINSRVID;
}

int	CSrvInitWorker::getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet)
{
	int nFunRes = 0;
	if (NULL != m_pMonitorHandSharkTime)
	{
		workTimeInfoGet.m_nHandSharkNotGet = m_pMonitorHandSharkTime->getNotworkTime();
	}
	else
	{
		workTimeInfoGet.m_nHandSharkNotGet = 0;
	}
	return nFunRes;
}


int CSrvInitWorker::setConReq( PCONREQ pConReq)
{
	//recv handshark end time here
	m_pMonitorHandSharkTime->workEnd();

	int nFunRes = 0;
	m_strPkgUUID32 = pConReq->m_strPkgID;
	m_strAgtName = pConReq->m_strAgtName;
	m_strQName = pConReq->m_strQName;
	m_strClientHostName = pConReq->m_strHostName;
	m_strClientIP = pConReq->m_strIP;
	m_nINSRVID = pConReq->m_nOServerID;
	

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Receive handshark: CSrvInitWorker [PkgUUID32=%s][AgtName=%s][QName=%s][ClientHostName=%s][ClientIP=%s][ServerID=%d]",
		m_strPkgUUID32.c_str(), m_strAgtName.c_str(), 
		m_strQName.c_str(), m_strClientHostName.c_str(), 
		m_strClientIP.c_str(), m_nINSRVID);
	

	//AgtName_QName_IP	
	return nFunRes;
}



int CSrvInitWorker::sendConReqAcK( int nConReqAcK )
{
	int nFunRes = 0;
	MSGPKG* pGetMsgPkg = NULL;
	//send STOCCONREQACK	
	if (NULL == m_pConACK)
	{
		m_pConACK = new CONACK();
	}
	m_pConACK->m_nConReqAck = nConReqAcK;//ConReqACkValue_Succeed;
	m_pConACK->m_strQName = m_strQName;
	m_pConACK->m_strPkgID = m_strPkgUUID32;
    
	m_pMsgItemHelper->setSTOCConACK(m_pConACK, pGetMsgPkg);
	if (NULL != m_pComWorkerCopy)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"send handshark ACK: CSrvInitWorker [PkgUUID32=%s][AgtName=%s][QName=%s][ClientHostName=%s][ClientIP=%s][ServerID=%d]",
			m_strPkgUUID32.c_str(), m_strAgtName.c_str(), 
			m_strQName.c_str(), m_strClientHostName.c_str(), 
			m_strClientIP.c_str(), m_nINSRVID);
		nFunRes = m_pComWorkerCopy->serverSendMsgPkg(pGetMsgPkg);
	}

	pGetMsgPkg = NULL;
	return nFunRes;
}

NS_END(TA_DB_Sync)











