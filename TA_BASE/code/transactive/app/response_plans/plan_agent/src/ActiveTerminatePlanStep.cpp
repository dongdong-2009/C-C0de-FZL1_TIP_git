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

#if defined ( STEPTYPE_TERMINATE_PLAN )

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveTerminatePlanStep::ActiveTerminatePlanStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::TerminatePlanParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveTerminatePlanStep::executeImpl( void )
	{
		FUNCTION_ENTRY( "ActiveTerminatePlanStep::executeImpl( void )" );
		bool instanceFound = false;

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: terminating plan id %lu", dumpContext().c_str(), m_params.plan );

        std::list<TA_Base_Core::ActivePlanId> instanceList;
		ActivePlanBulletinBoard::instance()->getActiveInstances( m_params.plan, instanceList );

        std::ostringstream reasonStrm;
        reasonStrm << "Terminated by instance " << m_flowControl.getPlan().instance << " of plan '"
            << ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in() << "'";

        std::string reason = reasonStrm.str();

		for( std::list<TA_Base_Core::ActivePlanId>::iterator it = instanceList.begin(); it != instanceList.end(); ++it )
		{
			// make sure we don't tell ourself to terminate...
			if( m_flowControl.getPlan().plan != (*it).plan )
			{
				PlanExecutor::instance()->stop( *it, reason );
				instanceFound = true;
			}
			else
			{
				if( m_flowControl.getPlan().instance != (*it).instance )
				{
					PlanExecutor::instance()->stop( *it, reason );
					instanceFound = true;
				}
			}
		}

		// Tell the active plan not to execute the next step until
		// the specified plan has terminated.
		if( instanceFound )
		{	//m_flowControl.setNextStep(); //Test setNextStep() MP
			m_flowControl.setTerminatedInterlock( m_params.plan );
		}
        else
        {
            stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
        }

		m_flowControl.setNextStep();

		FUNCTION_EXIT;
		return stepState;
	}
} // TA_Base_App


#endif //  STEPTYPE_TERMINATE_PLAN
