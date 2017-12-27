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
#include "app/response_plans/plan_agent/src/ActivePlanState.h"

namespace TA_Base_App
{
	int ActivePlanState::defaultExecuteNextStep( ActivePlan& activePlan ) const
	{
		return activePlan.executeNextStepImpl();
    }

    int ActivePlanState::defaultWaitCurStep( ActivePlan& activePlan ) const
    {
        return activePlan.waitCurStepImpl();
    }

	void ActivePlanState::defaultReassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		activePlan.reassignImpl(sessionId, pmId);
	}

	void ActivePlanState::defaultUnassign( ActivePlan& activePlan ) const
	{
	}
	
	void ActivePlanState::defaultCustomiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		activePlan.customiseStepImpl( newDetail );
	}

	void ActivePlanState::changeState( ActivePlan& activePlan, ActivePlanState* state ) const
	{
		activePlan.changeState( state );
	}

} // TA_Base_App
