#if !defined(__C_TCP_CONNECTION_LISTEN_HH___)
#define __C_TCP_CONNECTION_LISTEN_HH___
	

#include <string>
#include <set>

#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"
#include "core/sockets/src/TcpServerSocket.h"



					 

NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;


class CTcpConnectionPool;

class CTcpConnectionListen : public Thread
{
public:

	/*
	* 
	* @connectionObserver: observer to pass connected clients on to
	* @port: the port name/number to listen on
	*/
	CTcpConnectionListen(std::string& port );

	/*
	* ensure this thread is stopped
	*/
	~CTcpConnectionListen();

	virtual void	run(); 
	virtual void	terminate();

private:
	bool _ServerListenSocketAccept(TcpServerSocket<TcpSocket>* serverSocket);
	void _ThreadJob();
	void _StopThreadPool();
	void _StartThreadPool();
private:
	TcpServerSocket<TcpSocket>* m_serverListenSocket ;//server listen socket
	std::string	m_strServerListenName; 
	std::string	m_strServerPort ;
	bool m_toTerminate ;
	SocketSet< TcpServerSocket< TcpSocket > > m_SetTcpListenSocket;
	bool m_bInit;
};

NS_END(TA_DB_Sync)

#endif	//__C_TCP_CONNECTION_LISTEN_HH___














