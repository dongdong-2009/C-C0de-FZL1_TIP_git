#if !defined(__C_SERVER_COM_WORK_HH__)
#define __C_SERVER_COM_WORK_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>
#include "Buffer.h"
#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"
#include "core/sockets/src/TcpServerSocket.h"


NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;

class IMsgItem;
class IMsgPackage;
class CComProtocol;
class IObserverRcvMsg;
class CAWorkTime;

class CSrvComWorker : public Thread 
{
public:
	CSrvComWorker(TcpSocket* pTcpSocket);
	~CSrvComWorker();
	
public:
	virtual void	run(); 
	virtual void	terminate();
	
public:
	int serverSendMsgPkg(MSGPKG* pMsgPkg);
	void registerObserver( IObserverRcvMsg* Handler );
	void removeObserver();
	bool isTCPConnected();
	int setAgtInfo(PAQWorkInfo pDeqEnqInfo);

private:
	int	_ThreadJob();
	

private: 
	int _SocketRead();
	int _ProcessRecvBuffer();	
	int _ProcessRcvMsgPackages();
													
	int _SocketWriteMsgPackage(IMsgPackage* pSendMsgPackage);

	/*
	_SocketWriteBuffer() return value: 0 or -1
	*/
	int  _SocketWriteBuffer(CBuffer* pSendBuff);
	int  _FreeLstMsgItems(lstMsgItemsConT& lstMsgItems);
	int  _FreeLstMsgItem(lstMsgItemConT& lstMsgItem);
	void _CheckIntValue();	
	int	 _FreeLstRecvMsgPackage();

	// for heartbeat mechanism
	void _SendNotifyHeartBeat();
	void _InitHeartBeatTimer();
	void _BuildHeartPkg(MSGPKG*& pMsgPkg);

private:
	/*
	* socket read lock
	*/
	ReEntrantThreadLockable	m_SocketReadLock;
	CBuffer* m_pRecvBufferOnce;//use socket read lock
	CBuffer* m_pRecvBufferTotal;//use socket read lock
	lstMSGPKGConT m_lstRecvMsgPackage;//use socket read lock
	IObserverRcvMsg*   m_pObserverRcvMsg;//use socket read lock

	/*
	* socket info lock
	*/
	ReEntrantThreadLockable	m_SocketInfoLock;
	bool		m_isConnected;//use socket info lock

	/*
	* socket write lock
	*/
	ReEntrantThreadLockable	m_SocketWriteLock;
	
	TcpSocket* m_pSocket;//

	// the state of the heartbeat (ON/OFF) refer to HeartBeatCode in CommonDef.h
	//int        m_nHeartBeatState;

	// timer for sending heartbeat
	CAWorkTime*	    m_pHeartBeatTimer;

private:
    std::string		m_strClientID;
	
	bool			m_toTerminate;
	int             m_nLogCount;
	int				m_socketTimeout;

	CComProtocol*      m_pComProtocol;


	

};//class 


NS_END(TA_DB_Sync)




#endif//__C_SERVER_COM_WORK_HH__




















