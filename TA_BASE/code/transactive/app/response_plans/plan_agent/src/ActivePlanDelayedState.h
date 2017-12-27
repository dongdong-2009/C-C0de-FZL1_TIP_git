#ifndef ACTIVEPLANDELAYEDSTATE_H_INCLUDED
#define ACTIVEPLANDELAYEDSTATE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Justin Ebedes
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A class to represent the state of a plan that is currently in
  *  delay.
  */

#include <string>

#include "ace/Singleton.h"

#include "app/response_plans/plan_agent/src/ActivePlanState.h"

namespace TA_Base_Core
{
    struct StepCustomisableDetail;
}

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanDelayedStateImpl : public ActivePlanState
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanDelayedStateImpl, ACE_Recursive_Thread_Mutex>;

		virtual int executeNextStep( ActivePlan& activePlan ) const;

		virtual void stop( ActivePlan& activePlan, const std::string& reason ) const;
		virtual void complete( ActivePlan& activePlan ) const;
		virtual void pause( ActivePlan& activePlan ) const;
		virtual void pauseForOperator( ActivePlan& activePlan ) const;
		virtual void resume( ActivePlan& activePlan ) const;
		virtual void reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const;
		virtual void unassign( ActivePlan& activePlan ) const;
		virtual void customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const;

	private:
		ActivePlanDelayedStateImpl(){}
		~ActivePlanDelayedStateImpl(){}
		ActivePlanDelayedStateImpl( const ActivePlanDelayedStateImpl& );
		ActivePlanDelayedStateImpl& operator=( const ActivePlanDelayedStateImpl& );

	}; // ActivePlanDelayedState
	typedef ACE_Singleton<ActivePlanDelayedStateImpl, ACE_Recursive_Thread_Mutex> ActivePlanDelayedState;
}; // TA_Base_App

#endif // ACTIVEPLANDELAYEDSTATE_H_INCLUDED
