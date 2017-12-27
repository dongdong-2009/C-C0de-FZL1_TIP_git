#include "TcpConnectionListen.h"


#include "core/utilities/src/DebugUtil.h"
#include "TcpConnectionPool.h"

	 

NS_BEGIN(TA_DB_Sync)

using TA_Base_Core::DebugUtil;

CTcpConnectionListen::CTcpConnectionListen( std::string& port )
{
	m_serverListenSocket = NULL;
	m_strServerPort = port;
	m_toTerminate = false;
	m_bInit = false; 
}


CTcpConnectionListen::~CTcpConnectionListen()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"CTcpConnectionListen::~CTcpConnectionListen()");
}
	 
	 
void CTcpConnectionListen::run()
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionListen  begin run()");
  
	_StartThreadPool();
	while( !m_toTerminate ) 
	{
		_ThreadJob();
		
		//select function have timeout
		//TA_Base_Core::Thread::sleep(SERVER_LIMIT_THREAD_SLEEP);  
	}

	_StopThreadPool();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "CTcpConnectionListen  end run()");

}

void CTcpConnectionListen::_StartThreadPool()
{
	if (m_bInit)
	{
		return;
	}

	CTcpConnectionPool::getInstance()->start();

	try
	{
		if (NULL == m_serverListenSocket)
		{
			m_serverListenSocket = new  TcpServerSocket<TcpSocket>(BIND_TO_ALL_INTERFACES, m_strServerPort);	
		}
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "start to listen");	

		m_strServerListenName = m_serverListenSocket->getServiceName();
		m_serverListenSocket->bind();
		m_serverListenSocket->listen();

		m_SetTcpListenSocket.addSocket(m_serverListenSocket);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[listen successfully, port=%s , tcp server m_strServerListenName = %s]",  m_strServerPort.c_str(), m_strServerListenName.c_str());
		m_bInit = true;	
	}
	catch ( TA_Base_Core::TcpSocketException& e )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"create server listen port =[%s] socket error[%s] must Exit", m_strServerPort.c_str(), e.what());
		m_SetTcpListenSocket.removeSocket(m_serverListenSocket);
		DEF_DELETE(m_serverListenSocket);

		sleep(TIME_SECOND_TWO);
		exit(Ret_Success);
		m_bInit = false;		
	}
	
		 
}

void CTcpConnectionListen::_StopThreadPool()
{
	
	if (NULL != m_serverListenSocket)
	{
		m_SetTcpListenSocket.removeSocket(m_serverListenSocket);
		DEF_DELETE(m_serverListenSocket);		
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[begin m_pTcpConnectionPool->terminateAndWait()]");	

	CTcpConnectionPool::getInstance()->terminateAndWait();	
	CTcpConnectionPool::removeInstance();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[end m_pTcpConnectionPool->terminateAndWait()]");			

	m_bInit = false;
}

void CTcpConnectionListen::_ThreadJob()
{
	bool 	bRead = true;
	bool	bWrite = true;
	int		nSelectRes = 0;
	TcpServerSocket<TcpSocket>* pSocketGet = NULL;
	unsigned int timeMillis = SERVER_LIMIT_TCP_LISTEN_TIMEOUT;

	if (NULL != m_serverListenSocket)
	{ 
		bRead = true;
		bWrite = true;
		nSelectRes = -1;
		nSelectRes = m_SetTcpListenSocket.waitForIO( bRead, bWrite, timeMillis );//put into io

		bRead = false;
		bWrite = false;
		pSocketGet = NULL;
		pSocketGet = m_SetTcpListenSocket.getSocket( 0, bRead, bWrite );
		
		if ((bRead || bWrite)&& NULL != pSocketGet)
		{
			_ServerListenSocketAccept(pSocketGet);
		}
	}//if 
}


bool CTcpConnectionListen::_ServerListenSocketAccept(TcpServerSocket<TcpSocket>* serverSocket)
{
 	bool bRes = true;
	std::string clientName;
	int nNewSocketId = 0;
	TcpSocket*	clientSocket = NULL;
	
	if (NULL == serverSocket)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"NULL == serverSocket");
		bRes = false;	
		return bRes;
	}

	try
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive new connection, server=[%s]", m_strServerListenName.c_str());

		clientSocket = serverSocket->accept(SERVER_NON_BLOCKING_ACCEPT, false); //SERVER_BLOCKING_ACCEPT
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "accept successfully");
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "server=[%s] accept error!", m_strServerListenName.c_str());
		bRes = false;	
		return bRes;
	}

	if (NULL == clientSocket)
	{
		bRes = false;
		return bRes;
	}				

	try
	{
		clientName = clientSocket->getClientName();		
		nNewSocketId = clientSocket->getSocketId();
		 
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "server=[%s] get information error!", m_strServerListenName.c_str());
		bRes = false;
		return bRes;
	}


	LOG3(SourceInfo, DebugUtil::DebugInfo, "[server=%s][try to Accept this new client name=%s][nNewSocketId=%d]", 
			m_strServerListenName.c_str(), clientName.c_str(), nNewSocketId);

	bRes = false;
	
	bRes = CTcpConnectionPool::getInstance()->poolAcceptOneNewClient(clientSocket);

				   
	clientSocket = NULL;
	return bRes;
}


void CTcpConnectionListen::terminate()
{
	m_toTerminate = true;	

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"CTcpConnectionListen::terminate()");
}

   
NS_END(TA_DB_Sync)










