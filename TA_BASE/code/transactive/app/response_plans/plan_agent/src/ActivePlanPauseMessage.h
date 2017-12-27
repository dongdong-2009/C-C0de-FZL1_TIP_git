#ifndef ACTIVEPLANPAUSEMESSAGE_H_INCLUDED
#define ACTIVEPLANPAUSEMESSAGE_H_INCLUDED

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
  *  <description>
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanPauseMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanPauseMessage();

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanPauseMessage& operator=( const ActivePlanPauseMessage& );
		ActivePlanPauseMessage( const ActivePlanPauseMessage& );

	}; // ActivePlanPauseMessage
}; // TA_Base_App

#endif // ACTIVEPLANPAUSEMESSAGE_H_INCLUDED
