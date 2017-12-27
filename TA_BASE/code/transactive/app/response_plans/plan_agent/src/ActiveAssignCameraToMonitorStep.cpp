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

#if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )
// namespace alias for Corba structures/types

#include "core/utilities/src/DebugUtil.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveAssignCameraToMonitorStep.h"
#include "app/response_plans/common/src/StringConstants.h"
 
namespace TA_Base_App
{

	ActiveAssignCameraToMonitorStep::ActiveAssignCameraToMonitorStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::AssignCameraToMonitorParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAssignCameraToMonitorStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		try
		{
			std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
			TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationNameFromSession (session);
			TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* monitorObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutput(m_params.monitor);
			
			CORBA_CALL( (*monitorObj), requestSwitchOfInput, ( m_params.camera, session.c_str(), true ) );
			//On successful completion of the step
			m_flowControl.setNextStep();
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Bus::VideoSwitchAgentException& e )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what.in() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", dumpContext().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return stepState;
	}
} // TA_Base_App


#endif //  STEPTYPE_ASSIGN_CAMERA_TO_MONITOR

