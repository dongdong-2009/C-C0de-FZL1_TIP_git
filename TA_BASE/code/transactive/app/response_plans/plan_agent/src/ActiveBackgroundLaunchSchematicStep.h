#if defined ( STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC )

#ifndef ACTIVEBACKGROUNDLAUNCHSCHEMATICSTEP_H_INCLUDED
#define ACTIVEBACKGROUNDLAUNCHSCHEMATICSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveBackgroundLaunchSchematicStep.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Represents a Launch Schematic step in an executing plan.
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	// Class declaration
	class ActiveBackgroundLaunchSchematicStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveBackgroundLaunchSchematicStep( const TA_Base_Core::StepDetail& stepDetail, 
            const TA_Base_Core::BackgroundLaunchSchematicParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveBackgroundLaunchSchematicStep& operator=( const ActiveBackgroundLaunchSchematicStep& );
		ActiveBackgroundLaunchSchematicStep( const ActiveBackgroundLaunchSchematicStep& );

		const TA_Base_Core::BackgroundLaunchSchematicParameters m_params;
        unsigned short m_sendLauchSchemsticTryTimes;
	}; // ActiveBackgroundLaunchSchematicStep

}; // TA_Base_App

#endif // ACTIVEBACKGROUNDLAUNCHSCHEMATICSTEP_H_INCLUDED

#endif //  STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC
