/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ThreadConnResolve.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for ThreadConnResolve.
  */

#include "ThreadConnResolve.h"
#include "ServerConnectionManager.h"
#include "Poco/Net/HTTPResponse.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Bus
{
	/*
	* Constructor
	*/
	ThreadConnResolve::ThreadConnResolve(ServerConnectionManager* pServerCon, 
		ConnectionPtr pConnection)
		: m_bIsTerminate(false)
		, m_pServerCon(pServerCon)
		, m_pConnection(pConnection)
		, m_lastRetVal(GENA_NETWORK_FAILURE)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	
	/*
	* Destructor
	*/
	ThreadConnResolve::~ThreadConnResolve(void)
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		
		FUNCTION_EXIT;
	}

	/*
	* run
	*/
	void ThreadConnResolve::run(void)
	{
		FUNCTION_ENTRY("ThreadConnResolve::run");
		while(!m_bIsTerminate)
		{
			try
			{	// Lock the request data
				TA_Base_Core::ThreadGuard guard(m_threadLock);
				GenaResponse objRes;
				m_lastRetVal = m_pConnection->sendRequest(m_objReq, objRes);
				if (m_lastRetVal == GENA_OK ||
					m_lastRetVal == GENA_SID_NOT_FOUND)
				{	// This is the active server
					m_bIsTerminate=true;
					m_pServerCon->setActiveServer(m_pConnection->getUri());					
					break;
				}
				m_bIsTerminate=true;
				//m_condition.timedWait(2000); // Sleep 2 sec if failed before trying again
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to send request.");				
			}		
			m_bIsTerminate=true;
		}

		FUNCTION_EXIT;
	}

	/*
	* terminate
	*/
	void ThreadConnResolve::terminate(void)
	{
		FUNCTION_ENTRY("terminate");
		m_bIsTerminate = true;
		m_pConnection->cancelRetry();
		
		FUNCTION_EXIT;
	}

	void ThreadConnResolve::startResolve(boost::shared_ptr<GENARequest> objReq)
	{
		FUNCTION_ENTRY("startResolve");
		if (this->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING || m_bIsTerminate==false)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Previous call to resolve connection has not been terminated! Aborting previous request.");
			terminateAndWait();
			//return;
		}

		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_bIsTerminate = false;
		m_lastRetVal = GENA_UNKNOWN_ERROR;
		m_objReq = objReq;
		// Start thread
		start();

		FUNCTION_EXIT;
	}


} //TA_Base_Bus
