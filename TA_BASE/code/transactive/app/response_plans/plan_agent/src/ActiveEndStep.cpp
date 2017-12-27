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

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveEndStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

    ActiveEndStep::ActiveEndStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl)
    {
    }

	TA_Base_Core::EActiveStepState ActiveEndStep::executeImpl( void )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: Completed.", dumpContext().c_str() );

		m_flowControl.setIsActive( false );

        if ( m_flowControl.anyPlanStepFailed() )
        {
			m_remark = RemarkPlanCompletedWithFailedSteps;
        }
        else
        {
			m_remark = RemarkPlanCompletedSuccess;
        }

		ActivePlanBulletinBoard::instance()->updatePlanState( m_flowControl.getPlan(), TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );

        std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in();
        const unsigned long ulInstanceNum = m_flowControl.getPlan().instance;

		if (ActivePlanBulletinBoard::instance()->isGRCPlan (m_flowControl.getPlan()))
		{
			if (ActivePlanBulletinBoard::instance()->isManualPlan(m_flowControl.getPlan()))
			{
                AAMessageSender::instance()->submitAuditManualGrcSucceed(strPlanName, ulInstanceNum);
			}
			else//automatic GRC
            {
                AAMessageSender::instance()->submitAuditAutoGrcSucceed(strPlanName, ulInstanceNum);
			}
		}
		else//DSS plans
		{
            AAMessageSender::instance()->submitAuditPlanCompleted( session, strPlanName, ulInstanceNum );
		}

        std::ostringstream auditMessage;
		auditMessage << strPlanName.c_str() << "[Id:" << m_flowControl.getPlan().plan << ",Instance:" 
            << m_flowControl.getPlan().instance << "]. has completed.";

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());

        return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App
