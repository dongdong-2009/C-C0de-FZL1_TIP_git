#ifndef ACTIVE_END_STEP_H_INCLUDED
#define ACTIVE_END_STEP_H_INCLUDED

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

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveEndStep : public ActiveStep
	{
	// Public methods
	public:
	    ActiveEndStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveEndStep& operator=( const ActiveEndStep& );
		ActiveEndStep( const ActiveEndStep& );
	}; // ActiveEndStep
}; // TA_Base_App

#endif // ACTIVE_END_STEP_H_INCLUDED
