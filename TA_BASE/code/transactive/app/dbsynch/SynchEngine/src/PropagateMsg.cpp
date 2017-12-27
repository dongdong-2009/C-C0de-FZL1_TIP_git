//////////////////////////////////////////////////////////////////////
///		@file		PropagateMsg.cpp
///		@author		zhilin,ouyang
///		@date		2012-01-29 14:44:41
///
///		@brief	    implementation of class CPropagateMsg.. 
///                 each consumer per thread in the system.
///                 
//////////////////////////////////////////////////////////////////////
#include "PropagateMsg.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "DatabaseImpl.h"
#include "Listener.h"
#include "Consumer.h"
#include "Message.h"
#include "MsgHelper.h"
#include "MsgDispatcher.h"
#include "ClientComWorker.h"
#include "ProcessRcvMsg.h"
#include "WorkTime.h"
#include "CMysqlEnv.h"


#if !defined( WIN32 )
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif // !WIN32

#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbSyncException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/uuid/src/TAuuid.h"

NS_BEGIN(TA_DB_Sync)

static const uint32  s_uDeqCnt = 3000;   // deq 3K once from MySQL DB

CPropagateMsg::CPropagateMsg()
: m_bTerminate(false)
, m_pListener(0)
, m_pMsgConsumer(0)
, m_nTcpState(TCP_DISCONNECTED)
, m_pMsgObserver(NULL)
, m_ulMsgSent(0)
, m_nDBType(enumMysqlDb)
, m_nPropagateFlag(STATE_CODE_INIT_ENV)
, m_pDeqDatabase(NULL)
, m_pProTHD(NULL)
, m_pConAck(NULL)
, m_pEnqAck(NULL)
, m_pComWork(NULL)
, m_pEnqReqProcessor(NULL)
, m_nLogCount(0)
, m_nDBSrvID(0)
, m_nMsgSrvID(0)
, m_nReconTimes(0)
, m_nConFailTimes(0)
{	
	m_pMsgObserver = new CMsgDispatcher();
	m_pMsgItemHelper = new CMsgItemHelper();
	//create enqueue thread, will be started soon
	m_pEnqReqProcessor = new CProcessRcvMsg();	
	m_nDeqWorkIndex = 0;
	
	m_pTimelogEachDeqWork = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_EACHDEQWORK);
	m_pTimelogDeqData = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_DEQDATA);
	m_pTimelogSendData = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_SENDDATA);
	m_pTimelogWaitACK = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_WAITACK);
	m_pTimelogDeqCommit = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_DEQCOMMIT);
	m_pTimeConAck = new CWorkTimeNoLock(PROPAGATE_TIMER_CONACK);
		
}

CPropagateMsg::~CPropagateMsg(void)
{
	_ClearUpEnv();	

	m_pEnqReqProcessor->terminateAndWait();	

	_StopComWork();

	DEF_DELETE(m_pMsgItemHelper);
	DEF_DELETE(m_pEnqReqProcessor);
	DEF_DELETE(m_pMsgObserver);
	DEF_DELETE(m_pTimelogEachDeqWork);
	DEF_DELETE(m_pTimelogDeqData);
	DEF_DELETE(m_pTimelogSendData);
	DEF_DELETE(m_pTimelogWaitACK);
	DEF_DELETE(m_pTimelogDeqCommit);
	DEF_DELETE(m_pTimeConAck);

}
int  CPropagateMsg::setPropagateTHD(PRO_THD* pProThd)
{
	FUNCTION_ENTRY("setPropagateTHD");
	int nRetCode = Ret_Success;

	if (!pProThd)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	m_pProTHD = pProThd;

	FUNCTION_EXIT;
	return nRetCode;
}

int  CPropagateMsg::processRcvFrame(PCONACK pConAck)
{
	FUNCTION_ENTRY("processRcvFrame");
	int nRetCode = Ret_Success;

	if ( !pConAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	{
		ThreadGuard  guard(m_tLockAcks);
		m_pConAck = pConAck;
		m_strAckPkgID = m_pConAck->m_strPkgID;
	}	

	FUNCTION_EXIT;
	return nRetCode;
}

int   CPropagateMsg::processRcvFrame(PENQACK pEnqAck)
{
	FUNCTION_ENTRY("processRcvFrame");
	int nRetCode = Ret_Success;

	if ( !pEnqAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	
	{
		ThreadGuard  guard(m_tLockAcks);
		m_pEnqAck = pEnqAck;
		m_strAckPkgID = m_pEnqAck->m_strPkgID;
	}	

	FUNCTION_EXIT;
	return nRetCode;
}

int   CPropagateMsg::processTcpEvent( int nStateCode )
{
	FUNCTION_ENTRY("processTcpEvent");
	int nRet = Ret_Success;

	if (m_nTcpState != nStateCode)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Old Tcp Connection State Code: %d; New Tcp connection State Code: %d", m_nTcpState,  nStateCode);
		m_nTcpState = nStateCode;		
	}

	FUNCTION_EXIT;
	return nRet;
}

void  CPropagateMsg::run()
{	
	FUNCTION_ENTRY("run");	  
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"start one thread: Propagate, [Queue Name: %s][SubID: %d][Receiver Name: %s][Receiver Address: %s]", 
		m_pProTHD->m_szQName,
		m_pProTHD->m_nSubID, 
		m_Receiver.getName().c_str(),
		m_Receiver.getAddr().c_str());
	//start enqueue thread
	m_pEnqReqProcessor->start();
	int uPrintLog = 0;

	while ( !m_bTerminate )
	{	
		_ThreadJob();
		_PrintMsgLog(uPrintLog, m_nPropagateFlag);

		sleep( defDefaultListenTime );
	}

	if (m_bTerminate)
	{		
		m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ);
		m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK);
	}

	// end of the thread, need release the mysql resource.
	CMysqlEnv::getInstance().connThreadEnd();

	FUNCTION_EXIT;
}

void CPropagateMsg::terminate()
{
	FUNCTION_ENTRY("terminate");	

	m_bTerminate = true;		

	FUNCTION_EXIT;
}

void  CPropagateMsg::_ThreadJob()
{
	if (m_nDeqWorkIndex > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nDeqWorkIndex = 0;
	}

	switch (m_nPropagateFlag)
	{
	case STATE_CODE_INIT_ENV:
		_ProcessInitEnv();
		break;
	case STATE_CODE_CONNTOSRV:
		_ProcessCon2Srv();
		break;
	case STATE_CODE_SENDINFO:
		_ProcessSendInfo();
		break;
	case STATE_CODE_INITDEQ:
		_ProcessInitDeq();
		break;
	case STATE_CODE_DEQMSG:
		_ProcessDeqMsg();
		break;
	case STATE_CODE_SENDMSGREQ:
		_ProcessSendMsgReq();
		break;
	case STATE_CODE_WAITFORENQACK:
		_ProcessWaitForEnqAck();
		break;
	case STATE_CODE_WAITFORCONACK:
		_ProcessWaitForConAck();
		break;
	case STATE_CODE_RCVENQACK:
		_ProcessRcvEnqAck();
		break;
	case STATE_CODE_RCVCONACK:
		_ProcessRcvConAck();
		break;
	case STATE_CODE_FINISH_TRANSACTION:
		_ProcessFinishTransaction();
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));
	}
}

void  CPropagateMsg::_ProcessInitEnv()
{
	_ClearUpEnv();
	_GetSrvID();

	if (m_pProTHD->m_lstRecver.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Receiver_Lst_Null, defErrMsg(Err_Receiver_Lst_Null));
		return;
	}

	m_Receiver = *(m_pProTHD->m_lstRecver.front());
	_ParseSrvAddr(m_Receiver.getAddr());

	if (m_strSrvAddr.empty() || m_strSrvPort.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
		return;
	}

	m_strServerID = m_strSrvAddr + ":" + m_strSrvPort + ":" + m_pProTHD->m_szQName;
	if (m_strServerID.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Create_TCPCon_Failed, defErrMsg(Err_Create_TCPCon_Failed));
		return;
	}	
	_StartComWork();  // start the communication thread

	m_pEnqReqProcessor->setServerID(m_strServerID);	
	m_pEnqReqProcessor->setComWorkHander(m_pComWork);
	m_pEnqReqProcessor->setRcvPara(m_pProTHD->m_szQName, m_Receiver.getName());
		  
	m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK ,this);
	m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ, m_pEnqReqProcessor);

	m_nPropagateFlag = STATE_CODE_CONNTOSRV;
}

void  CPropagateMsg::_ProcessCon2Srv()
{
	if (m_strServerID.empty() || !m_pComWork)
	{
		m_nPropagateFlag = STATE_CODE_INIT_ENV;
	}
	int nRetCode = Ret_Success;
	uint32 uiLog = 0;	

    //connect to server
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"begin Connect to server, %s", m_strServerID.c_str());
	nRetCode = m_pComWork->connectToServer(TIME_BASE_SECOND);
	if ( Ret_Success != nRetCode )	  //fail to connect to server
	{	
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "fail to Connect to server, %s", m_strServerID.c_str());
		if (m_nReconTimes > defChannelSrvTryTimes && m_pProTHD->m_lstRecver.size() > 1)
		{
			// find next available receiver address(server address)
			_GetWorkAgent();

			// stop the previous client TCP connection 
			_StopComWork();
 
			m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK);
			m_pMsgObserver->unRegisterHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ);

			_ParseSrvAddr(m_Receiver.getAddr());
			if (m_strSrvAddr.empty() || m_strSrvPort.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
				return;
			}
              
			m_strServerID = m_strSrvAddr + ":" + m_strSrvPort + ":" + m_pProTHD->m_szQName;
			_StartComWork();  // create a new TCP connection
			if (m_strServerID.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Error Code: %d, Error message: %s", Err_Create_TCPCon_Failed, defErrMsg(Err_Create_TCPCon_Failed));
				return;
			}

			// remove the previous one first and then register the new one
			m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_CON_ACK | ComProtocol_STOC_ENQ_ACK ,this);
			m_pMsgObserver->registerHandler(m_strServerID, ComProtocol_STOC_ENQ_REQ, m_pEnqReqProcessor);			
		}

		m_nReconTimes > defChannelSrvTryTimes ? m_nReconTimes = 1 : m_nReconTimes++;
		sleep(TIME_SECOND_TWO);	
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Retry connect to server: %s, Times: %d", m_Receiver.getAddr().c_str(),  m_nReconTimes);
	}
	else
	{
		m_nReconTimes = 0;
		m_pEnqReqProcessor->setServerID(m_strServerID);
		m_pEnqReqProcessor->setComWorkHander(m_pComWork);
		m_nPropagateFlag = STATE_CODE_SENDINFO;
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"end Connect to server, %s", m_strServerID.c_str());
	}		

}

void  CPropagateMsg::_ProcessSendInfo()
{
	std::string strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	std::string strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32   = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	m_strReqPkgID	= strUUID32;

	int  nRetCode = Ret_Failed;
	CONREQ conReq;
	PMSGPKG pMsgPkg      = NULL;
	conReq.m_strAgtName  = (m_pProTHD->m_Sender).getName();
	conReq.m_strQName    = m_pProTHD->m_szQName;
	conReq.m_strHostName = _GetLocHostName();
	conReq.m_strPkgID    = m_strReqPkgID;
	conReq.m_nOServerID  = m_nDBSrvID;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Send handshake to: %s, Agent Name: %s, Queue Name: %s, PackageID: %s", m_Receiver.getAddr().c_str(), m_pProTHD->m_Sender.getName().c_str(), 
		m_pProTHD->m_szQName, m_strReqPkgID.c_str());	

	m_pMsgItemHelper->setCTOSConReq(&conReq, pMsgPkg);	

	ThreadGuard  guard(m_tLockAcks);
	DEF_DELETE(m_pConAck);  // clean connection ack before sending connection request
	
	// clean shake hand ack timer monitor
	m_pTimeConAck->workBegin();
	
	nRetCode = m_pComWork->clientSendData(m_strServerID, pMsgPkg);
	if ( Ret_Success == nRetCode )
	{
		m_nPropagateFlag = STATE_CODE_WAITFORCONACK;
	}
	else
	{		 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"ERR send handshake : %s, Agent Name: %s, Queue Name: %s, PackageID: %s", m_Receiver.getAddr().c_str(), m_pProTHD->m_Sender.getName().c_str(), 
			m_pProTHD->m_szQName, m_strReqPkgID.c_str());	
		if (TCP_DISCONNECTED == m_nTcpState)
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;

		sleep(TIME_SECOND_TWO);
	}	

}

void  CPropagateMsg::_ProcessInitDeq()
{
	if (!m_pProTHD)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}	

	_InitDeqEnv();	

	m_nPropagateFlag = STATE_CODE_DEQMSG;
}

void  CPropagateMsg::_ProcessDeqMsg()
{
	bool bRetCode = false;
	int  nRetCode = Ret_Failed;
	try
	{
		// check if the TCP connected or not
		if (TCP_DISCONNECTED == m_nTcpState)
		{
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;
			return;
		}
		// check if there still something to send from previous
		if (!m_stEnqReq.m_lstDBMsg.empty() || !m_lstSendMsg.empty()) 
		{
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			return;
		}

		bRetCode = m_pListener->listen();
		if (bRetCode) //has data
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Begin Deq message for SubID: %d, Queue name: %s",  m_pProTHD->m_nSubID, m_pProTHD->m_szQName);
				
			m_pTimelogEachDeqWork->workBegin();
			m_pTimelogDeqData->workBegin();	
			
			if ( enumOracleDb == m_nDBType)
				nRetCode = m_pMsgConsumer->deqMsg(m_lstSendMsg, defMaxPackageSize);
			else
				nRetCode = m_pMsgConsumer->deqMsg(m_lstSendMsg, s_uDeqCnt, m_pProTHD->m_nSubID);
			switch ( nRetCode )
			{
			case ER_MYSQL_GONE_AWAY:
				m_nPropagateFlag = STATE_CODE_INITDEQ;
				break;
			case Ret_Success:
				m_nPropagateFlag = STATE_CODE_SENDMSGREQ;	
				break;
			default:
				m_nPropagateFlag = STATE_CODE_DEQMSG;
				break;
			}
			
			m_nStepDeqDataTime = m_pTimelogDeqData->workEnd();
			m_nMsgCountEachDeq = m_lstSendMsg.size();
			if (m_nMsgCountEachDeq > 0)
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"[End Deq message for SubID: %d][DeqWork Message count: %llu][Queue name: %s][DeqWorkIndex: %d][Deq time: %llu ms][first SID: %llu, last SID: %llu]",  
				m_pProTHD->m_nSubID,  m_nMsgCountEachDeq, m_pProTHD->m_szQName, m_nDeqWorkIndex, 
				m_nStepDeqDataTime, m_lstSendMsg.front()->getMsgSeqID(), m_lstSendMsg.back()->getMsgSeqID());
		}
		else
		{
			m_nPropagateFlag = STATE_CODE_DEQMSG;
		}		
	}
	catch (TA_Base_Core::DataException& )
	{
		//continue to dequeue
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Data exception...");
	}
	catch (const CDBException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", e.getNativeCode(), e.getReason().c_str());
		int nErrCode = e.getNativeCode();
		if ( ER_MYSQL_GONE_AWAY == nErrCode ||
			ER_MYSQL_CANNT_CONNECT == nErrCode)
			m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
	catch (const DatabaseException& e)
	{
		//continue to dequeue
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error message: %s", e.what());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception...");
		m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
		
}

void  CPropagateMsg::_ProcessSendMsgReq()
{

	int  nRet = Ret_Failed;
	uint32 uDataLen = 0;
	
	m_pTimelogSendData->workBegin();

	if (TCP_DISCONNECTED == m_nTcpState)
	{
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		return;
	}
	if ( m_lstSendMsg.empty() )
	{
		m_nPropagateFlag = STATE_CODE_DEQMSG;
		return;
	}
	else
	{
		m_nMsgSrvID = m_lstSendMsg.front()->getOServerID();
	}

	if (m_stEnqReq.m_lstDBMsg.empty())
	{
		lstDBMsgConIterT iter = m_lstSendMsg.begin();
		while (iter != m_lstSendMsg.end())
		{
			if ((*iter)->getOServerID() != m_nMsgSrvID) break;
			if (uDataLen > g_system_variables.m_PackageSize) break;
			uDataLen += (*iter)->getPayloadSize();
			m_stEnqReq.m_lstDBMsg.push_back(*iter);

			iter++;
		}		
	}

	std::string strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	std::string strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32   = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	m_strReqPkgID	= strUUID32;
	m_stEnqReq.m_strPkgID = m_strReqPkgID;
	PMSGPKG pMsgPkg = NULL;	

	m_pMsgItemHelper->setCTOSEnqReq(&m_stEnqReq, pMsgPkg);

	LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"[start send message count: %ld][Package ID: %s]", 
		m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str() );

	ThreadGuard  guard(m_tLockAcks);
	DEF_DELETE(m_pEnqAck);   // clean reply ack before sending request
	nRet = m_pComWork->clientSendData(m_strServerID, pMsgPkg);
	if ( Ret_Success == nRet )
	{
		m_nPropagateFlag = STATE_CODE_WAITFORENQACK;		
		
		m_nStepSendDataTime = m_pTimelogSendData->workEnd();
		
		LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[DeqWorkIndex=%d][StepSendDataTime=%llu ms][msg count=%ld]", 
			m_nDeqWorkIndex, m_nStepSendDataTime, m_stEnqReq.m_lstDBMsg.size());
	
		m_pTimelogWaitACK->workBegin();
	}
	else
	{
		if (TCP_DISCONNECTED == m_nTcpState)
			m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		else
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ; //?? why go to this statu?
	}

	pMsgPkg = NULL;

}

void  CPropagateMsg::_ProcessWaitForEnqAck()
{
	ulong64 uiTimeDiff = 0;
	if (TCP_DISCONNECTED == m_nTcpState) 
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Tcp connection lost while waiting enqueue ack.");

		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
	}
	
	uiTimeDiff = m_pTimelogWaitACK->getNotworkTime();
	if (uiTimeDiff > TIME_MIN_FIFTEEN) 
	{
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;	
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"wait for Enqueue ack time out, reconnect...");	
		return; //otherwise the flag may change to STATE_CODE_RCVENQACK.
	}
	ThreadGuard  guard(m_tLockAcks); //add lock
	if (m_pEnqAck)
	{
		m_nPropagateFlag = STATE_CODE_RCVENQACK;
		
		m_nStepWaitACKTime = m_pTimelogWaitACK->workEnd();
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[DeqWorkIndex=%d][StepWaitACKTime=%llu ms]", m_nDeqWorkIndex, m_nStepWaitACKTime);
	}
     
}

void  CPropagateMsg::_ProcessWaitForConAck()
{
	ulong64 uiTimeDiff = 0;
	if (TCP_DISCONNECTED == m_nTcpState) 
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Tcp connection lost while waiting handshake ack.");

		m_nPropagateFlag = STATE_CODE_CONNTOSRV;

		if (Tcp_ConFailMon_Switch_OFF == g_system_variables.m_nConFailedMonSwitch) return;

		if (m_nConFailTimes > defTCPSeqFailTimes)
		{
			// restart after failed 20 times because of tcp sequence out of order
			// connect successfully and then disconnected immediately
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Tcp connection lost 20 times while waiting handshake ack.");
			sleep(TIME_SECOND_TEN);
			exit(Ret_Success);
		}
		else
		{
			m_nConFailTimes++;
		}		
	}
	
	uiTimeDiff = m_pTimeConAck->getNotworkTime();
	if (uiTimeDiff > TIME_MIN_FIVE) 
	{
		m_nPropagateFlag = STATE_CODE_CONNTOSRV;
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"handshake time out, reconnect...");	
		return; //otherwise the flag may change to STATE_CODE_RCVCONACK.
	}
	ThreadGuard  guard(m_tLockAcks);
	if (m_pConAck)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "go to handle handshake ack msg");	
		m_nPropagateFlag = STATE_CODE_RCVCONACK;
	}
}

void  CPropagateMsg::_ProcessRcvEnqAck()
{
	ThreadGuard  guard(m_tLockAcks);
	if ( !m_pEnqAck )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if ( m_strReqPkgID == m_strAckPkgID && 
		 m_stEnqReq.m_lstDBMsg.size() == m_pEnqAck->m_nEnqReqACK )
	{
		m_ulMsgSent += m_pEnqAck->m_nEnqReqACK;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Success propagated to %s,  Message count: %ld][total sent messages: %llu][Package ID: %s]",
			m_Receiver.getName().c_str(), m_pEnqAck->m_nEnqReqACK, m_ulMsgSent, m_strReqPkgID.c_str());
		
		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
			
		m_pTimelogDeqCommit->workBegin();

	}
	else
	{
		switch (m_pEnqAck->m_nEnqReqACK)
		{
		case Err_EnqReqACkCode_DiskFull:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBServer_DiskFull, defErrMsg(Err_DBServer_DiskFull), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_MIN_TWO);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_DBOffLine:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBOffLine, defErrMsg(Err_DBOffLine), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_THIRTY);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_EnqDisable:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBEnqOpt_Disable, defErrMsg(Err_DBEnqOpt_Disable), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_THIRTY);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		case Err_EnqReqACkCode_PLTypeInvalid:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_DBQ_PLT_Invalid, defErrMsg(Err_DBQ_PLT_Invalid), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());
			
			sleep(TIME_MIN_TWO);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s, Sent Count: %ld, Package ID: %s", 
				Err_Unknown, defErrMsg(Err_Unknown), m_stEnqReq.m_lstDBMsg.size(), m_strReqPkgID.c_str());

			sleep(TIME_SECOND_TEN);
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
			break;
		}

	}

	
	DEF_DELETE( m_pEnqAck );   // After processing, clean the reply ack.
}

void  CPropagateMsg::_ProcessRcvConAck()
{
	ThreadGuard  guard(m_tLockAcks);
	if ( !m_pConAck)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Rcv Handshake ,Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	if (m_strReqPkgID == m_strAckPkgID &&
		ConReqAckCode_Succeed == m_pConAck->m_nConReqAck)
	{
		m_pTimeConAck->workEnd();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[HandShake success connected to server: %s][Package ID: %s]", m_Receiver.getName().c_str(), m_strReqPkgID.c_str());

		m_nPropagateFlag = STATE_CODE_INITDEQ;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"[HandShake failed,wrong package id,server id: %s][Package ID: %s]", m_Receiver.getName().c_str(),  m_strReqPkgID.c_str());

		m_nPropagateFlag = STATE_CODE_SENDINFO;
	}


	 
	DEF_DELETE( m_pConAck );  // After processing, clean the reply ack.
	m_nConFailTimes = 0;      // clean after connect and send handshake to server successfully
}

void  CPropagateMsg::_ProcessFinishTransaction()
{

	try
	{
		std::string strConsumer;
		nPairT pairSID;
		pairSID.first = m_stEnqReq.m_lstDBMsg.front()->getMsgSeqID();
		pairSID.second = m_stEnqReq.m_lstDBMsg.back()->getMsgSeqID();
		m_pDeqDatabase->connect();
		m_pDeqDatabase->aqCommit(m_pProTHD->m_szQName, m_pProTHD->m_nSubID, strConsumer, pairSID);
		size_t uSentCnt = m_stEnqReq.m_lstDBMsg.size();
		for (size_t i = 0; i < uSentCnt; i++)
		{			
			m_lstSendMsg.pop_front();
		}
		_FreeMsgCon(m_stEnqReq.m_lstDBMsg);

		if (m_lstSendMsg.empty())
		{
			m_nPropagateFlag = STATE_CODE_DEQMSG;
		}
		else
		{
			m_nPropagateFlag = STATE_CODE_SENDMSGREQ;
		}

		m_nStepDeqCommitTime = m_pTimelogDeqCommit->workEnd();
		LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"[DeqWorkIndex=%d][StepDeqCommitTime=%llu ms][Msg count=%d]", m_nDeqWorkIndex, m_nStepDeqCommitTime, uSentCnt); 

		LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Finish the transaction for Queue: %s, Package ID: %s, firstMsgSeqID=%llu, lastMsgSeqID=%llu, message count: %d]", 
			m_pProTHD->m_szQName, m_strReqPkgID.c_str(), pairSID.first, pairSID.second, uSentCnt);
	
	
		if (STATE_CODE_DEQMSG == m_nPropagateFlag)
		{
			m_nEachDeqWorkUse = m_pTimelogEachDeqWork->workEnd();
			LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"[DeqWorkIndex=%d][EachDeqWorkUse=%llu ms][DeqWork Msg count=%llu]", 
				m_nDeqWorkIndex, m_nEachDeqWorkUse, m_nMsgCountEachDeq); 

			m_nDeqWorkIndex++;
		}
	
	}	
	catch (const CDBException& e) //TODO:: if database has problem, dead loop?	*** need to verify this code
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
		"Error Code: %d, Error message: %s", e.getNativeCode(), e.getReason().c_str());

		int nErrCode = e.getNativeCode();
		if ( ER_MYSQL_GONE_AWAY == nErrCode ||
			 ER_MYSQL_CANNT_CONNECT == nErrCode)
			_InitDeqEnv(Ret_Failed);

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}
	catch (const DatabaseException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error message: %s", e.what());

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception...");

		_InitDeqEnv(Ret_Failed);

		m_nPropagateFlag = STATE_CODE_FINISH_TRANSACTION;
	}	
}

void  CPropagateMsg::_ParseSrvAddr(const std::string& strAddr)
{
	if ( !m_pProTHD )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}
	if (strAddr.empty())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Server_IP_Addr_Not_Found, defErrMsg(Err_Server_IP_Addr_Not_Found));
		return;
	}

	std::string::size_type nPos = strAddr.find_first_of(defConfig_DbConStrTag);
	if ( std::string::npos == nPos )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr));
		TA_THROW(TA_Base_Core::SrvInfoNotFound(Err_Invalid_Addr, defErrMsg(Err_Invalid_Addr)));
	}
	std::string strSubTemp(strAddr.substr(0, nPos));
	m_strSrvAddr   = trim(strSubTemp);
	strSubTemp     =  strAddr.substr(nPos + 1);
	m_strSrvPort   = trim(strSubTemp);
}

std::string CPropagateMsg::_GetLocHostName()
{
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
	return std::string( unqualifiedHostName );
}

template<class MsgConT>
void  CPropagateMsg::_FreeMsgCon(MsgConT& rMsgCon)
{
	typename MsgConT::iterator  iter = rMsgCon.begin();

	while ( iter != rMsgCon.end() )
	{
		DEF_DELETE(*iter);
		iter++;
	}
	rMsgCon.clear();
}

void  CPropagateMsg::_ClearUpEnv()
{
	m_nReconTimes   = 0;
	m_nConFailTimes = 0;
	m_strReqPkgID.clear();
	m_strAckPkgID.clear();
	m_stEnqReq.m_lstDBMsg.clear();
	_FreeMsgCon(m_lstSendMsg);
	DEF_DELETE(m_pConAck);
	DEF_DELETE(m_pEnqAck);
	DEF_DELETE(m_pDeqDatabase);
	DEF_DELETE(m_pMsgConsumer);
	DEF_DELETE(m_pListener);
}

void  CPropagateMsg::_InitDeqEnv(int nStatus)
{
	if (!m_pDeqDatabase || Ret_Failed == nStatus )
	{		
		DEF_DELETE(m_pDeqDatabase);

		m_pDeqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);		
	}
	DEF_DELETE(m_pListener);
	DEF_DELETE(m_pMsgConsumer);

	m_pMsgConsumer = new CConsumer(m_pDeqDatabase);
	m_pListener    = new CListener(m_pDeqDatabase, m_pProTHD->m_szQName);

	if (!m_pListener || !m_pMsgConsumer )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr));
		TA_THROW(TA_Base_Core::InValidPtr(Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr)));
	}

	m_nDBType      = m_pDeqDatabase->getDbServerType();
	m_pMsgConsumer->setPayloadType(m_pProTHD->m_nPayloadType);

	m_pListener->setSubID(m_pProTHD->m_nSubID);
	m_pListener->setListenTimeOut(MSG_LISTEN_NOWAIT);
	m_pMsgConsumer->setQueueName(m_pProTHD->m_szQName);
	m_pMsgConsumer->setDeqMode(DEQ_REMOVE);
	m_pMsgConsumer->setPositionOfMsg(DEQ_FIRST_MSG);
	m_pMsgConsumer->setAgent(m_Receiver);
	m_pMsgConsumer->setPayloadType(m_pProTHD->m_nPayloadType);
}

void  CPropagateMsg::_GetWorkAgent()
{
	lstAgentContIterT iter = m_pProTHD->m_lstRecver.begin();
	while (iter != m_pProTHD->m_lstRecver.end())
	{
		if (*(*iter) == m_Receiver) 
		{
			if (m_pProTHD->m_lstRecver.end() == ++iter)					
				m_Receiver = *(m_pProTHD->m_lstRecver.front());					
			else
				m_Receiver = *(*iter);
			break;
		}
		
		iter++;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"Current Receiver: %s, Address: %s, Receiver list size: %d",  m_Receiver.getName().c_str(),  m_Receiver.getAddr().c_str(), m_pProTHD->m_lstRecver.size());

}

void  CPropagateMsg::_GetSrvID()
{
	if (m_pDeqDatabase)
	{
		m_nDBSrvID = m_pDeqDatabase->getDBServerID();
	}
	else
	{		
		m_pDeqDatabase = new CDatabaseImpl(g_system_variables.m_SrcDbConStr);
		m_pDeqDatabase->connect();
		m_nDBSrvID = m_pDeqDatabase->getDBServerID();
	}
}

void  CPropagateMsg::_StartComWork()
{
	if (!m_pComWork)
	{
		m_pComWork = new CClientComWorker(m_strSrvAddr, m_strSrvPort, m_pProTHD->m_szQName, m_strServerID);		
		if (m_pMsgObserver)
		{
			m_pComWork->registerObserver(m_pMsgObserver);
		}
		m_pComWork->managerStart();
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "the previous ComWork still exists ServerID=%s", m_strServerID.c_str());
	}
}

void  CPropagateMsg::_StopComWork()
{
	if (NULL != m_pComWork)
	{
		m_pComWork->managerTerminateAndWait();
		DEF_DELETE(m_pComWork);
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ComWork not have been started ServerID=%s", m_strServerID.c_str());
	}
	
}

void  CPropagateMsg::_PrintMsgLog(int& nPrintInterval, int nState)
{
	nPrintInterval++;
	if (nPrintInterval > defDefaultMsgPrintTimes)
	{
		nPrintInterval = 0;
		switch (nState)
		{
		case STATE_CODE_INIT_ENV:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_INIT_ENV", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_CONNTOSRV:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_CONNTOSRV", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_SENDINFO:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_SENDINFO", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_INITDEQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_INITDEQ", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_DEQMSG:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_DEQMSG", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_SENDMSGREQ:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_SENDMSGREQ", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_WAITFORENQACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_WAITFORENQACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_WAITFORCONACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_WAITFORCONACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_RCVENQACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_RCVENQACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_RCVCONACK:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_RCVCONACK", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		case STATE_CODE_FINISH_TRANSACTION:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Propagate state: %s, QName: %s, Receiver: %s",  "STATE_CODE_FINISH_TRANSACTION", m_pProTHD->m_szQName, m_Receiver.getName().c_str());
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", Err_Invalid_Propagate_Code, defErrMsg(Err_Invalid_Propagate_Code));

		}
		
	}
}



NS_END(TA_DB_Sync)
