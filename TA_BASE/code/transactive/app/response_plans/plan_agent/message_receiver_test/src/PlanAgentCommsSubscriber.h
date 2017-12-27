/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentCommsSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef PLANAGENTCOMMSSUBSCRIBER_H_INCLUDED
#define PLANAGENTCOMMSSUBSCRIBER_H_INCLUDED

#pragma warning( disable : 4786 )

 
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

class PlanAgentCommsSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
        {
        public:
			PlanAgentCommsSubscriber();

			virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

		private:
			typedef std::map<TA_Base_Core::EActivePlanState,std::string> PlanStateMap;
			typedef std::map<TA_Base_Core::EActiveStepState,std::string> StepStateMap;

			PlanStateMap m_planStateMap;
			StepStateMap m_stepStateMap;
			std::string m_session;
        };

#endif //PLANAGENTCOMMSSUBSCRIBER_H_INCLUDED
