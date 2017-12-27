// ActiveNullStep.cpp: implementation of the ActiveNullStep class.
//
//////////////////////////////////////////////////////////////////////
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActiveNullStep.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{

	ActiveNullStep::ActiveNullStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl)
	{
			m_detail.ignoreFailure = true;
			m_detail.skip = true;
			m_detail.skippable = true;
	}

	TA_Base_Core::EActiveStepState ActiveNullStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "should not execute the null step" );

		m_flowControl.setNextStep(); // Set NextStep: MP

		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

}