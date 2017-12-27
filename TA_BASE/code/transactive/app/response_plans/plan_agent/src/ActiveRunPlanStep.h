#if defined ( STEPTYPE_RUN_PLAN )

#ifndef ACTIVERUNPLANSTEP_H_INCLUDED
#define ACTIVERUNPLANSTEP_H_INCLUDED

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

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveRunPlanStep : public ActiveStep
	{
        friend class ActivePlanRunPlanStepMessage;

	// Public methods
	public:
        ActiveRunPlanStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::RunPlanParameters& params, IActivePlanFlow& flowControl );
		
	protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );
        std::string generateCompletionPendingRemark() const;

	private:
		ActiveRunPlanStep& operator=( const ActiveRunPlanStep& );
		ActiveRunPlanStep( const ActiveRunPlanStep& );

        const TA_Base_Core::RunPlanParameters m_params;
		TA_Base_Core::ActivePlanId m_childPlan;

	}; // ActiveRunPlanStep
}; // TA_Base_App

#endif // ACTIVERUNPLANSTEP_H_INCLUDED

#endif // STEPTYPE_RUN_PLAN
