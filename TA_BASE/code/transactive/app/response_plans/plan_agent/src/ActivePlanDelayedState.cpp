/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Justin Ebedes
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A class to represent the state of a plan that is currently in
  *  delay.
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanDelayedState.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
	int ActivePlanDelayedStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
        return( defaultExecuteNextStep( activePlan ) );	
    }

	void ActivePlanDelayedStateImpl::stop( ActivePlan& activePlan, const std::string& reason ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.getPlan(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}

	void ActivePlanDelayedStateImpl::complete( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

    void ActivePlanDelayedStateImpl::pause( ActivePlan& activePlan ) const
	{
        if ( activePlan.isPausePending() )
        {
            return;
        }
       
        activePlan.schedulePause();
        activePlan.publishNextStepDetail();

		changeState( activePlan, ActivePlanPausedState::instance() );

		// The plan manager would like to see which step will be executed next
		// rather than which step is currently loaded so we need a special
		// update message to give then that information.
		activePlan.publishNextStepDetail();

		std::string remark = activePlan.getRemark();
		
		if ( remark.empty() )
        {
            remark = RemarkPlanPausedByOperator;
        }

		ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), activePlan.getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, remark );
	}

	void ActivePlanDelayedStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanDelayedStateImpl::resume( ActivePlan& activePlan ) const
	{
        // Do nothing
    }

	void ActivePlanDelayedStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		defaultReassign( activePlan, sessionId, pmId );
	}

	void ActivePlanDelayedStateImpl::unassign( ActivePlan& activePlan ) const
	{
		defaultUnassign( activePlan );
	}

	void ActivePlanDelayedStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
        // Do nothing
    }
} // TA_Base_App
