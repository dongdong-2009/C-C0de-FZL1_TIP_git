
#include "GENAConnectionFactory.h"
#include "NoActiveConException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Bus
{
	GENAConnectionFactory::GENAConnectionFactory(void)
		: m_bTerminate(false)
		, m_semaphore(0)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	GENAConnectionFactory::~GENAConnectionFactory(void)
	{
		FUNCTION_ENTRY("Destructor");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		// Complete
		m_connections.clear();
		FUNCTION_EXIT;
	}

// 	ConnectionManager& ConnectionManager::getInstance()
// 	{
// 		FUNCTION_ENTRY("getInstance");
// 		TA_Base_Core::ThreadGuard guard(m_singletonLock);
// 		if (m_pInstance == 0)
// 		{
// 			m_pInstance = new ConnectionManager();
// 		}
// 		FUNCTION_EXIT;
// 		return *m_pInstance;
// 	}
// 
// 	void ConnectionManager::freeInstance()
// 	{
// 		FUNCTION_ENTRY("freeInstance");
// 		TA_Base_Core::ThreadGuard guard(m_singletonLock);
// 		if (m_pInstance != 0)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = 0;
// 		}
// 		FUNCTION_EXIT;
// 	}

	ConnectionPtr GENAConnectionFactory::createConnection(const std::string& connectionURL, unsigned long timeOut, int nMaxRetry)
	{
		FUNCTION_ENTRY("createConnection");
		ConnectionPtr pRetVal;
		ConnectionMap_T::iterator itFind = m_connections.find(connectionURL);
		if (itFind != m_connections.end())
		{
			pRetVal = itFind->second;
		}
		else
		{
			GENAHttpClient* genaClient= new GENAHttpClient(connectionURL, timeOut, nMaxRetry, this);
			pRetVal.reset(genaClient);
			m_connections[connectionURL] = pRetVal;
		}

		FUNCTION_EXIT;
		return pRetVal;
	}

	void GENAConnectionFactory::setServerConnections(const std::vector< GenaClientSourcePtr >& sourceServers)
	{
		FUNCTION_ENTRY("setServerConnections");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		m_pActiveServer.reset(new ServerConnectionManager(sourceServers, this));

		FUNCTION_EXIT;
	}

	ServerConnectionPtr GENAConnectionFactory::getActiveServer()
	{
		FUNCTION_ENTRY("getActiveServer");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
// 		Connection* pRetVal = 0;
// 		ConnectionList_T::iterator itr = m_serverCons.begin();
// 		for (itr; itr!=m_serverCons.end(); ++itr)
// 		{
// 			Connection* pCon = *itr;
// 			if (0 != pCon && pCon->getStatus() == StatusGood)
// 			{
// 				pRetVal = pCon;
// 				break;
// 			}
// 		}
// 
// 		if (0 == pRetVal)
// 		{
// 			throw TA_Base_Core::NoActiveConException("No active server connection has been found!");
// 		}

		FUNCTION_EXIT;
		return m_pActiveServer;
	}

	void GENAConnectionFactory::registerStateObserver(IConnStateListener* pConnStateListener)
	{
		FUNCTION_ENTRY("registerStateObserver");
		TA_ASSERT(NULL != pConnStateListener, "conection listener should not be null");

		bool bFound = false;
		T_ConnStateObservers::iterator itr = m_conStateObservers.begin();
		for (itr; itr!=m_conStateObservers.end(); itr++)
		{	// find duplicate first
			if ((*itr) == pConnStateListener) // Compare pointer
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "listener already registered.");
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			m_conStateObservers.push_back(pConnStateListener);
		}
		FUNCTION_EXIT;
	}

	void GENAConnectionFactory::unregisterStateObserver(IConnStateListener* pConnStateListener)
	{
		FUNCTION_ENTRY("registerStateObserver");
		TA_ASSERT(NULL != pConnStateListener, "conection listener should not be null");

		T_ConnStateObservers::iterator itr = m_conStateObservers.begin();
		for (itr; itr!=m_conStateObservers.end(); itr++)
		{	// find duplicate first
			if ((*itr) == pConnStateListener) // Compare pointer
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "cconnection found already registered.");
				m_conStateObservers.erase(itr);
				break;
			}
		}
		
		FUNCTION_EXIT;
	}

	void GENAConnectionFactory::notifyStateChange(const std::string& strServer, const ConnectionStatus& status)
	{
		FUNCTION_ENTRY("registerStateObserver");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		ConnectionMap_T::iterator itFind = m_connections.find(strServer);
		if (itFind != m_connections.end())
		{
			conQueuItem objItem;
			objItem.conString = strServer;
			objItem.state = status;
			{
				TA_Base_Core::ThreadGuard guard(m_queueLock);
				m_conStatusQueue.push(objItem);
				m_semaphore.post();
			}
		}

		FUNCTION_EXIT;
	}

	void GENAConnectionFactory::run()
	{
		FUNCTION_ENTRY("run");
		
		while(!m_bTerminate)
		{
			m_semaphore.wait();
			if (m_bTerminate)
				break;
			conQueuItem objItem;
			{
				TA_Base_Core::ThreadGuard guard (m_queueLock);
				objItem = this->m_conStatusQueue.front();
				m_conStatusQueue.pop();
			}

			if (objItem.conString != "" && !m_bTerminate)
			{	
				TA_Base_Core::ThreadGuard guard(m_threadLock);
				T_ConnStateObservers::iterator itr = m_conStateObservers.begin();
				for (itr; itr!=m_conStateObservers.end()  && !m_bTerminate; itr++)
				{	// Notify observers of state change
					IConnStateListener* pTempConState = *itr;
					pTempConState->onStateChanged(objItem.conString, objItem.state);
				}
			}
		}

		FUNCTION_EXIT;
	}

	void GENAConnectionFactory::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;
		m_semaphore.post();
		FUNCTION_EXIT;
	}


} //TA_Base_Bus
