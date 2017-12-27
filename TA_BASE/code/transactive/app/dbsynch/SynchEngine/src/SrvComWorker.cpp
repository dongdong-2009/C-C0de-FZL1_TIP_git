#include "SrvComWorker.h"
#include <string.h>		  
#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/uuid/src/TAuuid.h"
#include "zlib.h"
#include "IMsgItem.h"
#include "MsgItemImp.h"
#include "IMsgPackage.h"
#include "MsgPackageImp.h"
#include "IObserverRcvMsg.h"
#include "ComProtocol.h"
#include "WorkTime.h"


NS_BEGIN(TA_DB_Sync)

using TA_Base_Core::DebugUtil;

CSrvComWorker::CSrvComWorker(TcpSocket* pTcpSocket)
{
	m_pRecvBufferOnce = NULL;
	m_pRecvBufferTotal = NULL;
	m_pComProtocol = NULL;							
	m_pRecvBufferOnce = new CBuffer(BUFFER_LEN_READ);
	m_pRecvBufferOnce->clear();						
	m_pRecvBufferTotal = new CBuffer(BUFFER_LEN_READ);
	m_pRecvBufferTotal->clear();					
	m_pComProtocol = new CComProtocol();
	m_socketTimeout = SERVER_LIMIT_TCP_TIMEOUT;		
	m_pObserverRcvMsg = NULL;				
	m_nLogCount = 0;
	m_toTerminate = false;
	//m_nHeartBeatState = enHeartBeat_Timer_OFF;
	
	m_pSocket = pTcpSocket;
	m_lstRecvMsgPackage.clear();	
	
	m_strClientID.clear();
	m_isConnected = m_pSocket->stillConnected();
	m_pHeartBeatTimer = NULL;
	_InitHeartBeatTimer();
}


CSrvComWorker::~CSrvComWorker()
{
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "begin ~CSrvComWorker ClientID=%s", m_strClientID.c_str());
	DEF_DELETE(m_pRecvBufferOnce);	
	DEF_DELETE(m_pRecvBufferTotal);		
	DEF_DELETE(m_pComProtocol);		
	m_pObserverRcvMsg = NULL;
	_FreeLstRecvMsgPackage();
	if (m_pSocket!=NULL)
	{
	  m_pSocket->close();
	}	
	DEF_DELETE(m_pSocket);
	DEF_DELETE(m_pHeartBeatTimer);
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "end ~CSrvComWorker ClientID=%s", m_strClientID.c_str());
}




int CSrvComWorker::_FreeLstMsgItem(lstMsgItemConT& lstMsgItem)
{
	int nFunRes = 0;
	lstMsgItemConIterT iterlstMsgItem;
	IMsgItem* pGetMsgItem = NULL;
	
	iterlstMsgItem = lstMsgItem.begin();
	while (iterlstMsgItem != lstMsgItem.end())
	{
		pGetMsgItem = *iterlstMsgItem;			
		DEF_DELETE(pGetMsgItem);				
		iterlstMsgItem++;
	}
	lstMsgItem.clear();	
	return nFunRes;
}

int CSrvComWorker::_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems)
{
	int nFunRes = 0;
	lstMsgItemsConIterT iterLstAllMsgItems;
	lstMsgItemConT* pGetLstMsgItem = NULL;
	
	iterLstAllMsgItems = lstMsgItems.begin();
	while (iterLstAllMsgItems != lstMsgItems.end())
	{
		 pGetLstMsgItem = *iterLstAllMsgItems;
		_FreeLstMsgItem(*pGetLstMsgItem);
		//fix memory problem
		DEF_DELETE(pGetLstMsgItem);		
		iterLstAllMsgItems++;
	}
	
	lstMsgItems.clear();
	
	return nFunRes;
}




int CSrvComWorker::_FreeLstRecvMsgPackage()
{
	int nFunRes = 0;  	
	lstMSGPKGConIterT  iterMsgPkg;
	PMSGPKG pEachMsgPkg = NULL;

	if (m_lstRecvMsgPackage.empty())
	{
		nFunRes = 0;
		return nFunRes;
	}

	iterMsgPkg = m_lstRecvMsgPackage.begin();
	while (iterMsgPkg != m_lstRecvMsgPackage.end())
	{
		pEachMsgPkg = *iterMsgPkg;
		nFunRes = _FreeLstMsgItems(pEachMsgPkg->m_lstAllMsgItems);	
		DEF_DELETE(pEachMsgPkg);
		pEachMsgPkg = NULL;
		iterMsgPkg++; 
	}
	m_lstRecvMsgPackage.clear();

	return nFunRes;
}

int CSrvComWorker::serverSendMsgPkg(MSGPKG* pMsgPkg)
{
	int nFunRes = 0;
	IMsgPackage* pSendData = NULL;
	if (NULL == pMsgPkg)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MsgPkg handle is NULL");
		nFunRes = -1;
		return nFunRes;	 

	}
	
	m_pComProtocol->processSendData(pMsgPkg, pSendData);	
	
	_FreeLstMsgItems(pMsgPkg->m_lstAllMsgItems);	
	DEF_DELETE(pMsgPkg);
	 	
	nFunRes = _SocketWriteMsgPackage(pSendData);  
	DEF_DELETE(pSendData);
	
	return nFunRes;
}

int CSrvComWorker::_SocketRead()
{ 
	int					nFunRes = 0;
	unsigned int		nGetLength = 0;
	int					nRcvedRes = -1;		
	bool				bCanRead = false;
	   	
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);

	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		if (!m_isConnected)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "client disconnect to server");
			nFunRes = -1;
			return nFunRes;
		}
		else
		{
			//check if TCP is still connected
			//when the client  side disconnected and the server has no data to client, 
			//then the server side does not know the disconnection except calling this interface
			m_isConnected = m_pSocket->stillConnected();
		}		

		if (!m_isConnected)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "lost connect with client,[ClientID=%s]", m_strClientID.c_str()  );
			m_isConnected = false;
			nFunRes = -1;
			return nFunRes;
		}

	}
	

	
	//check if have data to read
	bCanRead = m_pSocket->isReadRequired(m_socketTimeout);		
	if (!bCanRead)
	{
		//if bCanRead = false, no data in socket can be readed
		nFunRes = TCP_NODATAFORREAD;
		return nFunRes;
	} 

	LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[ClientID=%s][isReadRequired() == %d]", 
			m_strClientID.c_str(), bCanRead );

	try
	{  
		m_pRecvBufferOnce->clear();

		nGetLength = 0;
		nRcvedRes = -1;
		nRcvedRes = m_pSocket->read(m_pRecvBufferOnce->getWritePos(), &nGetLength, m_pRecvBufferOnce->getBlankSize()-1);
		m_pRecvBufferOnce->moveWritePos(nGetLength);

		if (nGetLength <= 0 || nRcvedRes != 0)
		{
			TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
			m_isConnected = false;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"err: read data from socket error! [0 != nRcvedRes=%d][nGetLength=%d][ClientID=%s]",
				nRcvedRes, nGetLength, m_strClientID.c_str());
			nFunRes = -1;
		}
		else
		{
			LOG1(SourceInfo, DebugUtil::DebugDebug, "-->Server get data from Client [nGetLength=%d]", nGetLength);
			m_pRecvBufferTotal->bufferSizePrepare(m_pRecvBufferOnce->remainDataLength() + BUFFER_LEN_ADDEX);
			m_pRecvBufferTotal->write(m_pRecvBufferOnce->getReadPos(), m_pRecvBufferOnce->remainDataLength());
			m_pRecvBufferOnce->moveReadPos(m_pRecvBufferOnce->remainDataLength());
			nFunRes = 0;
		}

		
	}
	catch (TA_Base_Core::TcpSocketException& e)
	{	
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: read data from socket error! TcpSocketException:%s  [ClientID=%s]", e.what(), m_strClientID.c_str());
		nFunRes = -1;	
	}
	catch (...)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: read data from socket error! [ClientID=%s]", m_strClientID.c_str());
		nFunRes = -1;	
	}

	return nFunRes;
}

int CSrvComWorker::_ProcessRecvBuffer()
{
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	int			nFunRes = 0;
	bool		bHaveOneFrame = false;
	
	bHaveOneFrame = m_pComProtocol->isBufferHaveOneFrame(m_pRecvBufferTotal);
	if (false == bHaveOneFrame)
	{
		nFunRes = -1;
		return nFunRes;
	}	
	m_pComProtocol->processRecvData(m_pRecvBufferTotal, m_lstRecvMsgPackage);

	//m_pRecvBufferTotal->resizeFitData();
	m_pRecvBufferTotal->trim();	
	
	if (0 == (m_nLogCount % SERVER_LIMIT_TCP_RecvBuffReset))
	{
		m_pRecvBufferTotal->resizeFitData();
		m_nLogCount = 1;
	}
	m_nLogCount++;
	
	return nFunRes;
}
  
int CSrvComWorker::_SocketWriteMsgPackage(IMsgPackage* pSendMsgPackage)
{
	int			nSendRes = 0; 	
	CBuffer*	pSendBuff = NULL;
	int			nSendBuffLen = 0;
	unsigned char ucPkgType = 0;
	
	nSendBuffLen =	pSendMsgPackage->getMsgFramePL() + BUFFER_LEN_ADDEX;
	
	pSendBuff = new CBuffer(nSendBuffLen);
	
	pSendBuff->write((const char *)pSendMsgPackage->getFrameStartPtr(), pSendMsgPackage->getMsgFramePL());

	nSendRes = _SocketWriteBuffer(pSendBuff);

	/*ucPkgType = (*pSendMsgPackage->getMsgFramePT());
	if (HeartBeat_Switch_ON  == g_system_variables.m_nHeartBeatSwitch &&
		MSGPACKAGE_PT_ACK    == ucPkgType &&
		enHeartBeat_Timer_ON == m_nHeartBeatState)
	{
		m_nHeartBeatState = enHeartBeat_Timer_OFF;
		LOG1(SourceInfo, DebugUtil::DebugMsg, "set HeartBeat Timer end [%s]", m_strClientID.c_str());		
		m_pHeartBeatTimer->workEnd();
	}*/
	
	DEF_DELETE(pSendBuff);
	
	return nSendRes;		  
}

int CSrvComWorker::_SocketWriteBuffer(CBuffer* pSendBuff)
{
	TA_Base_Core::ThreadGuard guard(m_SocketWriteLock);
	int		nSendRes = 0;
	char*	pSendPos = NULL;
	int     nSendLen = 0;
	
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		if (!m_isConnected)
		{
			nSendRes = -1;
			return nSendRes;
		}
	}
	
	pSendPos = pSendBuff->getReadPos();
	nSendLen = pSendBuff->remainDataLength();

	try
	{		
		nSendRes = m_pSocket->write(pSendPos, nSendLen);
		LOG3(SourceInfo, DebugUtil::DebugDebug, "<--server send data to client [nSendRes=%d][nBufLen=%d][ClientID=%s]",
				nSendRes, nSendLen, m_strClientID.c_str());

		if (nSendRes != nSendLen)
		{
			TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
			m_isConnected = false;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! server send data to client error![nSendRes=%d][nBufLen=%d][ClientID=%s]",
				nSendRes, nSendLen, m_strClientID.c_str()); 
			nSendRes = -1;
		}
		else
		{
			//send data ok, return value is 0
			nSendRes = 0;
		}
	}					
	catch (TA_Base_Core::TcpSocketException& e)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! server send data to client error! TcpSocketException:%s [ClientID=%s]", e.what(), m_strClientID.c_str());
		nSendRes = -1;
	}
	catch (...)
	{
		TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
		m_isConnected = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error! server send data to client error! [ClientID=%s]", m_strClientID.c_str());
		nSendRes = -1;
	}

	return nSendRes;
}



int CSrvComWorker::_ProcessRcvMsgPackages()
{	  	
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	int nFunRes = 0;
	lstMSGPKGConIterT  iterMsgPkg;
	std::string strServerID;
	PMSGPKG pEachMsgPkg = NULL;

	if (m_lstRecvMsgPackage.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}
	
	iterMsgPkg = m_lstRecvMsgPackage.begin();
	pEachMsgPkg = *iterMsgPkg;	

	//if MSGPACKAGE_PT_CONREQUEST no eartBeat
	// start the heartbeat timer mechanism
	/*if (HeartBeat_Switch_ON   == g_system_variables.m_nHeartBeatSwitch &&
		MSGPACKAGE_PT_REQUEST == pEachMsgPkg->m_nPkgType)
	{
		m_nHeartBeatState = enHeartBeat_Timer_ON;
		LOG1(SourceInfo, DebugUtil::DebugMsg, "set HeartBeat timer begin [%s]", m_strClientID.c_str());
		m_pHeartBeatTimer->workBegin();
	}*/
	
	if (NULL != m_pObserverRcvMsg)
	{
		m_lstRecvMsgPackage.erase(iterMsgPkg);
		strServerID = defDefault_ServerIDName;
		m_pObserverRcvMsg->processRcvFrame(strServerID, pEachMsgPkg);
	}
	else
	{
		LOG2(SourceInfo, DebugUtil::DebugError, 	
			"m_pServerObserverRcvMsg is NULL,[%s][PkgID=%s] sever delete one Frame ", 	
			m_strClientID.c_str(), pEachMsgPkg->m_strPkgUUID32.c_str());
		
		//erase this iter from list
		m_lstRecvMsgPackage.erase(iterMsgPkg);

		//m_pServerObserverRcvMsg is NULL then free this  MsgPkg
		nFunRes = _FreeLstMsgItems(pEachMsgPkg->m_lstAllMsgItems);	
		DEF_DELETE(pEachMsgPkg);
		pEachMsgPkg = NULL;  

	}
		
	return nFunRes;
}


void CSrvComWorker::run()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CSrvComWorker  begin run()");

	while (false == m_toTerminate)
	{
		_ThreadJob();
		//TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP); //why need this sleep
	}


	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CSrvComWorker  end run()");

}
void CSrvComWorker::terminate()
{
	m_toTerminate = true;
}

int	CSrvComWorker::_ThreadJob()
{
	int  nFunRes = 0;

	nFunRes = _SocketRead();
	if (-1 == nFunRes)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "err:read data from socket error!");
		nFunRes = -1;
		m_toTerminate=true;  //add by luohr, thread should be terminated if connection is lost
		return nFunRes;
	}
	else if (TCP_NODATAFORREAD == nFunRes)
	{
		TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);	
	}

	
	_ProcessRecvBuffer();
	_ProcessRcvMsgPackages();
	_CheckIntValue();	
	_SendNotifyHeartBeat();

	return nFunRes;
}


void CSrvComWorker::registerObserver( IObserverRcvMsg* Handler )
{ 
	TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	m_pObserverRcvMsg = Handler;
}


void CSrvComWorker::removeObserver()
{ 
    TA_Base_Core::ThreadGuard guard(m_SocketReadLock);
	m_pObserverRcvMsg = NULL;
}

bool CSrvComWorker::isTCPConnected()
{
	TA_Base_Core::ThreadGuard guard(m_SocketInfoLock);
	return m_isConnected;
}

int CSrvComWorker::setAgtInfo(PAQWorkInfo pDeqEnqInfo)
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guardRead(m_SocketReadLock);

	m_strClientID = pDeqEnqInfo->m_strClientID;
		
	LOG2(SourceInfo, DebugUtil::DebugDebug, 
		"Server Com Worker setAgtInfo [ClientID=%s][SRVID=%d]", 
		m_strClientID.c_str(), pDeqEnqInfo->m_nINSRVID);

	return nFunRes;
}


void CSrvComWorker::_CheckIntValue()
{	
	if (m_nLogCount > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nLogCount	= 0;
	}
}

void CSrvComWorker::_SendNotifyHeartBeat()
{	
	if (HeartBeat_Switch_ON  == g_system_variables.m_nHeartBeatSwitch /*&& enHeartBeat_Timer_ON == m_nHeartBeatState*/)
	{
		// send heartbeat package when state is start and interval >= interval
		// the default interval is 2seconds
		ulong64 uTimeDelt = m_pHeartBeatTimer->getNotworkTime();
		if (uTimeDelt >= g_system_variables.m_nHeartBeatInterval)
		{
			LOG0(SourceInfo, DebugUtil::DebugDebug, "sending heartbeat package...");
			MSGPKG* pHeartBeatPkg = NULL;
			_BuildHeartPkg(pHeartBeatPkg);
			serverSendMsgPkg(pHeartBeatPkg);
			pHeartBeatPkg = NULL;
			m_pHeartBeatTimer->workEnd();
			m_pHeartBeatTimer->workBegin();
		}		
	}
}

void CSrvComWorker::_InitHeartBeatTimer()
{
	DEF_DELETE(m_pHeartBeatTimer);

	if (HeartBeat_Switch_ON == g_system_variables.m_nHeartBeatSwitch)
	{
		m_pHeartBeatTimer = new CWorkTimeLock(HEARTBEAT_TIMER);
		m_pHeartBeatTimer->workBegin();
	}
}

void CSrvComWorker::_BuildHeartPkg(MSGPKG*& pMsgPkg)
{
	pMsgPkg = new MSGPKG();
	pMsgPkg->m_bZip = false;
	pMsgPkg->m_nPkgType = MSGPACKAGE_PT_NOTIFY;

	std::string strUUID36   = TA_Base_Core::TA_Uuid::generateUUID();
	std::string strUUID32   = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	pMsgPkg->m_strPkgUUID32 = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	
	LOG2(SourceInfo, DebugUtil::DebugMsg, 
		"Start Send Heart Beat [ClientID=%s][PkgID=%s]", 
		m_strClientID.c_str(), pMsgPkg->m_strPkgUUID32.c_str());

}

NS_END(TA_DB_Sync)



























