/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENAConnectionFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for connection management.
  */

#ifndef __CONNECTION_MANAGER_H_INCLUDED__
#define __CONNECTION_MANAGER_H_INCLUDED__

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "IConnStateListener.h"
#include "GENAHttpClient.h"
#include "ServerConnectionManager.h"

#include <string>
#include <map>
#include <queue>

#pragma once

namespace TA_Base_Bus
{
	struct conQueuItem
	{
		std::string conString;
		ConnectionStatus state;
	};

	class GENAConnectionFactory
		: public TA_Base_Core::Thread
	{
	public:
		GENAConnectionFactory(void);
		
		~GENAConnectionFactory(void);

		ConnectionPtr createConnection(const std::string& connectionURL, unsigned long timeOut, int nMaxRetry);
		
		ServerConnectionPtr getActiveServer();

		void setServerConnections(const std::vector< GenaClientSourcePtr >& sourceServers);

		void registerStateObserver(IConnStateListener* pConnStateListener);

		void unregisterStateObserver(IConnStateListener* pConnStateListener);

		void run();

		void terminate();

		void notifyStateChange(const std::string& strServer, const ConnectionStatus& status);

	private:

		/* 
		* copy constructor disabled
		*/ 
		GENAConnectionFactory( const GENAConnectionFactory& rhs);
		GENAConnectionFactory& operator = ( const GENAConnectionFactory& rhs);

	private:
		typedef std::map<std::string, ConnectionPtr> ConnectionMap_T;
		typedef std::vector<ConnectionPtr > ConnectionList_T;
		typedef std::vector<IConnStateListener* > T_ConnStateObservers;
		typedef std::queue<conQueuItem > T_ConStatusQue;

		// Thread lock for this class
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		TA_Base_Core::NonReEntrantThreadLockable m_queueLock;
		
		// Container for all connections
		ConnectionMap_T m_connections;

		// Server Connection Instance
		ServerConnectionPtr m_pActiveServer;

		// Container for state update 
		T_ConStatusQue m_conStatusQueue; 
		// Container for all connection state listeners
		T_ConnStateObservers m_conStateObservers;
		// Terminate flag for thread exit
		bool m_bTerminate;
		// Thread lock signal for state processing
		TA_Base_Core::Semaphore m_semaphore;
	};

	typedef boost::shared_ptr<GENAConnectionFactory> ConnectionFactoryPtr;
}


#endif //__CONNECTION_MANAGER_H_INCLUDED__