#if !defined(__C_MYSQL_SERVER_ENQ_WORK_HH__)
#define __C_MYSQL_SERVER_ENQ_WORK_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>

#include "AQCommonDef.h"
#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

#include "IServerTcpComType.h"
#include "IPropagateHandler.h"
#include "IDatabase.h"
#include "Producer.h"

NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;


class CSrvComWorker;
class CMsgItemHelper;
class CAWorkTime;

class CMysqlSrvEnqWorker : public Thread, public IPropagateHandler
{
private:
	typedef std::map<int, ulong64>				mapSIDMIDConT;   
	typedef std::map<int, ulong64>::iterator	mapSIDMIDConIterT;
	typedef std::map<int, ulong64>::value_type	mapSIDMIDValueT;  

private:
	enum ServerEnqWorkState
	{
		server_state_enq_EnqTokenCheck,
		server_state_enq_InitEnq,
		server_state_enq_WaitForRequest,
		server_state_enq_RecvRequestEnq,
		server_state_enq_FinishWork,
	}; 	
public:
	CMysqlSrvEnqWorker(CSrvComWorker* pComWorker,PAQWorkInfo pDeqEnqInfo, unsigned long ID);
	~CMysqlSrvEnqWorker();

public:
	virtual void run(); 
	virtual void terminate(); 
public:
	virtual int	processTcpEvent( int nStateCode );
	virtual int	processRcvFrame(ENQREQEX* pEnqReq);

public:
	bool serverTerminateAndWait(int nMode);
	bool serverStart();	
	int	 getWorkTimeInfo(WorkTimeInfo& workTimeInfoGet);
                                             														
private:
	int	_SetAgtInfo(PAQWorkInfo pDeqEnqInfo);
	void _FreeMsgVec(lstDBMsgConT& rVecMsg);
	void _CheckIntValue();

private:
	int	_InitEnqWork();	  
	int	_UnInitEnqWork();
	int	_ProceesCtoSEnqReq(ENQREQEX* pEnqReq);
	void _updateActorEnqueueStatusBad();

private:
	int	_ServerPocessTokenCheck();
	int	_ServerPocessEnqInitEnq();
	int _ServerHandleEnqMessage();
	int	_ServerPocessCommit();
	int	_ServerPocessSendReply();
	int	_ServerAffairsSendReply(PENQACK pEnqACK); 
	int	_ServerPocessEnqData(ENQREQEX* pRcvFrame);

private:	
	int	_CheckEnqRequest(ENQREQEX* pRcvFrame);
	int	_EnqToDB(ENQREQEX* pRcvFrame, ulong64 uLLastMsgSID);
	int _EnqSelectForUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINServerID, std::string& strINQueueName, mapSIDMIDConT& mapOUTSIDMID);
	int _EnqInsert(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINServerID, std::string& strINQueueName, ulong64 nINMsgSID);
	int _EnqUpdate(IDatabase* pINDataBase, std::string& strINLocationKey, int& nINServerID, std::string& strINQueueName, mapSIDMIDConT& mapINSIDMID);
	void _Write2ExceptionQ(CMessage* pMsg);
	int _FreeEnqReqEx(ENQREQEX** ppEnqReqEx);
	int _FreeLstRecvMsgPackage();
	
	
private: 	 
	bool m_toTerminate;
	ServerEnqWorkState m_nEnqWorkState;

	//Enq thread each step work time
	CAWorkTime*   m_pMonitorEnqTime;	//MonitorEnqTime
	CAWorkTime*   m_pTimelogEnqData;	//for log time
	CAWorkTime*   m_pTimelogEnqCommit;	//for log time
	
private:
	TA_Base_Core::Semaphore	m_TcpRecvFramesSemaphore;
	ReEntrantThreadLockable	m_lockLstTcpRecvFrames; 
	lstEnqReqExConT	m_LstTcpRecvFrames;
	ENQREQEX* m_pCurrentRequestFrame;
	ulong64 m_ulMsgSeqIDFirst;
	ulong64	m_ulMsgSeqIDLast;
	int m_nMsgOServerID;
	std::string m_strRcvEnqRequestUUID32;  //the package id that is being handled.
	int	m_nRcvEnqRequestCount;

private: 
	CSrvComWorker* m_pComWorkerCopy; 
	TA_AQ_Lib::IDatabase* m_pProducerDataBase;
	TA_AQ_Lib::CProducer* m_pCServerProducer;
	TA_AQ_Lib::IDatabase* m_pDBForEnqCheck;
	CMsgItemHelper*	m_pMsgItemHelper;
	size_t	m_nEnqWorkIndex;
	size_t	m_nEnqTotal; 
private: 
	std::string	m_strClientAgtName;//"tra_dbg"
	std::string	m_strServerAgtName;//"TRASACT"
	std::string m_strQName;
	//ClientID
	std::string m_strClientID;
	int         m_nEnqInitOServerID;
	
	
private:
	mapSIDMIDConT  m_mapSIDMIDForEnq;
	//server id
	unsigned long   m_ID;

};//class CMysqlSrvEnqWorker


NS_END(TA_DB_Sync) 

#endif//__C_MYSQL_SERVER_ENQ_WORK_HH__




















