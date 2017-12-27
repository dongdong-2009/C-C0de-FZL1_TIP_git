/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/StateUpdateSender.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Sends Online Printing Agent state update messages.
  * 
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;

	
StateUpdateSender::StateUpdateSender() :
	m_messageSender(NULL)
{
	FUNCTION_ENTRY("StateUpdateSender");

    m_messageSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                          TA_Base_Core::OnlinePrintingAgentStateUpdate::Context);

	FUNCTION_EXIT;
}


StateUpdateSender::~StateUpdateSender()
{
	FUNCTION_ENTRY("~StateUpdateSender");

    delete m_messageSender;
    m_messageSender = NULL;
    
	FUNCTION_EXIT;
}


void StateUpdateSender::sendEventProcessedUpdate(EventSequenceNumber lastProcessedEvent)
{
	FUNCTION_ENTRY("sendEventProcessedUpdate");
    TA_ASSERT(m_messageSender != NULL, "Message sender pointer is NULL");

    try
    {
	    CORBA::Any stateUpdateData;
	    stateUpdateData <<= lastProcessedEvent;

        m_messageSender->sendStateUpdateMessage(TA_Base_Core::OnlinePrintingAgentStateUpdate::EventProcessedUpdate,
                                                ConfigData::getInstance().getAgentEntityKey(), 
                                                ConfigData::getInstance().getAgentEntityName(), 
                                                stateUpdateData);
    }
    catch (const std::exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send event processed update for event %lu", lastProcessedEvent);
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send event processed update for event %lu", lastProcessedEvent);
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send event processed update for event %lu", lastProcessedEvent);
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
    }

	FUNCTION_EXIT;
}


void StateUpdateSender::requestStateUpdate()
{
	FUNCTION_ENTRY("requestStateUpdate");
    TA_ASSERT(m_messageSender != NULL, "Message sender pointer is NULL");

    try
    {
	    CORBA::Any unused;

	    m_messageSender->sendStateUpdateMessage(TA_Base_Core::OnlinePrintingAgentStateUpdate::OnlinePrintingAgentStateUpdateRequest,
                                                ConfigData::getInstance().getAgentEntityKey(), 
                                                ConfigData::getInstance().getAgentEntityName(), 
                                                unused);
    }
    catch (const std::exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send state update request");
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send state update request");
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send state update request");
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
    }

	FUNCTION_EXIT;
}
