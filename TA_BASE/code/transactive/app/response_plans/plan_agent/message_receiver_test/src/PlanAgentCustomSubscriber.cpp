/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentCustomSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgentCustomSubscriber.h"
#include <iomanip>



PlanAgentCustomSubscriber::PlanAgentCustomSubscriber()
{
	std::cout << "PlanAgentCustomSubscriber started..." << std::endl;
}

void PlanAgentCustomSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	std::cout << "Received custom Message" << std::endl;
	std::ostringstream updt;

	TA_Base_Core::PlanCustomisationUpdate* customData=NULL;

	if ((message.messageState >>= customData))
	{						
		TA_Base_Core::ActivePlanId plan;
		TA_Base_Core::UpdateId updateId;
		TA_Base_Core::StepCustomisableDetail detail;

		updateId = customData->customUpdateId;
		plan.plan = customData->plan.plan;
		plan.instance = customData->plan.instance;
		detail.position = customData->detail.position;
		detail.skip = customData->detail.skip;
		detail.delay = customData->detail.delay;

		std::cout << "Customisation: PlanId:" << plan.plan << " Instance:" 
				  << plan.instance << " Position:" << detail.position
				  << " Skip:" << detail.skip << " Delay:" << detail.delay
				  << "UpdateId:" << updateId;

	}
}

