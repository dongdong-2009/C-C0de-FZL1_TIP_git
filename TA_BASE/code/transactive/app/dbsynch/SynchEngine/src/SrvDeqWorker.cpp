#include "SrvDeqWorker.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "TcpConnectionPool.h"
#include "IMsgItem.h"
#include "MsgItemImp.h"
#include "IMsgPackage.h"
#include "MsgPackageImp.h"
#include "SrvComWorker.h"
#include "IDbData.h"
#include "IDatabase.h"
#include "Consumer.h"
#include "DatabaseImpl.h" 
#include "Consumer.h"
#include "MsgHelper.h"
#include "Message.h"

#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/DebugUtil.h"
#include "WorkTime.h"

using TA_Base_Core::DebugUtil;

NS_BEGIN(TA_DB_Sync)


CSrvDeqWorker::CSrvDeqWorker( CSrvComWorker* pComWorker,PAQWorkInfo pDeqEnqInfo, unsigned long ID): m_actionSemaphore(0)
{	
	m_ID=ID;
	m_pComWorkerCopy =	pComWorker;

	m_toTerminate = false;
	m_nLogCount = 0;
	m_pEnqReqEx = NULL;
	m_pEnqReqEx = new ENQREQEX();

	m_pConsumerDataBase = NULL;	 
	m_pCServerConsumer = NULL; 

	m_nCheckSendMsgNum = 0;
	m_nDeqWorkIndex = 0;
	m_nDeqTotal = 0;
	m_pMsgItemHelper = new CMsgItemHelper();

	m_strClientAgtName.clear();
	m_nDeqInitOServerID = -1000;   
	m_strQName.clear();
	m_nLogCount = 0;
	_setAgtInfo(pDeqEnqInfo); 

	m_pMonitorDeqTime = NULL;
	m_pMonitorDeqTime = new CWorkTimeLock(PROPAGATE_TIMER_LOG_MonitorDeqTime);
	m_pMonitorWaitACKTime= new CWorkTimeLock(PROPAGATE_TIMER_LOG_MonitorClientEnqSendAckTime);
	
	m_pTimelogDeqData = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_DEQDATA);
	m_pTimeDeqUpdate = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_DEQUPDATE);
	m_pTimelogDeqCommit = new CWorkTimeNoLock(PROPAGATE_TIMER_LOG_DEQCOMMIT);
	m_nDeqWorkState = server_state_deq_DeqTokenCheck;

	m_pTimeDeqUpdate->workBegin();
}


CSrvDeqWorker::~CSrvDeqWorker()
{
	terminateAndWait();
	m_nLogCount = 0;

	if (NULL != m_pEnqReqEx)
	{
		_FreeMsgVec(m_pEnqReqEx->m_lstDBMsg);
		DEF_DELETE(m_pEnqReqEx);
	}

	m_pComWorkerCopy = NULL;
	DEF_DELETE(m_pMsgItemHelper);

	_FreeRecvPackage();
	DEF_DELETE(m_pMonitorDeqTime);
	DEF_DELETE(m_pMonitorWaitACKTime);
	DEF_DELETE(m_pTimelogDeqData);
	DEF_DELETE(m_pTimeDeqUpdate);
	DEF_DELETE(m_pTimelogDeqCommit);

}

int CSrvDeqWorker::_FreeRecvPackage()
{
	int nFunRes = 0;

	TA_Base_Core::ThreadGuard guard(m_lockLstTcpRecvFrames);

	PENQACK   pOneEnqReplyFrame = NULL;
	lstEnqACKConIterT iterLstFrame;

	iterLstFrame = m_LstTcpRecvFrames.begin();
	while (iterLstFrame != m_LstTcpRecvFrames.end())
	{
		pOneEnqReplyFrame = (*iterLstFrame);
		m_LstTcpRecvFrames.erase(iterLstFrame); 
		DEF_DELETE(pOneEnqReplyFrame);	   
		iterLstFrame = m_LstTcpRecvFrames.begin();	
	}


	return nFunRes;
}



int CSrvDeqWorker::_InitDeqWork()
{
	int nFunRes = 0;
	std::string strSrcDbConStr = g_system_variables.m_SrcDbConStr;

	if (strSrcDbConStr.empty() || m_strClientAgtName.empty())
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: deq error! [%s][DeqWorkIndex=%d][DbConStr=%s][ServerAgtName=%s]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, strSrcDbConStr.c_str(), m_strServerAgtName.c_str());

		nFunRes = -1;
		return nFunRes;
	}

	try
	{
		DEF_DELETE(m_pCServerConsumer);

		if (NULL == m_pConsumerDataBase)
		{
			m_pConsumerDataBase = new TA_AQ_Lib::CDatabaseImpl(strSrcDbConStr.c_str());
		}
		m_pConsumerDataBase->connect();		
		
		m_pCServerConsumer = new TA_AQ_Lib::CConsumer(m_pConsumerDataBase);
		m_pCServerConsumer->setQueueName(m_strQName);
		m_pCServerConsumer->setConsumberName(m_strClientAgtName);
		m_pCServerConsumer->setDeqMode(DEQ_BROWSE);	
		m_pCServerConsumer->setPositionOfMsg(DEQ_NEXT_MSG); 
	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: create DataBaseConsumer err! [%s][DeqWorkIndex=%d][DbConStr=%s][ServerAgtName=%s]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, strSrcDbConStr.c_str(), m_strServerAgtName.c_str());

	}

	m_nCheckSendMsgNum = 0;
	m_nDeqWorkIndex = 0;

	return nFunRes;
}


int CSrvDeqWorker::_UnInitDeqWork()
{
	int nRes = 0;  		
	LOG0(SourceInfo, DebugUtil::DebugDebug,"begin CSrvDeqWorker::_UnInitDeqWork()"); 

	DEF_DELETE(m_pCServerConsumer);
	DEF_DELETE(m_pConsumerDataBase);

	LOG0(SourceInfo, DebugUtil::DebugDebug,"end CSrvDeqWorker::_UnInitDeqWork()"); 

	return nRes;
}

  


int CSrvDeqWorker::_CheckEnvValue()
{
	int nFunRes = 0;

	if (m_nDeqWorkIndex > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nDeqWorkIndex = 0;
	}

	if (m_nDeqTotal > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nDeqTotal = 0;
	}

	if (m_nLogCount > SERVER_LIMIT_LOG_VLAUE_MAX)
	{
		m_nLogCount = 0;
	}	 

	if (false == m_pComWorkerCopy->isTCPConnected())
	{
		if (0 == (m_nLogCount % SERVER_LIMIT_LOG_VLAUE_MOD))
		{
			m_nLogCount = 1;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"err:client disconnected to server[%s][DeqWorkIndex=%d]", 
				m_strClientID.c_str(), m_nDeqWorkIndex);
		}
		m_nLogCount++;
		nFunRes = -1;

		return nFunRes;
	}

	return nFunRes;
}


int CSrvDeqWorker::_DeqDataFormDB(lstDBMsgConT& rVecMsg)
{
	int   nFunRes = 0;

	if (NULL == m_pCServerConsumer)
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "NULL == m_pCServerConsumer"); 
		nFunRes = -1;
		return nFunRes;
	}  	   

	//dequeue one message from AQ 
	m_pCServerConsumer->setDeqMode(DEQ_BROWSE);	//DEQ_REMOVE  DEQ_BROWSE
	m_pCServerConsumer->setPositionOfMsg(DEQ_NEXT_MSG); //DEQ_FIRST_MSG DEQ_NEXT_MSG        
	nFunRes = m_pCServerConsumer->deqMsg(rVecMsg, g_system_variables.m_PackageSize);

	return nFunRes; 
}




void  CSrvDeqWorker::_FreeMsgVec(lstDBMsgConT& rVecMsg)
{
	lstDBMsgConIterT  iter = rVecMsg.begin();
	CMessage* pGet = NULL;

	while ( iter != rVecMsg.end() )
	{
		pGet = *iter;
		DEF_DELETE(pGet);
		iter++;		
	}
	rVecMsg.clear();
}


void CSrvDeqWorker::terminate()
{		
	LOG0(SourceInfo, DebugUtil::DebugInfo, "CSrvDeqWorker::terminate()"); 

	TA_Base_Core::ThreadGuard guard(m_lockLstTcpRecvFrames);
	m_toTerminate = true;
	m_actionSemaphore.post();
}


bool CSrvDeqWorker::serverTerminateAndWait(int nMode)
{
	LOG1(SourceInfo, DebugUtil::DebugInfo, "begin CSrvDeqWorker::serverTerminateAndWait() nMode=%d", nMode); 

	terminateAndWait();

	LOG1(SourceInfo, DebugUtil::DebugInfo, "end CSrvDeqWorker::serverTerminateAndWait() nMode=%d", nMode); 

	return true;
}

bool CSrvDeqWorker::serverStart()
{
	start();
	return true;
}


int	CSrvDeqWorker::_ServerProcessCommit(PENQACK	pReplyFrame)
{
	int						nFunRes = 0;
	std::string             strRecvReplyUUID32;
	int                     nRecvReplyEnqNums = 0; 
	ulong64                 nDeqComitWorkTime;

	//deq commit begin
	m_pTimelogDeqCommit->workBegin();
			
	//get frame data
	strRecvReplyUUID32 = pReplyFrame->m_strPkgID;//getMsgFramePUUID();	
	nRecvReplyEnqNums = pReplyFrame->m_nEnqReqACK;
		
	
	//check whether the reply is the correct one
	if (m_nCheckSendMsgNum != nRecvReplyEnqNums	|| m_strCheckSendMsgUUID32 != strRecvReplyUUID32 )
	{  
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"[%s][DeqWorkIndex=%d] Wrong Reply ! MsgNum[%d] != RecvEnqNums[%d] MsgUUID32[%s] != ReplyUUID32=[%s]",                
			m_strClientID.c_str(), m_nDeqWorkIndex,
			m_nCheckSendMsgNum, nRecvReplyEnqNums,
			m_strCheckSendMsgUUID32.c_str(), strRecvReplyUUID32.c_str());
			
		nFunRes = _DeqUpdateTime(m_pConsumerDataBase, m_strClientAgtName, m_nDeqInitOServerID, m_strQName, NULL);
	 		 
		//commit for sql=select for update
		nFunRes = m_pConsumerDataBase->aqCommit(m_strQName, strRecvReplyUUID32);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Err: commit for sql=select for update error! [PkgID=%s][EnqReqACK=%d]",
				pReplyFrame->m_strPkgID.c_str(), pReplyFrame->m_nEnqReqACK);
		}
		_FreeMsgVec(m_pEnqReqEx->m_lstDBMsg);
		nFunRes = -1;
		return nFunRes;
	}

	LOG4(SourceInfo, DebugUtil::DebugDebug,  
		"DeqGetMsgsCount[%d] == RecvReplyEnqNums[%d] && CheckSendMsgUUID32[%s] == strRecvReplyUUID32[%s]",
		m_nCheckSendMsgNum, nRecvReplyEnqNums, m_strCheckSendMsgUUID32.c_str(), strRecvReplyUUID32.c_str());
	
	//deqMsg DEQ_REMOVE DEQ_FIRST_MSG					
	if (NULL != m_pEnqReqEx)
	{
		LOG2(SourceInfo, DebugUtil::DebugDebug, "deqMsg() DEQ_REMOVE [%s][DeqWorkIndex=%d]", m_strClientID.c_str(), m_nDeqWorkIndex);
		m_pCServerConsumer->setDeqMode(DEQ_REMOVE);	
		m_pCServerConsumer->setPositionOfMsg(DEQ_FIRST_MSG); 
		nFunRes = m_pCServerConsumer->deqMsg(m_pEnqReqEx->m_lstDBMsg, 0);
		
		//free data
		_FreeMsgVec(m_pEnqReqEx->m_lstDBMsg);

		//check	 deqMsg DEQ_REMOVE DEQ_FIRST_MSG result
		if (0 != nFunRes)
		{
			LOG3(SourceInfo, DebugUtil::DebugError, 
				"Err: deqMsg() DEQ_REMOVE have error! [deqmsgRes=%d][%s][DeqWorkIndex=%d]", 
				nFunRes, m_strClientID.c_str(), m_nDeqWorkIndex);	
			//return deq error code	
			return nFunRes;
		}
		
	}//if

	//befor commit( free lock ) need to update deq_token_time
	nFunRes = _DeqUpdateTime(m_pConsumerDataBase, m_strClientAgtName, m_nDeqInitOServerID, m_strQName, NULL);

	//commit for sql=select for update and deqmsg DEQ_REMOVE
	LOG2(SourceInfo, DebugUtil::DebugDebug, "begin Commit [%s][DeqWorkIndex=%d]", m_strClientID.c_str(), m_nDeqWorkIndex);		
	nFunRes = m_pConsumerDataBase->aqCommit(m_strQName, strRecvReplyUUID32);
	//check commit Res
	if (0 != nFunRes)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"[%s][DeqWorkIndex=%d]aqCommit() have error! then [DeqWorkState = server_state_deq_FinishWork]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);	
		nFunRes = -1;	
	}
	else
	{  
		nDeqComitWorkTime = m_pTimelogDeqCommit->workEnd();		
		m_nDeqTotal += 	m_nCheckSendMsgNum;				   
		LOG5(SourceInfo, DebugUtil::DebugDebug, "end Commit [%s][DeqWorkIndex=%d][Msg.size=%d][DeqComitWorkTime=%llu ms][DeqTotal=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum, nDeqComitWorkTime, m_nDeqTotal);		
		m_nDeqWorkIndex++;
		m_nLogCount = 0; //for print log
		nFunRes = 0;
	} 
		
	return nFunRes;	 
}


int	CSrvDeqWorker::_ProceesCountTypeMsg(PENQACK pEnqAck)
{ 	
	TA_Base_Core::ThreadGuard guard(m_lockLstTcpRecvFrames);
	int					nFunRes = 0; 

	m_LstTcpRecvFrames.push_back(pEnqAck);
	pEnqAck = NULL;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"[IDForLog=%s][DeqWorkIndex=%d]recv ack from client[TcpRecvFrames.size=%d]",	
		m_strClientID.c_str(),m_nDeqWorkIndex, m_LstTcpRecvFrames.size());  

	m_actionSemaphore.post();

	return nFunRes;	 
}




int   CSrvDeqWorker::processRcvFrame(PENQACK pEnqAck)
{
	int nFunRes = 0;

	_ProceesCountTypeMsg(pEnqAck);

	return nFunRes;	
}

int   CSrvDeqWorker::processTcpEvent( int nStateCode )
{
	return 0;
}


int	CSrvDeqWorker::_setAgtInfo(PAQWorkInfo pDeqEnqInfo)
{
	int nFunRes = 0;	  

	m_strQName = pDeqEnqInfo->m_strQName;
	m_strClientAgtName = pDeqEnqInfo->m_strClientAgtName;
	m_strServerAgtName = pDeqEnqInfo->m_strServerAgtName;
	m_nDeqInitOServerID =  pDeqEnqInfo->m_nINSRVID;
	m_strClientID = pDeqEnqInfo->m_strClientID;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"Deq setAgtInfo ClientID=%s, ServerAgtName=%s, ServerID=%d", 
		m_strClientID.c_str(), m_strServerAgtName.c_str(), m_nDeqInitOServerID);



	return nFunRes;
}


void CSrvDeqWorker::run()
{
	bool receivedMsg = false;

	LOG0(SourceInfo, DebugUtil::DebugDebug, "CSrvDeqWorker  begin run()");

	m_nDeqWorkState = server_state_deq_DeqTokenCheck;
	
	while( false == m_toTerminate ) 
	{			
		if (0 != _CheckEnvValue())
		{
			//tcp diconnected, stop work
			m_nDeqWorkState = server_state_deq_FinishWork; 
		} 
		
		switch (m_nDeqWorkState)   
		{			 
		case server_state_deq_DeqTokenCheck:
			_ServerPocessDeqTokenCheck();		 
			break;
		case server_state_deq_DeqInit:
			_ServerPocessDeqInit();			 
			break;	 
		case server_state_deq_DeqData:
			_ServerPocessDeqData();
			break;		
		case server_state_deq_WaitForReply:		
			{			 
				TA_Base_Core::ThreadGuard guard(m_lockLstTcpRecvFrames); 			 
				if (!m_LstTcpRecvFrames.empty())
				{	
					receivedMsg = true;				
				}
				else
				{
					receivedMsg = false;
				}
			}				
			if (false == m_toTerminate && false == receivedMsg)	
			{	
				m_actionSemaphore.wait();
			}			 
			_ServerPocesRecvReplyAndComit();     		    
			break;
		case server_state_deq_FinishWork:
			//Dequeue job stopped, so notify TcpConnectionActor for closing client connection;						
			_updateActorDequeueStatusBad();
			m_toTerminate = true;  
			break; 	
		default:
			break;	
		}//switch (m_nDeqWorkState)
		
				
	}//while

	_UnInitDeqWork();

	LOG0(SourceInfo, DebugUtil::DebugDebug, "CSrvDeqWorker  end run()");

}


int CSrvDeqWorker::_ServerPocessDeqTokenCheck()
{
	int		nFunRes = 0;
	std::string strSrcDbConStr = g_system_variables.m_SrcDbConStr;
	
	//MonitorDeqTime begin time here
	m_pMonitorDeqTime->workBegin();
	
	try	
	{
		if (NULL == m_pConsumerDataBase)
		{
			m_pConsumerDataBase = new TA_AQ_Lib::CDatabaseImpl(strSrcDbConStr.c_str());
		}
		m_pConsumerDataBase->connect();	

	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"create oracle connection error![%s][DeqWorkIndex=%d][strSrcDbConStr=%s]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, strSrcDbConStr.c_str());
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState = server_state_deq_FinishWork;
		nFunRes = -1;
		return nFunRes;
	}

	//if no record in table AQ_DEQ_TOKEN_CHECK then insert one
	nFunRes = _DeqTokenCheckInsert(m_pConsumerDataBase, m_strClientAgtName, m_strQName, m_nDeqInitOServerID);
	if (0 != nFunRes)
	{
		//insert record into AQ_DEQ_TOKEN_CHECK have error! 
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"err: insert record into AQ_DEQ_TOKEN_CHECK have error! [%s][DeqWorkIndex=%d][strSrcDbConStr=%s]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, strSrcDbConStr.c_str());
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState = server_state_deq_FinishWork;
		nFunRes = -1;
		return nFunRes;		
	}
	//MonitorDeqTime end time here
	m_pMonitorDeqTime->workEnd();
	m_nDeqWorkState = server_state_deq_DeqInit;

	return nFunRes;
}		

int	CSrvDeqWorker::_ServerPocessDeqInit()
{
	int		nFunRes = 0;
	LOG2(SourceInfo, DebugUtil::DebugDebug, "InitDeq %s,DeqWorkIndex=%d", 
		m_strClientID.c_str(), m_nDeqWorkIndex);		

	nFunRes = _InitDeqWork();

	if (0 == nFunRes)
	{
		m_nDeqWorkState = server_state_deq_DeqData;
	}
	else
	{		 
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "InitDeq()error! %s,DeqWorkIndex=%d", 
			m_strClientID.c_str(), m_nDeqWorkIndex);
		m_nDeqWorkState = server_state_deq_FinishWork;
	}												  

	return nFunRes;
}



int	CSrvDeqWorker::_ServerPocessDeqData()
{
	int	nFunRes = 0;
	ulong64 nDeqDeqWorkTime = 0;
	
	//MonitorDeqTime begin time here
	m_pMonitorDeqTime->workBegin();	  
		  
	//log info
	if (0 == (m_nLogCount % SERVER_LIMIT_LOG_VLAUE_MOD))
	{
		m_nLogCount = 1;
		LOG2(SourceInfo, DebugUtil::DebugDebug,	"DeqDeqMsgs [%s][DeqWorkIndex=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);		
	}
	m_nLogCount++;

	//GetToken and 	update AQ_DEQ_TOKEN_CHECK.UPD_TIME
	nFunRes = _DeqTokenCheckUpdate(m_pConsumerDataBase, m_strClientAgtName, m_strQName, m_nDeqInitOServerID);	   
	if (0 != nFunRes)
	{
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: Deq Token Check have error! [%s][DeqWorkIndex=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);		
		m_nDeqWorkState = server_state_deq_FinishWork; 
		nFunRes = -1; 
		return nFunRes;		
	}
	 
	//lock table AQ_DEQ_TOKEN_CHECK one record for update
	nFunRes = _DeqSelectForUpdate(m_pConsumerDataBase, m_strClientAgtName, m_nDeqInitOServerID, m_strQName);
	if (0 != nFunRes)
	{
		//check error
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Err:select AQ_DEQ_TOKEN_CHECK for update error! [%s][DeqWorkIndex=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);
		//Lost token, stop client connection.
		nFunRes = -1;
		return nFunRes;
	}

	

	//deq data from Oracle DB
	m_pTimelogDeqData->workBegin();
	
	//dequeue message by batch
	/*	
	_DeqDataFormDB return value will be -1 or 0 or ER_ORACLE_AQ_DEQUEUE_TIMEOUT or ER_ORACLE_DequeueFailed 
	-1: means deq have error!
	0 :means deq function is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	ER_ORACLE_DequeueFailed; oracle deq closed
	*/
	nFunRes = _DeqDataFormDB(m_pEnqReqEx->m_lstDBMsg);
	nDeqDeqWorkTime = m_pTimelogDeqData->workEnd();

	//check Deq Result 	
	switch (nFunRes)
	{
	case -1:
		//deq have error
		_FreeMsgVec(m_pEnqReqEx->m_lstDBMsg);
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState=server_state_deq_FinishWork; 
		nFunRes = -1;
		return nFunRes;
		break;
	case ER_ORACLE_DequeueFailed:
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState =server_state_deq_DeqData;
		//Oracle Deq closed, the deq timeout 2 seconds not work, add sleep fun
		TA_Base_Core::Thread::sleep(TIME_BASE_MIN);
		nFunRes = 0;
		return nFunRes;	
		break;
	case 0:
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState =server_state_deq_DeqData;
		//if the deq timeout 2 seconds not work, add sleep fun
		TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);

		nFunRes = 0;
		break;	
	case ER_ORACLE_AQ_DEQUEUE_TIMEOUT:
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState =server_state_deq_DeqData;
		//if the deq timeout 2 seconds not work, add sleep fun
		TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);
		nFunRes = 0;
		break;
	default:
		//deq have other error
		_FreeMsgVec(m_pEnqReqEx->m_lstDBMsg);
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState=server_state_deq_FinishWork; 
		nFunRes = -1;
		return nFunRes;
		break;
	}//switch
	

	if (m_pEnqReqEx->m_lstDBMsg.empty())
	{	
		//deq is ok but not have data continue deq 
		m_nDeqWorkState =server_state_deq_DeqData;
		//free lock table AQ_DEQ_TOKEN_CHECK one record for update
		m_pConsumerDataBase->commitTransaction();
		nFunRes = 0;
		return nFunRes;	
	}

	
	//deq get data then send
	m_nCheckSendMsgNum = (int)m_pEnqReqEx->m_lstDBMsg.size(); 
	//print Deq time to log file
	LOG4(SourceInfo, DebugUtil::DebugDebug, "[%s][DeqWorkIndex=%d][DeqGetMsgsCount=%d][Deq Time=%llu ms]", 
		m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum, nDeqDeqWorkTime);		

	//send request to client
	nFunRes = _ServerPocessSendRequest();
	if (0 != nFunRes)
	{
		//m_nDeqWorkState = server_state_deq_FinishWork;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error: send deq request,[%s][DeqWorkIndex=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);	
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState = server_state_deq_FinishWork;
		nFunRes = -1; 
		return nFunRes;	
	}
	
	//MonitorDeqTime end time here
	m_pMonitorDeqTime->workEnd();

	m_nDeqWorkState = server_state_deq_WaitForReply;
	
	
	//server send request to client, then wait for client send ack back
	m_pMonitorWaitACKTime->workBegin();

	return nFunRes;
}


int	CSrvDeqWorker::_ServerPocessSendRequest()
{
	int						nFunRes = 0;
	std::string				strUUID36;
	std::string				strUUID32;
	MSGPKG*					pGetMsgPkg = NULL;	    
	if (m_pEnqReqEx->m_lstDBMsg.empty())
	{
		nFunRes = -1;
		return nFunRes;
	} 

	//get Msg Frame UUID32
	strUUID36 = TA_Base_Core::TA_Uuid::generateUUID();
	strUUID32 = CUtilityFun::strRemoveUnNeedChar(strUUID36); 
	strUUID32 = CUtilityFun::strtoupper((char*)strUUID32.c_str());
	m_strCheckSendMsgUUID32 = strUUID32;
	m_pEnqReqEx->m_strPkgID = strUUID32;
	//get send EnqRequest Count
	m_nCheckSendMsgNum = (int)m_pEnqReqEx->m_lstDBMsg.size(); 

	LOG4(SourceInfo, DebugUtil::DebugDebug, 
		"send request,[%s][DeqWorkIndex=%d][CheckSendMsgUUID32=%s][CheckSendMsgNum=%d]", 
		m_strClientID.c_str(), m_nDeqWorkIndex, 
		m_strCheckSendMsgUUID32.c_str(), m_nCheckSendMsgNum);


	//send data to client
	try
	{		
		m_pMsgItemHelper->setSTOCEnqReq(m_pEnqReqEx, pGetMsgPkg); 
		nFunRes = m_pComWorkerCopy->serverSendMsgPkg(pGetMsgPkg);
		pGetMsgPkg = NULL;

	}	 
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"failed to send request,[%s][DeqWorkIndex=%d][CheckSendMsgNum=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum);
		nFunRes = -1;
		return nFunRes;
	}	

	//check send data rusult
	if (0 != nFunRes)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"Error,send request,[%s][DeqWorkIndex=%d][CheckSendMsgNum=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum);
		nFunRes = -1;	 
		return nFunRes;
	}
	
	LOG3(SourceInfo, DebugUtil::DebugDebug, 
		"deq SendRequest ok! [%s][DeqWorkIndex=%d][CheckSendMsgNum=%d]", 
		m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum); 

	//print log info
	_LOGSendRequestDeq(m_pEnqReqEx);

	return nFunRes;
}


int	CSrvDeqWorker::_ServerPocesRecvReplyAndComit()
{
	int nFunRes = 0;
	PENQACK   pOneEnqReplyFrame = NULL;
	lstEnqACKConIterT iterLstFrame;
	std::string strRcvEnqReplyUUID32;  
	
	//MonitorDeqTime begin time here
	m_pMonitorDeqTime->workBegin();


	//get Reply Msg
	TA_Base_Core::ThreadGuard guard(m_lockLstTcpRecvFrames); 	

	iterLstFrame = m_LstTcpRecvFrames.begin();
	if (iterLstFrame != m_LstTcpRecvFrames.end())
	{
		pOneEnqReplyFrame = (*iterLstFrame);
		m_LstTcpRecvFrames.erase(iterLstFrame);
	}
	
	if (NULL == pOneEnqReplyFrame)
	{
		LOG2(SourceInfo, DebugUtil::DebugDebug, "err: _ServerPocesRecvReplyAndComit not get frame [%s][DeqWorkIndex=%d]", 
			m_strClientID.c_str(), m_nDeqWorkIndex);		
		//not get enq Reply yet so m_pMonitorDeqTime is not workEnd
		nFunRes = -1;
		return nFunRes;
	}
	 
	//client send ack to server, stop wait 
	m_pMonitorWaitACKTime->workEnd();
  	
	LOG3(SourceInfo, DebugUtil::DebugDebug, "Received Reply [%s][DeqWorkIndex=%d][CheckSendMsgNum=%d]", 
		m_strClientID.c_str(), m_nDeqWorkIndex, m_nCheckSendMsgNum); 

	//comit deq
	/*
	return value will be -1 or 0 or Err_DEQACkCode_NOTFIND or ER_ORACLE_AQ_DEQUEUE_TIMEOUT 
	-1: means deq have error!
	0 :deq remove data is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	Err_DEQACkCode_NOTFIND: means deq function is ok! but there deq get one msg is not in remove list
	*/
	nFunRes = _ServerProcessCommit(pOneEnqReplyFrame); 

	//check Deq Result 	
	switch (nFunRes)
	{
	case -1:
		LOG2(SourceInfo, DebugUtil::DebugError, "err: deq comit err! [%s][DeqWorkIndex=%d]",m_strClientID.c_str(), m_nDeqWorkIndex);		
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState=server_state_deq_FinishWork; 
		nFunRes = -1;
		break;
	case Err_DEQACkCode_NOTFIND:
		LOG2(SourceInfo, DebugUtil::DebugError, "err: deq comit err! [%s][DeqWorkIndex=%d]",m_strClientID.c_str(), m_nDeqWorkIndex);		
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState=server_state_deq_FinishWork; 
		nFunRes = -1;
		break;
	case 0:
		LOG2(SourceInfo, DebugUtil::DebugDebug, "deq comit ok! [%s][DeqWorkIndex=%d]", m_strClientID.c_str(), m_nDeqWorkIndex);
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState =server_state_deq_DeqData;
		nFunRes = 0;
		break;	
	default:
		LOG2(SourceInfo, DebugUtil::DebugError, "err: deq comit err! [%s][DeqWorkIndex=%d]",m_strClientID.c_str(), m_nDeqWorkIndex);		
		//MonitorDeqTime end time here
		m_pMonitorDeqTime->workEnd();
		m_nDeqWorkState=server_state_deq_FinishWork; 
		nFunRes = -1;
		break;
	}//switch
	
	//free Frame Data 
	strRcvEnqReplyUUID32 = pOneEnqReplyFrame->m_strPkgID;//->getMsgFramePUUID();
	LOG2(SourceInfo, DebugUtil::DebugDebug, "[%s][RcvEnqReplyUUID32=%s] sever delete one Frame ",m_strClientID.c_str(), strRcvEnqReplyUUID32.c_str()); 
	DEF_DELETE(pOneEnqReplyFrame);	

	
	return nFunRes;
}

 
int	 CSrvDeqWorker::getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet)
{
	int nFunRes = 0;
	
	if (NULL != m_pMonitorDeqTime)
	{
		workTimeInfoGet.m_nDeqNotWork = m_pMonitorDeqTime->getNotworkTime();
	}
	else
	{
		workTimeInfoGet.m_nDeqNotWork = 0;
	}
	
	if (NULL != m_pMonitorWaitACKTime)
	{
		workTimeInfoGet.m_nDeqACKNotGet = m_pMonitorWaitACKTime->getNotworkTime();
	}
	else
	{
		workTimeInfoGet.m_nDeqACKNotGet = 0;
	}
	
	return nFunRes;
}


int	CSrvDeqWorker::_DeqUpdateTime(IDatabase* pINDataBase,std::string& strINLocationKey, 
										int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime)

{ 
	int					nFunRes = 0;
	std::string			strOracleSQL;

	//update AQ_DEQ_TOKEN_CHECK set UPD_TIME = current_timestamp where UPPER(LOCATION)=UPPER('%s') and SRV_ID=%s and UPPER(Q_NAME) = UPPER('%s')
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_UPDATE_SET_UPDATETIME_7012, strINLocationKey.c_str(), nINSRVID, strINQueueName.c_str());

	try
	{
		pINDataBase->connect();
		pINDataBase->executeModification(strOracleSQL);
		pINDataBase->commitTransaction();
	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"error! [strOracleSQL=%s]", strOracleSQL.c_str());
	}

	return nFunRes;
}

int	CSrvDeqWorker::_DeqUpdateTimeAndSrvID(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime)

{ 
	int					nFunRes = 0;
	std::string			strOracleSQL;

	//("update AQ_DEQ_TOKEN_CHECK set UPD_TIME = current_timestamp,SRV_ID=%s where UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME) = UPPER('%s')"),
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_UPDATE_SET_TIMEANDSRVID_7013, nINSRVID, strINLocationKey.c_str(), strINQueueName.c_str());
	try
	{
		pINDataBase->connect();
		pINDataBase->executeModification(strOracleSQL);	
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"error! [strOracleSQL=%s]", strOracleSQL.c_str());
	}

	return nFunRes;
}



int	CSrvDeqWorker::_DeqUpdateSrvIDThenUpdateTime(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime)
{ 
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	int                 nOUTNotWorkSeconds = -1;
	
	//DEQ_SQL_UPDATE_SET_SRVID_7018=update AQ_DEQ_TOKEN_CHECK set SRV_ID=%s where UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME) = UPPER('%s')
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_UPDATE_SET_SRVID_7018, nINSRVID, strINLocationKey.c_str(), strINQueueName.c_str());
	try
	{
		pINDataBase->connect();
		//try to get lock AQ_Deq_token 
		//and update old SrvID to New SrvID  
		//but not commit
		pINDataBase->executeModification(strOracleSQL);	
		//select update time to check old SrvID thread is work ok or not
		//select tmp.minutes * 60 + tmp.seconds as NOTWORKTIME from 
		pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_NOTWORKTIME_7016, strINLocationKey.c_str(), strINQueueName.c_str());
		columnNames.push_back("NOTWORKTIME");
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		if (NULL != pData && pData->getNumRows() > 0)
		{	
			nOUTNotWorkSeconds = pData->getIntegerData(0, columnNames[0], -1000); 
		} 
		//if not  old SrvID thread not work time > DeqFailOver time,
		//update UPD_TIme to current_timestamp 
		//and commit
		if (nOUTNotWorkSeconds * TIME_BASE_SECOND > g_system_variables.m_nDeqFailOverTime)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 	
				"[DeqNotWorkTime >= DeqFailOverTime][%d>=%d][nGetSRVID=%d] then set UPD_TIME = current_timestamp", 
				nOUTNotWorkSeconds*TIME_BASE_SECOND, g_system_variables.m_nDeqFailOverTime, nINSRVID);	
			//update AQ_DEQ_TOKEN_CHECK set UPD_TIME = current_timestamp,SRV_ID=%s where LOCATION = '%s' and Q_NAME = '%s'
			pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_UPDATE_SET_TIMEANDSRVID_7013, nINSRVID, strINLocationKey.c_str(), strINQueueName.c_str());
			pINDataBase->executeModification(strOracleSQL);
			pINDataBase->commitTransaction();
			nFunRes = 0; 
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 	
				"[DeqNotWorkTime < DeqFailOverTime][%d<%d][nGetSRVID=%d] then rollback update AQ_DEQ_TOKEN_CHECK set new SRV_ID", 
				nOUTNotWorkSeconds*TIME_BASE_SECOND, g_system_variables.m_nDeqFailOverTime, nINSRVID);	

			pINDataBase->rollbackTransaction();
			nFunRes = -1; 
		}
	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"error! [strOracleSQL=%s]", strOracleSQL.c_str());
	}
	
	DEF_DELETE(pData); 
	return nFunRes;	 	
}


int	CSrvDeqWorker::_DeqSelectForUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName)

{ 
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;

	//select UPD_TIME from AQ_DEQ_TOKEN_CHECK where  UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME)=UPPER('%s') and SRV_ID = %s for update
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_LOCKUPDATETIME_7015, 
		strINLocationKey.c_str(), strINQueueName.c_str(), nINSRVID);


	columnNames.clear();
	columnNames.push_back("UPD_TIME");
	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		//not commit here
	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error,get token,[SQL=%s]", strOracleSQL.c_str());
	}	

	//if get no data, pData is NULL, check pData before use		
	if (NULL == pData)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error,loss token, [SQL=%s]", strOracleSQL.c_str());
		nFunRes = -1;
		return nFunRes;
	}
	else if (NULL != pData && pData->getNumRows() <= 0)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error,loss token, [SQL=%s]", strOracleSQL.c_str());
		nFunRes = -1;
	}
	
		
	DEF_DELETE(pData);

	return nFunRes;
}
int	CSrvDeqWorker::_DeqInsert(IDatabase* pINDataBase, 
									std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime)
{

	int					nFunRes = 0;
	std::string			strOracleSQL;

	//"insert into AQ_DEQ_TOKEN_CHECK (LOCATION, SRV_ID, Q_NAME, UPD_TIME) values( UPPER('%s'), %s, UPPER('%s'), current_timestamp)"
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_INSERT_ADD_UPDATETIME_7017, strINLocationKey.c_str(), nINSRVID, strINQueueName.c_str());
	try
	{
		pINDataBase->connect();
		pINDataBase->executeModification(strOracleSQL);
		pINDataBase->commitTransaction();
	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"_OracleDeqInsert() error! [strOracleSQL=%s]", strOracleSQL.c_str());
	}

	return nFunRes;
}

int	CSrvDeqWorker::_DeqTokenCheckInsert(IDatabase* pINDataBase, std::string& strINLocationKey, 
											  std::string& strINQueueName, int nINSRVIDForCheck)
{
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	struct timeb        getOUTfbtime;
	int					nGetSRVID = -1000;
	bool                bInsert = false;

	if (NULL == pINDataBase)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err! pDatabase is NULL");
		nFunRes = -1;
		return nFunRes;
	}

	/*
	select SRV_ID, UPD_TIME from AQ_DEQ_TOKEN_CHECK where  UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME)=UPPER('%s')
	*/
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_UPDATETIME_7011, 
		strINLocationKey.c_str(), strINQueueName.c_str());


	columnNames.clear();
	//SRV_ID,UPD_TIME 
	columnNames.push_back("SRV_ID");
	columnNames.push_back("UPD_TIME");
	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error! [strOracleSQL=%s]", strOracleSQL.c_str());
		DEF_DELETE(pData);
		return nFunRes;
	}

	//if select no record  insert and return nFunRes=0	
	if (NULL == pData)
	{
		bInsert = true;		
	}
	else if (NULL != pData && pData->getNumRows() <= 0)
	{
		bInsert = true;
	}
	else
	{
		bInsert = false;
	}
	
	if (bInsert)
	{
		nFunRes = _DeqInsert(pINDataBase, strINLocationKey, nINSRVIDForCheck, strINQueueName, &getOUTfbtime);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: Deq insert record to AQ_DEQ_TOKEN_CHECK");
			nFunRes = -1;
		}
		DEF_DELETE(pData);
		return nFunRes;
	}

	DEF_DELETE(pData);
	return nFunRes;	
} 


int	CSrvDeqWorker::_DeqTokenSelect(IDatabase* pINDataBase, std::string& strINLocationKey, 
										 std::string& strINQueueName, int nINSRVIDForCheck, timeb* pOUTUpdateTime)
{
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	struct timeb        getOUTfbtime;
	int					nGetSRVID = -1000;

	/*
	select SRV_ID, UPD_TIME from AQ_DEQ_TOKEN_CHECK where  UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME)=UPPER('%s')
	*/
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_UPDATETIME_7011, strINLocationKey.c_str(), strINQueueName.c_str());

	columnNames.clear();
	//SRV_ID,UPD_TIME 
	columnNames.push_back("SRV_ID");
	columnNames.push_back("UPD_TIME");
	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error! [strOracleSQL=%s]", strOracleSQL.c_str());
		DEF_DELETE(pData);
		return nFunRes;
	}


	//if select no record  return nFunRes=-1
	if (NULL == pData)
	{
		nFunRes = -1;
		return nFunRes;		
	}
	else if (NULL != pData && pData->getNumRows() <= 0)
	{
		DEF_DELETE(pData);
		nFunRes = -1;
		return nFunRes;		
	}
	else if (NULL != pData && pData->getNumRows() > 0)
	{	
		//get record data   SRV_ID	UPD_TIME	 
		//get nGetClientDBID
		nGetSRVID =   pData->getIntegerData(0, columnNames[0], -1000);			
		ftime(&getOUTfbtime);	
		getOUTfbtime = pData->getTimestampData(0, columnNames[1]);
	}
	

	if (nGetSRVID == nINSRVIDForCheck)
	{
		*pOUTUpdateTime = getOUTfbtime;
	}
	DEF_DELETE(pData);

	return nFunRes;
} 

int	CSrvDeqWorker::_DeqTokenCheckUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, 
											  std::string& strINQueueName, int nINSRVIDForCheck)
{
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	struct timeb        getOUTfbtime;
	int					nGetSRVID = -1000;
	int					nNotWorkSeconds;
	bool                bInsert = false;


	//("select SRV_ID, UPD_TIME from AQ_DEQ_TOKEN_CHECK where  UPPER(LOCATION) = UPPER('%s') and UPPER(Q_NAME)=UPPER('%s')"),
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_UPDATETIME_7011, strINLocationKey.c_str(), strINQueueName.c_str());

	columnNames.clear();
	//SRV_ID,UPD_TIME 
	columnNames.push_back("SRV_ID");
	columnNames.push_back("UPD_TIME");
	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"_OracleDeqTokenCheckUpdate() error! [strOracleSQL=%s]", strOracleSQL.c_str());
		DEF_DELETE(pData);
		return nFunRes;
	}

	//if select no record  return nFunRes=-2
	if (NULL == pData)
	{
		bInsert = true;
	}
	else if (NULL != pData && pData->getNumRows() <= 0)
	{
		bInsert = true;
	}
	else
	{
		bInsert = false;
	}
	
	if (bInsert)
	{
		nFunRes = _DeqInsert(pINDataBase, strINLocationKey, nINSRVIDForCheck, strINQueueName, &getOUTfbtime);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: Deq insert record to AQ_DEQ_TOKEN_CHECK");
			DEF_DELETE(pData);
			nFunRes = -1; 
			return nFunRes;	
		}
	}
	
	

	//get record data   SRV_ID	UPD_TIME
	if (NULL != pData && pData->getNumRows() > 0)
	{		 
		//get nGetClientDBID
		nGetSRVID =   pData->getIntegerData(0, columnNames[0], -1000);			
		ftime(&getOUTfbtime);	
		getOUTfbtime = pData->getTimestampData(0, columnNames[1]);
	}

	//SRV_ID data in table	AQ_DEQ_TOKEN_CHECK	is the same as	 nINSRVIDForCheck
	//then update the time
	if (nINSRVIDForCheck == nGetSRVID)
	{
		//log info
		if (0 == (m_nLogCount % SERVER_LIMIT_LOG_VLAUE_MOD))
		{
			m_nLogCount = 1;
			LOG2(SourceInfo, DebugUtil::DebugDebug, "[INSRVIDForCheck=%d][AQ_DEQ_TOKEN_CHECK nGetSRVID=%d]", 
				nINSRVIDForCheck, nGetSRVID);
		}
		m_nLogCount++;

		//update AQ_DEQ_TOKEN_CHECK.UPD_TIME
		nFunRes = _ProcessDeqUpdateTime();	
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: _ProcessDeqUpdateTime");
			nFunRes = -1; 
			DEF_DELETE(pData);
			return nFunRes;	
		}
		DEF_DELETE(pData);
		return nFunRes;
	}
	   
	//SRV_ID data in table	AQ_DEQ_TOKEN_CHECK	is not the same as	 nINSRVIDForCheck
	//then check the updatatime
	//get not work time Milliseconds  use nGetSRVID
	nFunRes = _DeqGetNotWorkTime(pINDataBase, strINLocationKey, nGetSRVID, strINQueueName, nNotWorkSeconds);
	if (0 != nFunRes)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: get not work time use nGetSRVID");
		nFunRes = -1; 
		DEF_DELETE(pData);
		return nFunRes;	
	}


	if (nNotWorkSeconds * TIME_BASE_SECOND >= g_system_variables.m_nDeqFailOverTime)//TIME_SECOND_FOUR   
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	
			"Err: [DeqNotWorkTime >= DeqFailOverTime][%d>=%d][nGetSRVID=%d][nINSRVIDForCheck=%d]", 
			nNotWorkSeconds*TIME_BASE_SECOND, g_system_variables.m_nDeqFailOverTime, nGetSRVID, nINSRVIDForCheck);

		nFunRes = _DeqUpdateSrvIDThenUpdateTime(pINDataBase, strINLocationKey, nINSRVIDForCheck, strINQueueName, NULL);
		if (0 != nFunRes)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: _DeqUpdateSrvIDThenUpdateTime");
			nFunRes = -1; 
			DEF_DELETE(pData);
			return nFunRes;	
		}
	}
	else
	{
		//if  DeqNotWorkTime < DeqFailOverTime not to deq data refuse this client
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "err: _OracleDeqUpdateTimeAndSrvID");
		nFunRes = -1; //return -1
	}
	   	   
	DEF_DELETE(pData); 
	return nFunRes;
}


int	CSrvDeqWorker::_OracleGetDBNowTime(IDatabase* pINDataBase, struct timeb* pOUTfbtime)
{
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	//DEQ_SQL_SELECT_GET_DBTIMENOW_7010=select current_timestamp as DB_NOW_TIME from dual
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_DBTIMENOW_7010);


	columnNames.clear();
	columnNames.push_back("DB_NOW_TIME");
	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error! [strOracleSQL=%s]", strOracleSQL.c_str());
		nFunRes = -1;
		DEF_DELETE(pData);
		return nFunRes;
	}

	if (NULL != pData && pData->getNumRows() > 0)
	{
		ftime(pOUTfbtime);	
		*pOUTfbtime = pData->getTimestampData(0, columnNames[0]);
		nFunRes = 0;
	}
	
	DEF_DELETE(pData); 
	return nFunRes;
} 


int CSrvDeqWorker::_DeqGetNotWorkTime(IDatabase* pINDataBase, 
											std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, 
											int& nOUTNotWorkSeconds)
{  	
	int					nFunRes = 0;
	std::string			strOracleSQL;
	std::vector<std::string> columnNames;
	IDbData*			pData = NULL;
	int					nGetSRVID = -1000;
	
	//select tmp.minutes * 60 + tmp.seconds as NOTWORKTIME from 
	pINDataBase->prepareSQLStatement(strOracleSQL, DEQ_SQL_SELECT_GET_NOTWORKTIME_7016, strINLocationKey.c_str(), strINQueueName.c_str());

	columnNames.clear();
	//SRV_ID,UPD_TIME 
	columnNames.push_back("NOTWORKTIME");

	try
	{
		pINDataBase->connect();
		pData = pINDataBase->executeQuery(strOracleSQL, columnNames);
		pINDataBase->commitTransaction();

	}
	catch (...)
	{
		nFunRes = -1;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 	"error! [strOracleSQL=%s]", strOracleSQL.c_str());
		DEF_DELETE(pData);
		return nFunRes;	
	}


	//if select no record  return nFunRes=-1
	if (NULL == pData)
	{
		nFunRes = -1;
		return nFunRes;		
	}
	else if (NULL != pData && pData->getNumRows() <= 0)
	{
		nFunRes = -1;
		DEF_DELETE(pData);
		return nFunRes;		
	}
	else if (NULL != pData && pData->getNumRows() > 0)
	{	
		//get record data   SRV_ID	UPD_TIME
		nOUTNotWorkSeconds = pData->getIntegerData(0, columnNames[0], -1000); 
	} 
	DEF_DELETE(pData);
	return nFunRes;

}

int CSrvDeqWorker::_ProcessDeqUpdateTime()
{
	int nFunRes = 0;
	ulong64 uiTimeDiff = 0;

	if ( m_nDeqWorkState == server_state_deq_DeqData ||		
		m_nDeqWorkState == server_state_deq_WaitForReply ) 
	{
		uiTimeDiff = m_pTimeDeqUpdate->getNotworkTime();

		//update AQ_DEQ_TOKEN_CHECK.UPD_TIME every 2 seconds
		if (uiTimeDiff > g_system_variables.m_nDeqUpdateTokenTime)   //2500 ms
		{ 		
			if (uiTimeDiff >= g_system_variables.m_nDeqFailOverTime)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "[%s][DeqWorkIndex=%d][deq token update time = %llu]", 
					m_strClientID.c_str(), m_nDeqWorkIndex, uiTimeDiff);
			}
			m_pTimeDeqUpdate->workEnd();
			m_pTimeDeqUpdate->workBegin();
			nFunRes = _DeqUpdateTime(m_pConsumerDataBase, m_strClientAgtName, m_nDeqInitOServerID, m_strQName, NULL);
			if (0 != nFunRes)
			{				 
				//m_nDeqWorkState = server_state_deq_FinishWork;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[%s][DeqWorkIndex=%d] cannot access duplicated check table", 
					m_strClientID.c_str(), m_nDeqWorkIndex);
			}
		}
	} 	

	return nFunRes;
}								


int	CSrvDeqWorker::_LOGSendRequestDeq(PENQREQEX pEnqReqEx)
{
	int	nFunRes = 0;
	lstDBMsgConIterT iterLst;
	CMessage* pMsgGet = NULL;
	ulong64 ulMsgSeqIDFirst = 0;
	ulong64 ulMsgSeqIDLast = 0;
	size_t uSentCnt = 0;

	if (NULL == pEnqReqEx || pEnqReqEx->m_lstDBMsg.empty())
	{
		nFunRes = -1;
		return nFunRes;
	}

	uSentCnt =  pEnqReqEx->m_lstDBMsg.size();
	pMsgGet = pEnqReqEx->m_lstDBMsg.front();
	ulMsgSeqIDFirst = pMsgGet->getMsgSeqID();

	pMsgGet = pEnqReqEx->m_lstDBMsg.back();
	ulMsgSeqIDLast = pMsgGet->getMsgSeqID();	

	LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		"[PkgID=%s][ulMsgSeqIDFirst=%llu][ulMsgSeqIDLast=%llu][uSentCnt=%d]", 
		pEnqReqEx->m_strPkgID.c_str(), ulMsgSeqIDFirst, ulMsgSeqIDLast, uSentCnt);


	return nFunRes;	   
}


void   CSrvDeqWorker::_updateActorDequeueStatusBad()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "set Dequeue Status Bad [m_ID=%d][%s][DeqWorkIndex=%d]", 
		m_ID, m_strClientID.c_str(), m_nDeqWorkIndex);

	CTcpConnectionPool::getInstance()->setDequeueStatusBad(m_ID);
}

NS_END(TA_DB_Sync)


























