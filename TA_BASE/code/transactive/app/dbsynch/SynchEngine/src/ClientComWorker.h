#if !defined(___C_CLIENT_COM_WORKER_HH____)
#define ___C_CLIENT_COM_WORKER_HH____

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/sockets/src/TcpSocket.h"

#include "CommonDef.h"
#include "CommonData.h"
#include "ITcpComType.h"


NS_BEGIN(TA_DB_Sync)
using namespace TA_Base_Core;

class IMsgPackage;
class CComProtocol;
class IObserverRcvMsg;
class CBuffer;
class CAWorkTime;

class CClientComWorker : public Thread, public ITcpComType
{

public:
	CClientComWorker(const std::string& strServerIP, const std::string& strServerPort, const std::string& strQName, const std::string& strServerID);
	~CClientComWorker();

public:		
	virtual void	run();	
	virtual void	terminate();
	
public:
	virtual void	managerStart() ;
	virtual void	managerTerminateAndWait();
	virtual int		registerObserver(IObserverRcvMsg* Handler);
	virtual int		removeObserver(); 
	virtual int		clientSendData(const std::string& strServerID, PMSGPKG pSendMsgPakage);
	virtual int		connectToServer(int nTimeOut);
	
private:	
	void		_ThreadJob();
	void		_CheckIntValue();
	int			_ConnectToServer(int nTimeOut);

private: 
	int			_SocketRead();
	int			_ProcessRecvBuffer();	
	int			_ProcessRcvMsgPackages();
	int			_SocketWriteMsgPackage(IMsgPackage* pSendMsgPackage);	
	int			_FreeLstMsgItems(lstMsgItemsConT& lstMsgItems);
	int			_FreeLstMsgItem(lstMsgItemConT& lstMsgItem);
	int			_FreeLstRecvMsgPackage();
	
	/*
	*  _SocketWriteBuffer return value 0 or -1
	* return value is 0  : send data ok
	* return value is -1  : send data error
	*/
	int			_SocketWriteBuffer(CBuffer* pSendBuff);	

	void        _InitHeartBeatTimer();
	void        _StartHeartBeatTimer(unsigned char ucPkgType = MSGPACKAGE_PT_REQUEST);
	void        _CheckHeartPkg();
private:
	/*
	* socket Read lock
	*/
	ReEntrantThreadLockable	    m_SocketReadLock;
	CBuffer*					m_pRecvBufferOnce;//use socket Read lock
	CBuffer*					m_pRecvBufferTotal;//use socket Read lock
	lstMSGPKGConT               m_lstRecvMsgPackage;//use socket Read lock

	
	/*
	* socket info lock
	*/
	ReEntrantThreadLockable		m_SocketInfoLock;
	bool                        m_isConnected;//use socket info lock
	IObserverRcvMsg*			m_pObserverRcvMsg;//use socket info lock

	/*
	* socket write lock
	*/
	ReEntrantThreadLockable	    m_SocketWriteLock;
	
	TcpSocket*					m_pSocket;

	// the state of the heartbeat (ON/OFF) refer to HeartBeatCode in CommonDef.h
	//int        m_nHeartBeatState;

	// timer for sending heartbeat
	CAWorkTime*                 m_pHeartBeatTimer;

private:
	CComProtocol*               m_pComProtocol;	

	int                         m_nLogCount;
	int							m_socketTimeout; 
	std::string					m_strServerIP;
	std::string					m_strServerPort;
	std::string					m_strServerID;
	bool						m_toTerminate;

};


NS_END(TA_DB_Sync)






#endif//___C_CLIENT_COM_WORKER_HH____














