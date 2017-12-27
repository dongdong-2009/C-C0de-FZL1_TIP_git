
#ifndef  __I_SERVER_TCP_COM_TYPE_HH__
#define  __I_SERVER_TCP_COM_TYPE_HH__
#include "CommonDef.h"
#include <string>
#include "IDatabase.h"
#include "SrvInitWorker.h"
#include "IPropagateHandler.h"


NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;
using namespace TA_AQ_Lib;

class IObserverRcvMsg;

class IServerTcpComType :IPropagateHandler
{

public:
	//get server enq thread, com thread, deq thread not work max time
	virtual int  getServerThreadNotWorkTime(WorkTimeInfo& workTimeInfoGet) = 0;	

	//stop  enq thread, com thread, deq thread
	virtual bool serverTerminateAndWait(int nMode)=0;	

	//new one thread for deq work		
	virtual bool serverStartDeqWork(int nConReqACk) = 0; 

	//new one thread for Enq work  
	virtual bool serverStartEnqWork(int nConReqACk) = 0;  


public:
	IServerTcpComType(TcpSocket* pNewTcpSocket);
	virtual ~IServerTcpComType(void) {};

	//implement IPropagateHandler interface
	virtual int   processTcpEvent( int nStateCode );
	virtual int   processRcvFrame(PCONREQ pConReq);

	virtual void   setID(unsigned long ID); // used to identify TcpConnectionActor, it is passed from TcpConnectionActor
	virtual std::string	getClientAgentName();
	virtual std::string	getClientID(); 


public:	
	//new one thread for Com work  
	virtual bool serverStartComWork();   

	//if tcp diconnected, return false, other return true	
	virtual bool isTcpConnected() const;


protected: 
	int _InitAgentName();
	int _InitSrvStream(TcpSocket* pSocket);
	std::string _GetClientIpByClientName(const std::string& strClientName);
	int	_GetServerAgentInfo(IDatabase *pDataBase, std::string& strServerAgtName);
	int	_GetClientAgentInfo(IDatabase *pDataBase, const std::string& strClientName, std::string& strClientAgtName);		
	int _InitClientID(CSrvInitWorker* pSrvInitWorker);
	int _CheckConReq( PCONREQ pConReq);
protected:
	std::string	m_strAgtName;
	std::string	m_strQName;
	TA_AQ_Lib::EnqueueMode	m_nEnqMode;
	std::string	m_strClientID;//value eg.: AgtName_QName_IP
	int  m_nINSRVID;//OServerID
	std::string	m_strClientAgtName;//value eg.: "tra_dbg"
	std::string	m_strServerAgtName;//value eg.: "TRASACT"
	//log print number control
	int	m_nLogCount;
	//ClientHostName
	std::string	m_strClientHostName;
	//ClientIP
	std::string	m_strClientIP;

	//com work thread		
	CSrvComWorker* m_pSrvComWorker;
	//com work thread ID
	unsigned int m_nThreadIDSrvComWorker;
	//msg   Dispatch 
	IObserverRcvMsg* m_pMsgPackageDispatch;

	//lock for Handshark msg
	TA_Base_Core::ReEntrantThreadLockable  m_dataLock;
	
	//Handshark msg class
	CSrvInitWorker*	m_pSrvInitWorker;
	//recv Handshark msg valuse vill set true
	bool m_ReceivedHandshark; 
	
	
	//this server ID
	unsigned long m_ID;
};

NS_END(TA_DB_Sync)

#endif // __I_SERVER_TCP_COM_TYPE_HH__








