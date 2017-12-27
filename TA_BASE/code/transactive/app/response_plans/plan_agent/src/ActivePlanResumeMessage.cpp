/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"

namespace TA_Base_App
{
	ActivePlanResumeMessage::ActivePlanResumeMessage()
	{
	}
	
	void ActivePlanResumeMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.resume();
	}

} // TA_Base_App
