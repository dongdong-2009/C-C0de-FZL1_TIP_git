/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/QueueStateUpdater.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements QueueStateUpdater.
 */

#include "QueueStateUpdater.h"
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "core/data_access_interface/src/ISession.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/AuthenticationAgentStateUpdate_MessageTypes.h"

namespace TA_Base_App
{

    QueueStateUpdater::QueueStateUpdater( unsigned long entityKey ) :
		m_sender(TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                        TA_Base_Core::AuthenticationAgentStateUpdate::Context)),
		m_entityKey(entityKey)
    {
    }

	void QueueStateUpdater::sendStateUpdateMessage(TA_Base_Core::ISession* item,bool isNew)
    {
        FUNCTION_ENTRY("sendStateUpdateMessage");

        // Create an any into which the session object can be placed
        CORBA::Any stateUpdate;

        // Create a TA_Base_Bus::SessionInfoCorbaDef object that will carry the information across the wire
        TA_Base_Bus::SessionInfoCorbaDef sessionInfo;
        
        sessionInfo.SessionId = CORBA::string_dup(item->getSessionId().c_str());
        sessionInfo.WorkstationId = item->getConsoleKey();
        sessionInfo.ProfileId = item->getProfileKey();
        sessionInfo.UserId = item->getOperatorKey();
		sessionInfo.isDisplayOnly = item->getDisplayMode();
        sessionInfo.additionalOperators.length(0);
        sessionInfo.additionalProfiles.length(0);

        if (isNew)
        {
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionStart;
        }
        else
        {
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionEnd;
        }

        // Place the sequence into the state update
        stateUpdate <<= sessionInfo;

        //TD15069 azenitha++
        if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// Send the message
			m_sender->sendStateUpdateMessage(
						TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdate,
						m_entityKey,
						TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
						stateUpdate);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "[TD15069] Expected sending of AuthenticationAgentStateUpdate (System is in Control Mode)");
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of AuthenticationAgentStateUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha
		
        FUNCTION_EXIT;
    }
}
