/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentAuditSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef PLANAGENTAUDITSUBSCRIBER_H_INCLUDED
#define PLANAGENTAUDITSUBSCRIBER_H_INCLUDED

#pragma warning( disable : 4786 )

#include "core/message/src/MessagePublicationManager.h"
//#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/AuditMessageCorbaDef.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

class PlanAgentAuditSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::AuditMessageCorbaDef>
        {
        public:
			PlanAgentAuditSubscriber();

			virtual void receiveSpecialisedMessage(const TA_Base_Core::AuditMessageCorbaDef& message);

		private:
        };

#endif //PLANAGENTAUDITSUBSCRIBER_H_INCLUDED
