#if !defined(__C_SERVER_DEQ_WORKER_HH__)
#define __C_SERVER_DEQ_WORKER_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>
#include "CommonDef.h"
#include "CommonData.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "IPropagateHandler.h"
#include "IDatabase.h"
#include "Consumer.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;


class IMsgItem;

class IObserverRcvMsg;
class CMsgItemHelper;
class CSrvComWorker;
class CAWorkTime;


class CSrvDeqWorker : public Thread,
					  public IPropagateHandler
{
private:	
	enum ServerDeqWorkState
	{
		server_state_deq_DeqTokenCheck, 
		server_state_deq_DeqInit,
		server_state_deq_DeqData,
		server_state_deq_WaitForReply,
		server_state_deq_FinishWork,
	};

public:
	CSrvDeqWorker(CSrvComWorker* pComWorker,PAQWorkInfo pDeqEnqInfo, unsigned long ID);
	~CSrvDeqWorker();

public:
	virtual void									run(); 
	virtual void									terminate();


public:
	virtual int   processTcpEvent( int nStateCode );
	virtual int   processRcvFrame(PENQACK pEnqAck);

public:
	bool serverTerminateAndWait(int nMode);
	bool serverStart();
	int	 getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet);


private:
	void _FreeMsgVec(lstDBMsgConT& rVecMsg);
	int	_CheckEnvValue();
	int	_setAgtInfo(PAQWorkInfo pDeqEnqInfo);
    void _updateActorDequeueStatusBad();
private:
	int	_InitDeqWork();
	int	_UnInitDeqWork();

private:
	/*	
	return value will be -1 or 0 or ER_ORACLE_AQ_DEQUEUE_TIMEOUT or ER_ORACLE_DequeueFailed 
	-1: means deq have error!
	0 :means deq function is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	ER_ORACLE_DequeueFailed; oracle deq closed
	*/
	int	_DeqDataFormDB(lstDBMsgConT& rVecMsg);
	
	/*
	return value will be -1 or 0 or Err_DEQACkCode_NOTFIND or ER_ORACLE_AQ_DEQUEUE_TIMEOUT 
	-1: means deq have error!
	0 :deq remove data is ok!
	ER_ORACLE_AQ_DEQUEUE_TIMEOUT: means deq function is ok! but there have no data for deq
	Err_DEQACkCode_NOTFIND: means deq function is ok! but there deq get one msg is not in remove list
	*/
	int	_ServerProcessCommit(PENQACK pReplyFrame);
private:
	int	_ServerPocessDeqTokenCheck(); 
	int	_ServerPocessDeqInit();
	int	_ServerPocessDeqData();
	int	_ServerPocessSendRequest(); 
	int	_ServerPocesRecvReplyAndComit();

private:	
	int _DeqInsert(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime);
	int _DeqUpdateTime(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime);
	int _OracleGetDBNowTime(IDatabase* pINDataBase, struct timeb* pOUTfbtime);
	int _DeqGetNotWorkTime(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, int& nOUTNotWorkSeconds);
	int _ProcessDeqUpdateTime();
	/*
	* use select * XX check
	* if no record in table AQ_DEQ_TOKEN_CHECK then insert one
	*/
	int _DeqTokenCheckInsert(IDatabase* pINDataBase, std::string& strINLocationKey, std::string& strINQueueName, int nINSRVIDForCheck);
	int _DeqUpdateTimeAndSrvID(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime);
	int	_DeqUpdateSrvIDThenUpdateTime(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName, struct timeb* pOUTfbtime);
	int _DeqTokenSelect(IDatabase* pINDataBase, std::string& strINLocationKey, std::string& strINQueueName, int nINSRVIDForCheck, timeb* pOUTUpdateTime);
	
	/*
	* GetToken and 	update AQ_DEQ_TOKEN_CHECK.UPD_TIME
	*/
	int _DeqTokenCheckUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, std::string& strINQueueName, int nINSRVIDForCheck);
	int _DeqSelectForUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINSRVID, std::string& strINQueueName);
	
private:	
	int	_ProceesCountTypeMsg(PENQACK pEnqAck);
	int _LOGSendRequestDeq(PENQREQEX pEnqReqEx);
	int _FreeRecvPackage();

private:
	bool m_toTerminate;
	PENQREQEX m_pEnqReqEx;
	int	m_nCheckSendMsgNum;
	std::string	m_strCheckSendMsgUUID32;
																					   
private:																				
	ServerDeqWorkState	m_nDeqWorkState;
	CSrvComWorker* m_pComWorkerCopy;
	TA_AQ_Lib::IDatabase* m_pConsumerDataBase;
	TA_AQ_Lib::CConsumer* m_pCServerConsumer;
	CMsgItemHelper* m_pMsgItemHelper;

	size_t m_nDeqWorkIndex;
	size_t m_nDeqTotal;	
 	size_t m_nLogCount;
 	
 	/*
 	* CAWorkTime used to update AQ_DEQ_TOKEN_CHECK.UPD_TIME every 30 seconds  and print log info, 
	* print log info: Deq Date use time, Deq Commit use time;
	*/
	CAWorkTime*   m_pTimelogDeqData;
	CAWorkTime*   m_pTimeDeqUpdate;
	CAWorkTime*   m_pTimelogDeqCommit;


	/*
	* m_pMonitorDeqTime use to Monitor, Deq TokenCheck time, Deq data time and Deq Commit time, (the timeout value is TIME_SECOND_FIVE)  
	*/
	CAWorkTime*   m_pMonitorDeqTime;	//MonitorDeqTime

	/*
	* m_pMonitorWaitACKTime use to Monitor, Client Enq data and send ack time, (the timeout value is TIME_MIN_FIFTEEN)
	*/
	CAWorkTime*   m_pMonitorWaitACKTime;	//MonitorDeqTime

private:	
	std::string	m_strClientAgtName;//"tra_dbg"
	std::string	m_strServerAgtName;//"TRASACT"
	std::string	m_strQName;
	std::string m_strClientID;
	int m_nDeqInitOServerID;
	
	
private:
	TA_Base_Core::ReEntrantThreadLockable	m_lockLstTcpRecvFrames; 
	lstEnqACKConT	m_LstTcpRecvFrames;
	TA_Base_Core::Semaphore m_actionSemaphore;      //used to replace sleep
	unsigned long m_ID;
	
};//class CSrvDeqWorker


NS_END(TA_DB_Sync)

#endif//__C_SERVER_DEQ_WORKER_HH__





















