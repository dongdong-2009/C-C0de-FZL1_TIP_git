//////////////////////////////////////////////////////////////////////
///		@file		PropagateMsg.h
///		@author		zhilin,ouyang
///		@date		2012-01-29 14:44:41
///
///		@brief	    the thread to synch data based on the consumer name. 
///                 each consumer per thread in the system.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  PROPAGATEMSG_H
#define  PROPAGATEMSG_H

#include <sys/timeb.h>

#include "CommonData.h"
#include "IPropagateHandler.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

using TA_Base_Core::ReEntrantThreadLockable;

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CListener;
class CConsumer;
class CMessage;


NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class IObserverRcvMsg;
class ITcpComType;
class CMsgItemHelper;
class CProcessRcvMsg;
class CDeqMsgJob;
class CAWorkTime;

class CPropagateMsg : public  TA_Base_Core::Thread
					, public IPropagateHandler
{
private:
	enum eumPropagateState
	{
		STATE_CODE_INIT_ENV                             = 0,
		STATE_CODE_CONNTOSRV,
		STATE_CODE_SENDINFO,
		STATE_CODE_INITDEQ,
		STATE_CODE_DEQMSG,
		STATE_CODE_SENDMSGREQ,
		STATE_CODE_WAITFORENQACK,
		STATE_CODE_WAITFORCONACK,
		STATE_CODE_RCVENQACK,
		STATE_CODE_RCVCONACK,
		STATE_CODE_FINISH_TRANSACTION
	};

public:
	CPropagateMsg();
	~CPropagateMsg(void);

public:
	virtual int   setPropagateTHD(PRO_THD* pProThd);
	virtual int   processRcvFrame(PCONACK pConAck);
	virtual int   processRcvFrame(PENQACK pEnqAck);
	virtual int   processTcpEvent( int nStateCode );

public:
	virtual void run();
	virtual void terminate();

private:
	void  _ThreadJob();

	void  _ProcessInitEnv();
	void  _ProcessCon2Srv();
	void  _ProcessSendInfo();
	void  _ProcessInitDeq();
	void  _ProcessDeqMsg();
	void  _ProcessSendMsgReq();
	void  _ProcessWaitForEnqAck();
	void  _ProcessWaitForConAck();
	void  _ProcessRcvEnqAck();
	void  _ProcessRcvConAck();
	void  _ProcessFinishTransaction();

	void  _ParseSrvAddr(const std::string& strAddr);
	std::string _GetLocHostName();

	template<class MsgConT>
	void  _FreeMsgCon(MsgConT& rMsgCon);

	void  _ClearUpEnv();
	void  _InitDeqEnv(int nStatus = Ret_Success);
	void  _GetWorkAgent();
	void  _GetSrvID();
	void  _StartComWork();
	void  _StopComWork();
	void  _PrintMsgLog(int& nPrintInterval, int nState);
private:
	bool                     m_bTerminate;                       // the thread terminate flag
	IDatabase*               m_pDeqDatabase;                     // the database connection for dequeue message
	CListener*               m_pListener;                        // the listener for monitor message from database
	CConsumer*				 m_pMsgConsumer;                     // the consumer to consume message from database
	lstDBMsgConT             m_lstSendMsg;                       // the total list of messages dequeue from database once a time (3000)
	std::string              m_strServerID;                      // the client tcp id consist of (Q_Name + Sender + IP)
	int                      m_nTcpState;                        // the tcp/ip state
	int                      m_nDBType;                          // the database type (oracle or mysql)
	ulong64                  m_ulMsgSent;                        // the total sent messages 
	IObserverRcvMsg*         m_pMsgObserver;                     // the message observer for register to message dispatcher
	ITcpComType*             m_pComWork;                         // the communication work thread for this propagate channel
	eumPropagateState        m_nPropagateFlag;                   // the state flag of the propagation
	PRO_THD*                 m_pProTHD;                          // the propagate thread info 
	CAgent                   m_Receiver;                         // the message receiver (can be more than one)
	CONACK*                  m_pConAck;                          // the struct of connection reply acknowledge
	ENQACK*                  m_pEnqAck;                          // the struct of enqueue request reply acknowledge
	ENQREQEX                 m_stEnqReq;                         // the package of enqueue request. currently 200K per batch based on the configuration
	CMsgItemHelper*          m_pMsgItemHelper;                   // the helper for message pack stream
	std::string              m_strReqPkgID;                      // the unique package UUID for enqueue request
	std::string              m_strAckPkgID;                      // the unique package UUID for reply acknowledge
	std::string              m_strSrvAddr;                       // the server address
	std::string              m_strSrvPort;                       // the server port

	CProcessRcvMsg*          m_pEnqReqProcessor;                 // the handler of download message processor
	ReEntrantThreadLockable  m_tLockAcks;                        // the thread lock for acknowledge reply (connection & enq request)
	int                      m_nLogCount;                        // the count to control the message log out
	int                      m_nDBSrvID;                         // the database server ID
	int                      m_nMsgSrvID;                        // the message server ID of the first item
	uint32                   m_nReconTimes;                      // the fail over reconnect server times
	uint32                   m_nConFailTimes;                    // the fail times during TCP failed as sequence out of order (can connected but fail immediately)
	ulong64					 m_nEachDeqWorkUse;	 	             // only for log usage
	size_t					 m_nDeqWorkIndex;                    // only for log usage

	ulong64					 m_nStepDeqDataTime;                 // only for log usage
	ulong64					 m_nStepSendDataTime;	             // only for log usage
	ulong64					 m_nStepWaitACKTime;	             // only for log usage
	ulong64					 m_nStepDeqCommitTime;	             // only for log usage
	
	CAWorkTime*				 m_pTimelogEachDeqWork;				 // the helper only for calculate time
	CAWorkTime*				 m_pTimelogDeqData;					 // the helper only for calculate time
	CAWorkTime*              m_pTimelogSendData;				 // the helper only for calculate time
	CAWorkTime*              m_pTimelogWaitACK;				     // the helper only for calculate time
	CAWorkTime*              m_pTimelogDeqCommit;				 // the helper only for calculate time

	CAWorkTime*              m_pTimeConAck;				 // shake hand ack timer monitor
	ulong64                  m_nMsgCountEachDeq;         // m_lstSendMsg.size
};

NS_END(TA_DB_Sync)

#endif  // PROPAGATEMSG_H
