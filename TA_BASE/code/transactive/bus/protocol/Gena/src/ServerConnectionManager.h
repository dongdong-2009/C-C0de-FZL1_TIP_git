/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ServerConnectionManager.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for ServerConnection.
  */

#ifndef __SERVER_CONNECTION_H_INCLUDED__
#define __SERVER_CONNECTION_H_INCLUDED__
#pragma once

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "boost/shared_ptr.hpp"
#include "CommonDef.h"
#include "GENAHttpClient.h"
#include "ThreadConnResolve.h"

#include <map>
#include <vector>
#include <string>

namespace TA_Base_Bus
{
	struct ServerConState 
	{
		ServerState state;
		ConnectionPtr pConnection;
	};

	//forward declaration
	class GENAConnectionFactory;

	class ServerConnectionManager
	{
	public:
		/*
		* Constructor
		*/
		ServerConnectionManager(const std::vector< GenaClientSourcePtr >& sourceServers, GENAConnectionFactory* pConnectionMgr);
		
		/*
		* Destructor
		*/
		~ServerConnectionManager(void);

		/*
		* sendRequest
		*/
		GENARequestStatus sendRequest(GENARequestPtr request, GenaResponse& objResponse);

		/*
		* setActiveServer
		*/
		void setActiveServer(std::string serverURI);
	private: // Methods

	private:
		typedef std::map<std::string, ServerConState> T_ServerMap;
		
		// Container for the server connections
		T_ServerMap m_servers;

		// Container for the resolve threads
		std::vector<ThreadConnResolvePtr> m_threadResolveList; 

		// Thread lock for access into class
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		TA_Base_Core::ReEntrantThreadLockable m_resolveLock;
		
		// lock for resolving active server
		TA_Base_Core::Condition m_conditionLock;

		// Active server connection
		ConnectionPtr m_activeConn;
		
	}; //Connection

	typedef boost::shared_ptr<ServerConnectionManager> ServerConnectionPtr;

} //TA_Base_Bus


#endif //__SERVER_CONNECTION_H_INCLUDED__
