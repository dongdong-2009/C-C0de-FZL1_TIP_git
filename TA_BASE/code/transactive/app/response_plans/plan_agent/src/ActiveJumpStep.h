#if defined ( STEPTYPE_JUMP )

#ifndef ACTIVEJUMPSTEP_H_INCLUDED
#define ACTIVEJUMPSTEP_H_INCLUDED

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
	class ActiveJumpStep : public ActiveStep
	{
	// Public methods
	public:
        ActiveJumpStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::JumpParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveJumpStep& operator=( const ActiveJumpStep& );
		ActiveJumpStep( const ActiveJumpStep& );

        const TA_Base_Core::JumpParameters m_params;
	}; // ActiveJumpStep
}; // TA_Base_App

#endif // ACTIVEJUMPSTEP_H_INCLUDED

#endif // STEPTYPE_JUMP 
