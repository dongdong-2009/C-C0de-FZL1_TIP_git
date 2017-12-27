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
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_JUMP )

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActiveJumpStep.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveJumpStep::ActiveJumpStep( const TA_Base_Core::StepDetail& stepDetail,  
        const TA_Base_Core::JumpParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params)
	{
	}

    TA_Base_Core::EActiveStepState ActiveJumpStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: jumping to step %lu", dumpContext().c_str(), m_params.step );

		if ( !m_flowControl.setNextStep( m_params.step ))
        {
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

        return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App


#endif //  STEPTYPE_JUMP
