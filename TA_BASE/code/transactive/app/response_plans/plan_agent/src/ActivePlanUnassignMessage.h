#ifndef ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED
#define ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED

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
	class ActivePlanUnassignMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanUnassignMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanUnassignMessage& operator=( const ActivePlanUnassignMessage& );
		ActivePlanUnassignMessage( const ActivePlanUnassignMessage& );

	}; // ActivePlanUnassignMessage
}; // TA_Base_App

#endif // ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED
