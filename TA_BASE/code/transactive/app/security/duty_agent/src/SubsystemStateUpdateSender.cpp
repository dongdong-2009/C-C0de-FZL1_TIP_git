/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateUpdateSender.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to PeerStateChangeSubject and sends updates to Duty Managers.
  *
  */

#include <sstream>
#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessageConstants.h"

#include "DutyAgent.h"
#include "SubsystemStateUpdateSender.h"
#include "SubsystemStateUpdateConverter.h"

namespace TA_Base_App
{
    SubsystemStateUpdateSender::SubsystemStateUpdateSender() :
        TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>(*TA_Base_Bus::PeerStateChangeSubject::instance()),
        TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
        m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
            getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))
    {
        FUNCTION_ENTRY("SubsystemStateUpdateSender");
        FUNCTION_EXIT;
    }

		void SubsystemStateUpdateSender::update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state )
    {
        FUNCTION_ENTRY("PeerStateChangeSubjectImpl::update");

        // TD12082: Subsystem state change messages should be sent any time a subsystem changes state
        // and should therefore not be limited to any particular initiating action.

        // Convert the provided DutyState object to a sequence of changed subsystem states.
        SubsystemStateUpdateConverter stateConverter;
        state.convert( stateConverter );

        // Get the list of changed states.
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStates = stateConverter.getSubsystemStates();

        // Only bother trying to send the message if there were some changes.
        if ( 0 < subsystemStates->length() )
        {
            // Convert the region key into a string for the filter.
            std::stringstream regionStr;
            regionStr << stateConverter.getRegionKey();

            // Create the filter.
            // Filters to not currently work on broadcasts, however this is provided in case support is
            // enabled when federated broadcasts are introduced.
            TA_Base_Core::FilterData filterData;
            TA_Base_Core::NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, regionStr.str() );
            filterData.push_back( &regionFilter );

			std::ostringstream dataUpdateInfo;
			for(int i=0; i<subsystemStates->length(); ++i)
			{
				dataUpdateInfo << "["
					<< subsystemStates[i].regionKey << ","
					<< subsystemStates[i].subsystemKey << ","
					<< subsystemStates[i].prevState	<< ":"
					<< subsystemStates[i].currState << "]";
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CL-21120] SubsystemStateUpdateSender::update[locationKey, subsystemKey,prevState:currentSate]:%s",
				dataUpdateInfo.str().c_str());

            // Send a broadcast comms message containing the state information.
            CORBA::Any commsData;
            commsData <<= subsystemStates;
            m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification, commsData, &filterData );
        }

        FUNCTION_EXIT;
    }

    void SubsystemStateUpdateSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
    {
        FUNCTION_ENTRY("AgentTerminateSubjectImpl::update");

        PeerStateParent::detach();
        AgentTerminateParent::detach();

        FUNCTION_EXIT;
    }
}
