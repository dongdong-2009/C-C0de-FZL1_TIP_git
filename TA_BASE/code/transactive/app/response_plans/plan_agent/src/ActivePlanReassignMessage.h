#ifndef ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED
#define ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED

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

#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanReassignMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanReassignMessage(const std::string& session, const std::string& pmId);

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanReassignMessage& operator=( const ActivePlanReassignMessage& );
		ActivePlanReassignMessage( const ActivePlanReassignMessage& );

		std::string m_sessionId;
        std::string m_pmId;

	}; // ActivePlanReassignMessage
}; // TA_Base_App

#endif // ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED
