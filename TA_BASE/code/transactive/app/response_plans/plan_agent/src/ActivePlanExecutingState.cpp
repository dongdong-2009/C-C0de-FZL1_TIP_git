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
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanOperatorPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanExecutingState.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
	int ActivePlanExecutingStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
		return( defaultExecuteNextStep( activePlan ) );
	}

	void ActivePlanExecutingStateImpl::stop( ActivePlan& activePlan, const std::string& reason ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.getPlan(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}


	void ActivePlanExecutingStateImpl::complete( ActivePlan& activePlan ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.getPlan(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, RemarkPlanCompletedSuccess );		
	}

    void ActivePlanExecutingStateImpl::pause( ActivePlan& activePlan ) const
	{
        if ( activePlan.isPausePending() )
        {
            return;
        }

        activePlan.schedulePause();

        changeState( activePlan, ActivePlanPausedState::instance() );

        activePlan.publishNextStepDetail();

//		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(), activePlan.getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, "Plan paused by operator" );
	}

	void ActivePlanExecutingStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		changeState( activePlan, ActivePlanOperatorPausedState::instance() );
		// the plan manager would like to see which step will be executed next
		// rather than which step is currently loaded so we need a special
		// update message to give then that information.
		activePlan.publishNextStepDetail();
        activePlan.setOperatorResponseInterlock(true);

        // Update to Operator Pause state for Monitor Agent update
        ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.getPlan(), TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE, RemarkWaitingForOperatorResponse );

		ActivePlanBulletinBoard::instance()->sendOperatorPausedStateUpdate( activePlan.getPlan() );
	}

	void ActivePlanExecutingStateImpl::resume( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanExecutingStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		activePlan.scheduleTakeControl( sessionId, pmId );
	}

	void ActivePlanExecutingStateImpl::unassign( ActivePlan& activePlan ) const
	{
		defaultUnassign( activePlan );
	}

    void ActivePlanExecutingStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		// Do nothing.
	}

} // TA_Base_App
