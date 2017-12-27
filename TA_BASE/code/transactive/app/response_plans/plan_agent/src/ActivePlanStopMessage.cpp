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
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"

namespace TA_Base_App
{
    ActivePlanStopMessage::ActivePlanStopMessage( const std::string& reason) : 
    m_reason( reason.c_str() )
    {
    }

	void ActivePlanStopMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.stop( m_reason );
	}

} // TA_Base_App
