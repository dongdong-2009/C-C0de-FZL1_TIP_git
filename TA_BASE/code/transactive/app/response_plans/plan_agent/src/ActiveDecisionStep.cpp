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

#if defined ( STEPTYPE_DECISION )

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveDecisionStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveDecisionStep::ActiveDecisionStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::DecisionParameters& params, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl, params.responseTimeout),
    m_params(params)
	{
	}

    TA_Base_Core::StepNumber ActiveDecisionStep::getNextDecisionStep(const int status)
    {
        if ( status == TA_Base_Core::PSCS_YES_RESPONSE )
            return m_params.yesStep;
        else if ( status == TA_Base_Core::PSCS_NO_RESPONSE )
            return m_params.noStep;
		else if ( status == TA_Base_Core::PSCS_TIMEOUT )
			return 0;

		return 0;
    }

	TA_Base_Core::EActiveStepState ActiveDecisionStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Decision step %lu", getStepNumber() );

      TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		m_remark = RemarkWaitingForOperatorResponse;

		TA_Base_Core::CurrentPlanStepDetail detail;

		detail.activePlan = m_flowControl.getPlan();
		detail.step = getStepNumber();
        detail.data.decision(m_params);

		TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;
		try
		{
			ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);

			if ( planManager != NULL )
			{
				if ( planManager->_non_existent() )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Reference to non existent plan manager");
					stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
					m_remark = RemarkPlanManagerIsNotRegistered;
				}
				else
                {
                    try
                    {
					    planManager->executeInteractiveStep(detail); 
                        m_flowControl.setInteractiveStep(true);
                    }
                    catch (const TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError& isee)
                    {   
                        std::string message = "ActiveDecisionStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                        message += isee.reason.in();
			            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );
			            m_remark = RemarkPlanManagerInteractiveActionError;
			            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                            
                    }
                }
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				m_remark = RemarkNoPlanManagerForOperatorInput;
			}
		}
		catch ( const CORBA::Exception& e)
		{
			LOG_EXCEPTION_DETAILS( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
			// The object is invalid
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			m_remark = RemarkPlanManagerIsNotRegistered;
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			m_remark = RemarkPlanManagerIsNotRegistered;
        }

        if ( TA_Base_Core::FAILED_ACTIVE_STEP_STATE != stepState )
        {
            m_flowControl.pauseForOperator();
        }

		return( stepState );
	}

} // TA_Base_App


#endif //  STEPTYPE_DECISION
