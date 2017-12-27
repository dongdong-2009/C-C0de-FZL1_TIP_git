/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseForOperatorMessage.h"

namespace TA_Base_App
{
	void ActivePlanPauseForOperatorMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.pauseForOperator();
	}

} // TA_Base_App
