/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ServerConnection.cpp $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the implementation for ServerConnection.
  */

#include "ServerConnectionManager.h"
#include "NoActiveConException.h"
#include "GENAConnectionFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "Poco/Net/HTTPResponse.h"

#include <vector>

namespace TA_Base_Bus
{
	/*
	* Constructor
	*/
	ServerConnectionManager::ServerConnectionManager(const std::vector< GenaClientSourcePtr >& sourceServers, GENAConnectionFactory* pConnectionMgr)
	{
		FUNCTION_ENTRY("Constructor");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		std::vector< GenaClientSourcePtr >::const_iterator itr = sourceServers.begin();
		for (itr; itr!=sourceServers.end(); itr++)
		{
			GenaClientSourcePtr tempConfig = (GenaClientSourcePtr)*itr;
			ServerConState objServer;
			LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating Connection for URL: %s, TimeOut: %d, MaxRetry: %d", tempConfig->serverURL.c_str(), tempConfig->timeout, tempConfig->maxRetry);
			objServer.pConnection = pConnectionMgr->createConnection(tempConfig->serverURL, tempConfig->timeout, tempConfig->maxRetry);
			objServer.state = Unknown;
		
			// add to server list
			m_servers[tempConfig->serverURL] = objServer;

			// Create assigned resolve thread
			ThreadConnResolvePtr objThdResolve(new ThreadConnResolve(this, objServer.pConnection));
			m_threadResolveList.push_back(objThdResolve);
		}

		FUNCTION_EXIT;
	}
	
	/*
	* Destructor
	*/
	ServerConnectionManager::~ServerConnectionManager(void)
	{
		FUNCTION_ENTRY("Constructor");

		FUNCTION_EXIT;
	}

	/*
	* sendRequest
	*/
	GENARequestStatus ServerConnectionManager::sendRequest(GENARequestPtr request, GenaResponse& objResponse)
	{
		FUNCTION_ENTRY("sendRequest");
		GENARequestStatus nRetVal = GENA_UNKNOWN_ERROR;
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		{	
			TA_Base_Core::ThreadGuard guard(m_resolveLock);
			if (m_activeConn.get() != 0)
			{	// Active connection is ok send it immediately
				nRetVal = m_activeConn->sendRequest(request, objResponse);
				if (nRetVal == GENA_OK || 
					nRetVal == GENA_SID_NOT_FOUND)
				{	// Sending is ok
					return nRetVal;
				}
			}

			// Reset the active connection
			m_activeConn.reset(); 
			T_ServerMap::iterator itr = m_servers.begin();
			for (itr; itr!=m_servers.end(); itr++)
			{
				itr->second.state = Unknown;
			}
		}
		
		// We need to resolve
		for (int idx=0; idx<m_threadResolveList.size(); idx++)
		{
			m_threadResolveList[idx]->startResolve(request);
		}

		int nWaitRes = m_conditionLock.timedWait(DEFAULT_RESOLVE_TIMEOUT);
		if (nWaitRes == 0)
		{ //Timeout occurred
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Timeout occurred while resolving server connections.");
			throw TA_Base_Core::NoActiveConException("Servers did not respond timeout to request.");
		}

		for (int idx=0; idx<m_threadResolveList.size(); idx++)
		{
			m_threadResolveList[idx]->terminate();
		}

		// Get the last response recorded
		objResponse = m_activeConn->getLastResponse();
		nRetVal = m_activeConn->getLastResult();
		FUNCTION_EXIT;
		return nRetVal;
	}

	/*
	* setActiveServer
	*/
	void ServerConnectionManager::setActiveServer(std::string serverURI)
	{
		FUNCTION_ENTRY("setActiveServer");
		{	 
			TA_Base_Core::ThreadGuard guard(m_resolveLock);
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set Active Server: %s", serverURI.c_str());
			m_activeConn = m_servers[serverURI].pConnection;
			m_servers[serverURI].state = Active;
			T_ServerMap::iterator iter=m_servers.begin();
			for (iter; iter!=m_servers.end(); iter++)
			{
				//Change other to Passive Mode;
				if (iter->second.pConnection->getUri()!=serverURI)
				{
					iter->second.state=Passive;
				}
			}		 
		}
		m_conditionLock.signal();
		FUNCTION_EXIT;
	}

} //TA_Base_Bus