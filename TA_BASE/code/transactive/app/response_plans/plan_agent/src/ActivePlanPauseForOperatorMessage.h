#ifndef ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED
#define ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED

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

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanPauseForOperatorMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanPauseForOperatorMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanPauseForOperatorMessage& operator=( const ActivePlanPauseForOperatorMessage& );
		ActivePlanPauseForOperatorMessage( const ActivePlanPauseForOperatorMessage& );

	}; // ActivePlanPauseMessage
}; // TA_Base_App

#endif // ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED
