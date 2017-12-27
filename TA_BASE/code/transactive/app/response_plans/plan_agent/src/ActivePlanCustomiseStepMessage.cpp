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
#include "app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h"

namespace TA_Base_App
{
    ActivePlanCustomiseStepMessage::ActivePlanCustomiseStepMessage( const TA_Base_Core::StepCustomisableDetail& detail ) :
    m_detail(detail),
    m_historyUpdate(),
    m_bUpdateHistory( false )
	{
	}
    
    ActivePlanCustomiseStepMessage::ActivePlanCustomiseStepMessage( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate ) :
    m_detail(),
    m_historyUpdate( historyUpdate ),
    m_bUpdateHistory( true )
    {
    }

	void ActivePlanCustomiseStepMessage::execute( ActivePlan& activePlan ) const
	{
// 		activePlan.customiseStep( m_detail );
        if ( m_bUpdateHistory )
        {
            activePlan.customiseSteps( m_historyUpdate.history.details );
        }
        else
        {
            activePlan.customiseStep( m_detail );
        }
	}

} // TA_Base_App
