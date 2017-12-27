//////////////////////////////////////////////////////////////////////
///		@file		ProcessRcvMsg.h
///		@author		zhilin,ouyang
///		@date		2012-02-29 14:44:41
///
///		@brief	    the thread to process the received messages from  
///                 remote database/location.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  PROCESSRCVMSG_H
#define  PROCESSRCVMSG_H

#include <sys/timeb.h>
#include "CommonDef.h"
#include "IPropagateHandler.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include <list>
#include <map>
#include <string>

NS_BEGIN(TA_AQ_Lib)

class IDatabase;
class CProducer;

NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class CMsgItemHelper;
class ITcpComType;
class CAWorkTime;

using TA_Base_Core::ReEntrantThreadLockable;

class CProcessRcvMsg : public TA_Base_Core::Thread
					 , public IPropagateHandler
{
private:
	enum eumRcvMsgProcessState
	{
		STATE_CODE_INITIAL_ENV                       = 0,
		STATE_CODE_INITIAL_ENQ,
		STATE_CODE_WAITFOR_ENQREQ,
		STATE_CODE_RCV_MSG,
		STATE_CODE_GETSID_FROMDB,
		STATE_CODE_ENQ_MSG,
		STATE_CODE_UPDSID_2DB,
		STATE_CODE_SEND_ACK,
		STATE_CODE_FINISH_TRANSACTION
	};

private:
	typedef  std::list<ENQREQEX*>					lstEnqReqT;
	typedef  std::list<ENQREQEX*>::iterator			lstEnqReqIterT;
	typedef  std::map<int, ulong64>					mapDupChkSIDT;
	typedef  std::map<int, ulong64>::iterator		mapDupChkSIDIterT;
	typedef  std::map<int, ulong64>::value_type		mapDupChkSIDValueT;

public:
	CProcessRcvMsg();
	~CProcessRcvMsg(void);

	virtual int   processTcpEvent( int nStateCode );
	virtual int   processRcvFrame(ENQREQEX* pEnqReq);

public:
	virtual void run();
	virtual void terminate();

public:
	void  setServerID(const std::string& strServerID);
	void  setComWorkHander(ITcpComType* pComWork);
	void  setRcvPara(const std::string& strQName, const std::string& strRcver);

private:
	void   _ThreadJob();
	void   _ProcessInitEnv();
	void   _ProcessInitEnq();
	void   _ProcessWaitForEnqReq();
	void   _ProcessRcvMsg();
	void   _ProcessGetSIDFromDB();
	void   _ProcessEnqMsg();
	void   _ProcessUpdSID2DB();
	void   _ProcessSendAck();
	void   _ProcessFinishTransaction();

private:

	void   _FreeMsgLst(lstEnqReqT& rLstMsg);
	void   _FreeMsgItem(ENQREQEX*& pEnqReq);
	void   _PrintLog(int& nPrintInterval, int nState);
	void   _InitEnqEnv();
	void   _InitDupChkEnv();	
	void   _Write2ExceptionQ(CMessage* pMsg);

	int    _InitSID2DB();	
	int    _GetCurSIDFromDB();
	int    _UpdCurSID2DB();
	int    _MsgDupChk(int nSrvID, ulong64 ulSID, mapDupChkSIDIterT& rIter);
private:
	bool                     m_bTerminate;
	int                      m_nTcpState;
	std::string              m_strServerID;	
	std::string              m_strQName;
	std::string              m_strRcvName;
	IDatabase*               m_pEnqDatabase;
	IDatabase*               m_pDupChkDb;
	CProducer*				 m_pMsgProducer;	
	uint32                   m_nMsgRcvCnt;
	uint32                   m_nEnqMsgCnt;
	ulong64                  m_ulMsgRcvCntAll;
	ulong64                  m_ulTotalEnqMsgCnt;
	lstEnqReqT               m_lstEnqReq;
	eumRcvMsgProcessState    m_nProcessState;
	CMsgItemHelper*          m_pMsgItemHelper;
	ENQACK*                  m_pEnqAck;
	ENQREQEX*                m_pCurItem;
	mapDupChkSIDT            m_mapDupChkSID;
	ITcpComType*             m_pComWorkHandler;

	ReEntrantThreadLockable  m_threadLock;
	TA_Base_Core::Semaphore m_queueSemaphore;

private:
	size_t                   m_nEnqWorkIndex;

	ulong64					 m_nEachEnqWorkUse;	 	
	ulong64					 m_nStepEnqDataTime;
	ulong64					 m_nStepEnqCommitTime;	 	
	ulong64					 m_nStepSendACkTime;	
	//Enq thread each step work time
	CAWorkTime*				 m_pTimelogEachEnqWork;	//for log time
	CAWorkTime*				 m_pTimelogEnqData;	//for log time
	CAWorkTime*				 m_pTimelogSendACK;	//for log time
	CAWorkTime*              m_pTimelogGetSID;  //for log time
	CAWorkTime*              m_pTimelogUpdSID;  //for log time

};


NS_END(TA_DB_Sync)
#endif /// PROCESSRCVMSG_H
