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

#if defined ( STEPTYPE_LOG )

#include <algorithm> 

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanLogStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
	ActivePlanLogStepMessage::ActivePlanLogStepMessage(const int status, const std::string& logMessage) : 
    m_status(status), 
    m_logMessage(logMessage.c_str())
	{
	}

	void ActivePlanLogStepMessage::execute ( ActivePlan& activePlan ) const
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

        std::string session = ActivePlanBulletinBoard::instance()->getSession(activePlan.getPlan());
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(activePlan.getPlan()).path.in();
        const unsigned long ulInstanceNum = activePlan.getPlan().instance;
        std::string strippedLogMessage = removeLineFeed(m_logMessage);

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"LOG STEP:Operator (SessionId[%s]):Plan ID(Number:%u,Instance:%u) logged.(%s).", 
			session.c_str(), activePlan.getPlan().plan, activePlan.getPlan().instance, m_logMessage.c_str() );

        // TD#3400 - Enable Event Viewer to display the message in a readable format. 
        // Remove this once Event Viewer has been modified to properly handle non-printable characters.
		AAMessageSender::instance()->submitAuditOperatorLog( session, strPlanName, strippedLogMessage, ulInstanceNum );

		ActivePlanBulletinBoard::instance()->updateStepState( activePlan.getPlan(), stepNumber, TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, RemarkPlanStepExecuted );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
    }

    // TD#3400
    std::string ActivePlanLogStepMessage::removeLineFeed ( const std::string& message ) const
    {
        std::string log = message;

        std::replace(log.begin(), log.end(), '\n', ' ');
        std::string::iterator last = std::remove(log.begin(), log.end(), '\r');

        log.resize(last - log.begin());

        return log;
    }
}

#endif