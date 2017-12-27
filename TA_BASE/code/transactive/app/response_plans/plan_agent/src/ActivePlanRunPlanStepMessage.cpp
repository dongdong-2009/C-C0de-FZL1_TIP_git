/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Bart Golab
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_RUN_PLAN )

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveRunPlanStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
    ActivePlanRunPlanStepMessage::ActivePlanRunPlanStepMessage(const int status, const std::string& error) : 
    m_status(status), 
    m_error(error.c_str())
    {
    }


    ActivePlanRunPlanStepMessage::~ActivePlanRunPlanStepMessage()
    {
    }


    void ActivePlanRunPlanStepMessage::execute( ActivePlan& activePlan ) const
    {
        activePlan.resume();

        ActiveRunPlanStep* activeStep = static_cast<ActiveRunPlanStep*>(activePlan.getCurrentStep());

		if ( TA_Base_Core::PSCS_SUCCESS != m_status )            
	    {            
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure(m_error);
            return;
	    }

        if (activeStep->m_params.waitForCompletion)
        {
			// Must wait for the child plan to complete.
			activePlan.setCompletedInterlock(activeStep->m_childPlan);

		    ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), activeStep->getStepNumber(),
                TA_Base_Core::LOADED_ACTIVE_STEP_STATE, activeStep->generateCompletionPendingRemark() );
        }
        else
        {
		    ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), activeStep->getStepNumber(),
                TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, RemarkPlanStepExecuted );
        }

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        //PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );        
    }
}

#endif