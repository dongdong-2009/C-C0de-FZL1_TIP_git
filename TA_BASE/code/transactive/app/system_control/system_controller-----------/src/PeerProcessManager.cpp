/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/PeerProcessManager.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class allows peers to register for updates
  * on this System Controller's process data. The
  * System Controller will call notify() everytime
  * process information changes.
  *
  */

#include "PeerProcessManager.h"
#include "ProcessManager.h"
#include "ProcessUtilities.h"
#include "core/process_management/IDL/src/IProcessSubjectCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // Constructor
    //
    PeerProcessManager::PeerProcessManager()
    {
        FUNCTION_ENTRY("Constructor");

        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "PeerProcessManager");
        activateServant();

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    PeerProcessManager::~PeerProcessManager()
    {
    }

    //
    // registerProcessObserver
    //
	TA_Base_Core::IProcessObserverCorbaDef_ptr PeerProcessManager::registerProcessObserver(
					const char* host, TA_Base_Core::IProcessObserverCorbaDef_ptr observer)
    {
		FUNCTION_ENTRY("registerProcessObserver");

        ProcessManager::getInstance().syncCentralDbOfflineTimerWithPeer(host);

		// Perform the registration process.
		registerPeer(host, observer);
        
		// Return the peer a reference to our ProcessManager so it can also send
		// us process updates.
        FUNCTION_EXIT;
		return TA_Base_Core::IProcessObserverCorbaDef::_duplicate(
			ProcessManager::getInstance()._this());
    }

    //
    // deregisterProcessObserver
    //
    void PeerProcessManager::deregisterProcessObserver(const char* host)
    {
        FUNCTION_ENTRY("deregisterProcessObserver");

        TA_Base_Core::ThreadGuard guard(m_lock);
        
        if (m_observers.find(host) != m_observers.end())
        {
            // Host is in map. 

            CORBA::release(m_observers[host]);

            // Delete this host from the map.
            m_observers.erase(host);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Number of observers is now %d",
                        m_observers.size());
        }

        FUNCTION_EXIT;
    }

	//
	// registerPeer
	//
	void PeerProcessManager::registerPeer(std::string host, 
								TA_Base_Core::IProcessObserverCorbaDef_ptr observer)
	{
		{
			TA_Base_Core::ThreadGuard guard(m_lock);

			std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>::iterator it = 
				m_observers.find( host );

			if (it != m_observers.end())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
					"%s already in map of observers.", host.c_str());
				CORBA::release(it->second);
				m_observers.erase(host);
			}
        
			// set observer corba call timeout to 2 seconds
			omniORB::setClientCallTimeout( observer, 2000 );

			m_observers.insert(
				std::pair<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>
				(host, TA_Base_Core::IProcessObserverCorbaDef::_duplicate(observer)));

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Peer %s registered for process updates", host.c_str()),
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Number of observers is now %d", m_observers.size());
		} // Close threadlock

        // Now that the peer has registered, we need to send it all of our
        // process information.
        std::vector<TA_Base_Core::ProcessData> processes = ProcessManager::getInstance().getAllProcesses();
         
        for (std::vector<TA_Base_Core::ProcessData>::iterator iter = processes.begin(); iter != processes.end(); iter++)
        {
			notify(*iter);
        }
	}

	//
	// notify
	// 
	void PeerProcessManager::notify(const TA_Base_Core::ProcessData& processData)
	{
		// Copy the struct before adding it to the queue.
		TA_Base_Core::ProcessData copy;
		copy.ProcessId = CORBA::string_dup(processData.ProcessId);
        copy.started = processData.started;
        copy.host = CORBA::string_dup(processData.host);
        copy.state = processData.state;
        copy.requestedState = processData.requestedState;
		copy.operationMode = processData.operationMode;
        copy.restarts = processData.restarts;
		copy.debugLevel = CORBA::string_dup(processData.debugLevel);

		TA_Base_Core::ThreadGuard guard(m_lock);

		m_processDataQueue.push(copy);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%d item(s) in process data queue.",
			m_processDataQueue.size());

		m_semaphore.post();
	}

    //
    // sendProcessUpdate
    //
    void PeerProcessManager::sendProcessUpdate(const TA_Base_Core::ProcessData& processData)
    {
        FUNCTION_ENTRY("sendProcessUpdate");
        std::vector<std::string> failedObservers;   // Keep a list of observers
                                                    // which we failed to notify.

		{
			std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr> observerMap;
			{
                // duplicate corba reference in order not hold lock too long due to corba timeout.
				TA_Base_Core::ThreadGuard guard(m_lock);
			    for (std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>::iterator iter = m_observers.begin();
				iter != m_observers.end(); iter++)
                {
                    TA_Base_Core::IProcessObserverCorbaDef_ptr observer = TA_Base_Core::IProcessObserverCorbaDef::_duplicate(iter->second);
                    // set observer corba call timeout to 2 seconds
			        omniORB::setClientCallTimeout( observer, 2000 );
			        observerMap.insert(
				        std::pair<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>
				        (iter->first, observer));
                }
				// yanrong: CL-21195: should put this assert into lock scope
				TA_Assert( observerMap.size() == m_observers.size() );
			}
			
			for (std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>::iterator iter = observerMap.begin(); iter != observerMap.end(); iter++)
			{
                std::stringstream strm;
                strm
                    << "Sending data about process " << processData.ProcessId.in() << " to peer " << iter->first << std::endl
                    << "Time started:    " << processData.started << std::endl
                    << "Host:            " << processData.host.in() << std::endl
                    << "State:           " << processData.state << std::endl
                    << "Requested State: " << processData.requestedState << std::endl
                    << "Operation mode:  " << processData.operationMode << std::endl
                    << "Restarts:        " << processData.restarts << std::endl
                    << "Debug level:     " << processData.debugLevel.in();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", strm.str().c_str());

				try
				{
					iter->second->notifyProcessData(processData);
				}
				catch (CORBA::SystemException& e)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"Error sending process data to peer %s", (iter->first).c_str());
					LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));

					failedObservers.push_back(iter->first);
				}

                // release corba reference
                CORBA::release(iter->second);
			}

		} // Release threadlock
				
        // Go through the list of of observers we failed to notify.
        for (std::vector<std::string>::iterator failIter = failedObservers.begin(); failIter != failedObservers.end(); failIter++)
        {
            // Couldn't send process data to peer, even though we've registered
            // successfully. Better tell the ProcessManager to try to re-register.
            ProcessManager::getInstance().reregisterWithPeer(*failIter);
        }

        FUNCTION_EXIT;
    }

	//
	// run
	//
    void PeerProcessManager::run()
	{
		FUNCTION_ENTRY("run");

		m_isTerminated = false;

		while (!m_isTerminated)
		{
			m_lock.acquire();

			// Send all process updates in the queue.
			while (!m_processDataQueue.empty())
			{
				TA_Base_Core::ProcessData& processData = m_processDataQueue.front();
				
				// Need to release the threadlock before calling sendProcessData(),
				// or the ProcessManager::reregisterWithPeer method may deadlock.

				m_lock.release();
				sendProcessUpdate(processData);
				m_lock.acquire();

				m_processDataQueue.pop();
			}

			m_lock.release();
			m_semaphore.wait();
		}

		FUNCTION_EXIT;
	}

	//
	// terminate
	//
    void PeerProcessManager::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_isTerminated = true;
		m_semaphore.post();

		FUNCTION_EXIT;
	}

    TA_Base_Core::IProcessObserverCorbaDef_var PeerProcessManager::getObserver(const std::string& host)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr>::iterator findIt = m_observers.find(host);

        if (findIt != m_observers.end())
        {
            return TA_Base_Core::IProcessObserverCorbaDef::_duplicate(findIt->second);
        }

        return TA_Base_Core::IProcessObserverCorbaDef::_nil();
    }

} // namespace TA_Base_App
