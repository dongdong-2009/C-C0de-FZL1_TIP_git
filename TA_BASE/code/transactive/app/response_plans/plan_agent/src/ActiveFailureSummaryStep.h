#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )

#ifndef ACTIVEFAILURESUMMARYSTEP_H_INCLUDED
#define ACTIVEFAILURESUMMARYSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Lizette Lingo
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
	class ActiveFailureSummaryStep : public ActiveStep
	{
	// Public methods
	public:
			ActiveFailureSummaryStep( const TA_Base_Core::StepDetail& stepDetail,
                const TA_Base_Core::FailureSummaryParameters& params, IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		
		ActiveFailureSummaryStep& operator=( const ActiveFailureSummaryStep& );
		ActiveFailureSummaryStep( const ActiveFailureSummaryStep& );

		const TA_Base_Core::FailureSummaryParameters m_params;
		IActivePlanFlow& m_flowControl;

	}; // ActiveFailureSummaryStep
}; // TA_Base_App

#endif // ACTIVEFAILURESUMMARYSTEP_H_INCLUDED

#endif // STEPTYPE_FAILURE_SUMMARY_STEP 
