/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentAuditSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgentAuditSubscriber.h"
#include <iomanip>

namespace PaData = TA_Base_Core;

PlanAgentAuditSubscriber::PlanAgentAuditSubscriber()
{

}

void PlanAgentAuditSubscriber::receiveSpecialisedMessage(const TA_Base_Core::AuditMessageCorbaDef& message)
{

	std::string description = message.messageDescription.in();
}

