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
  *  <description>
  */
#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_PROMPT )

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"


namespace TA_Base_App
{
    ActivePlanPromptStepMessage::ActivePlanPromptStepMessage( const int status) : 
    m_status (status)
    {

    }

    void ActivePlanPromptStepMessage::execute ( ActivePlan& activePlan ) const
    {
        activePlan.resume();

        ActiveStep* activeStep = activePlan.getCurrentStep();

        TA_Base_Core::StepNumber stepNumber = activeStep->getStepNumber();

		if( TA_Base_Core::PSCS_ACKNOWLEDGEMENT != m_status )
	    {
            activePlan.setInteractiveStepFailed();
            std::string strReason = (m_status == TA_Base_Core::PSCS_TIMEOUT) ? RemarkStepExecutionTimeout : "";
            activeStep->processStepExecutionFailure(strReason);

            return;
	    }

        ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), stepNumber, TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, RemarkPlanStepExecuted );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
    }
}

#endif