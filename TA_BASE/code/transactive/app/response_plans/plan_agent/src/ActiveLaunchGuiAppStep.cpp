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

#if defined ( STEPTYPE_LAUNCH_GUI_APP )

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveLaunchGuiAppStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveLaunchGuiAppStep::ActiveLaunchGuiAppStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::LaunchGuiAppParameters& params, IActivePlanFlow& flowControl ) : 
    ActiveStep(stepDetail, flowControl, TA_Base_Bus::IPlanManagerCorbaDef::LAUNCHGUIAPP_RESPONSE_TIMEOUT),
    m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveLaunchGuiAppStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prompt step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		m_remark = RemarkWaitingForAppToBeLaunched;

		TA_Base_Core::CurrentPlanStepDetail detail;

		detail.activePlan = m_flowControl.getPlan();
		detail.step = getStepNumber();
        detail.data.launchGuiApp(m_params);

		TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;
		try
		{
			ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);

			if ( planManager != NULL )
			{
				if ( planManager->_non_existent() )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Reference to non existent Plan Manager");
					m_remark = RemarkPlanManagerIsNotRegistered;
					stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
					// The object is invalid
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
                        std::string message = "ActiveLaunchGuiAppStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                        message += isee.reason.in();
			            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );
			            m_remark = RemarkApplicationUnableToLaunch;
			            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                            
                    }
                }
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
				m_remark = RemarkNoPlanManagerForOperatorInput;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
		}
		catch ( const CORBA::Exception& e)
		{
			LOG_EXCEPTION_DETAILS( SourceInfo, "TA_Base_Core::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
			m_remark = RemarkPlanManagerIsNotRegistered;
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered PlanManager is running the interactive plan.");
			m_remark = RemarkPlanManagerIsNotRegistered;
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

        if ( TA_Base_Core::FAILED_ACTIVE_STEP_STATE != stepState )
        {
            m_flowControl.pauseForOperator();
        }

		return( stepState );
	}
} // TA_Base_App


#endif //  STEPTYPE_LAUNCH_GUI_APP
