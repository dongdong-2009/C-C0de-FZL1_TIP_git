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
#include "app/response_plans/plan_agent/src/ActivePlanPauseMessage.h"

namespace TA_Base_App
{
    ActivePlanPauseMessage::ActivePlanPauseMessage()
    {
    }

	void ActivePlanPauseMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.pause();
	}

} // TA_Base_App
