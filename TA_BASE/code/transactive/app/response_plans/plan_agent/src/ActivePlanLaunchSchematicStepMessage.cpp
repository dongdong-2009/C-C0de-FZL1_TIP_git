
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
  *  Generated when the result of a Launch Schematic interactive step execution request
  *  is received back from the Plan Manager.
  */
#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_LAUNCH_SCHEMATIC )

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"


namespace TA_Base_App
{
    ActivePlanLaunchSchematicStepMessage::ActivePlanLaunchSchematicStepMessage(const int status, const std::string& error) : 
    m_status(status),
    m_error(error.c_str())
    {
    }


    ActivePlanLaunchSchematicStepMessage::~ActivePlanLaunchSchematicStepMessage()
    {
    }


    void ActivePlanLaunchSchematicStepMessage::execute( ActivePlan& activePlan ) const
    {
        activePlan.resume();

        ActiveStep* activeStep = activePlan.getCurrentStep();

		if ( TA_Base_Core::PSCS_SUCCESS != m_status )            
	    {
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure(m_error);
            return;
	    }

		ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), activeStep->getStepNumber(),
            TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, RemarkPlanStepExecuted );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        //PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );   
    }
}

#endif