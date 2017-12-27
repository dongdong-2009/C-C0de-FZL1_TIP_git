/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/core/naming/src/HeartbeatSender.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/02/27 19:31:54 $
  * Last modified by:  $Author: avteam $
  *
  * This is a workaround and should be removed once naming issue had been fixed.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/naming/src/NamingHeartbeatMonitor.h"
#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    const unsigned long NamingHeartbeatMonitor::DEFAULT_HEARTBEAT_INTERVAL      = 60;
    const unsigned long NamingHeartbeatMonitor::DEFAULT_HEARTBEAT_FAILLED_COUNT = 2;
    const std::string   NamingHeartbeatMonitor::PARAM_NAMING_HEARTBEAT_INTERVAL = "NamingHeartbeat";

    NamingHeartbeatMonitor::NamingHeartbeatMonitor(std::string agentName)
        : m_agentName(agentName),
          m_inControlMode(false),
          m_lastHeartbeat(0),
          m_bindingRequestSender(NULL)
    {
        FUNCTION_ENTRY("NamingHeartbeatMonitor");
        m_heartbeatInterval = getRunParamValue(PARAM_NAMING_HEARTBEAT_INTERVAL, DEFAULT_HEARTBEAT_INTERVAL);
        const unsigned long DEFAULT_RESOLVE_TIMEOUT_MS = 2000; // 2 seconds
        m_resolveTimeoutMs = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, DEFAULT_RESOLVE_TIMEOUT_MS);

        activateServant();

		m_bindingRequestSender = std::auto_ptr<BindingRequestMessageSender>(
			TA_Base_Core::MessagePublicationManager::getInstance().getBindingRequestMessageSender(
				TA_Base_Core::NamingService::BindName) );

		TA_ASSERT( NULL != m_bindingRequestSender.get(), "Unable to get the BindingRequestMessageSender" );

        FUNCTION_EXIT;
    }


    NamingHeartbeatMonitor::~NamingHeartbeatMonitor()
    {
        FUNCTION_ENTRY("~NamingHeartbeatMonitor");
        FUNCTION_EXIT;
    }


    void NamingHeartbeatMonitor::run()
    {
        FUNCTION_ENTRY("run");

        m_isRunning = true;

        while (m_isRunning)
        {
            {
                ThreadGuard guard(m_condition.getLinkedThreadLockable());
                if (m_inControlMode)
                {
                    m_condition.timedWait( m_heartbeatInterval * 1000 );
                }
                else
                {
                    m_condition.wait();
                }
            }

            sendNamingHeartbeat();

            {
                ThreadGuard guard(m_condition.getLinkedThreadLockable());
                m_condition.timedWait( m_resolveTimeoutMs );
            }

            time_t currTime = time( NULL );
            {
                ThreadGuard guard( m_lock );

                if ((currTime - m_lastHeartbeat) >= m_heartbeatInterval * DEFAULT_HEARTBEAT_FAILLED_COUNT)
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                        "Agent will be terminated! last heartbeat recieved: %l, current time: %l", m_lastHeartbeat, currTime );

                    // didn't receive heartbeat, kill this agent.
                    exit(1);
                }
            }

            sleep(100);
        }

        FUNCTION_EXIT;
    }


    void NamingHeartbeatMonitor::terminate()
    {
        m_isRunning = false;
    }


    void NamingHeartbeatMonitor::doOperationModeChange(bool toControl)
    {
        m_inControlMode = toControl;

        ThreadGuard guard1( m_lock );
        m_lastHeartbeat = time( NULL );

        if (m_inControlMode)
        {
            ThreadGuard guard2(m_condition.getLinkedThreadLockable());
            m_condition.signal();
        }
    }


    void NamingHeartbeatMonitor::sendNamingHeartbeat()
    {
        FUNCTION_ENTRY("sendNamingHeartbeat");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "NamingHeartbeatMonitor::sendNamingHeartbeat sending heartbeat" );

        m_bindingRequestSender->sendBindingRequest( TA_Base_Core::NamingService::BindName,
                                                    m_agentName,
                                                    this->_this() );

        FUNCTION_EXIT;
    }


    void NamingHeartbeatMonitor::callback( 
        NamedObjectRepositoryCorbaDef_ptr requestedObject, CORBA::ULong timeToLive )
    {
        FUNCTION_ENTRY("callback");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "NamingHeartbeatMonitor::callback receive heartbeat" );

        ThreadGuard guard1( m_lock );
        m_lastHeartbeat = time( NULL );
        ThreadGuard guard2(m_condition.getLinkedThreadLockable());
        m_condition.signal();

        FUNCTION_EXIT;
    }


}; // TA_Base_Core
