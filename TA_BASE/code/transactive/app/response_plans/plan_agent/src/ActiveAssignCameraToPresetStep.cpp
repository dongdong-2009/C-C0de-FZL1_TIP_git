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

#if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )
// namespace alias for Corba structures/types

#include "core/utilities/src/DebugUtil.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveAssignCameraToPresetStep.h"
 
namespace TA_Base_App
{

	ActiveAssignCameraToPresetStep::ActiveAssignCameraToPresetStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::ActivateCameraPresetParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveAssignCameraToPresetStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
        bool cameraLocked = false;
        TA_Base_Bus::VideoSwitchAgentFactory::VideoInputNamedObject* videoInputObject = NULL;
		try
		{
            videoInputObject = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoInput( m_params.camera );
			CORBA_CALL( (*videoInputObject), lock, ( session.c_str() ) );
            cameraLocked = true;
        }
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );
		}
        catch ( const TA_Base_Bus::VideoSwitchAgentException& e )
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what() );
        }
		catch( ... )
		{
            m_remark = RemarkCanNotLockCamera;
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", dumpContext().c_str() );
		}

        if (!cameraLocked)
        {
		    return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        cameraLocked = false;
		try
		{
            TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject* cameraObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera(m_params.camera);

            std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
			CORBA_CALL( ((*cameraObj)->getMovementControl()), activatePreset, ( m_params.preset, session.c_str() ) );
            //On successful completion of the step
            delete cameraObj;
            cameraLocked = true;
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
        catch ( const TA_Base_Bus::VideoSwitchAgentException& e )
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		catch( ... )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", dumpContext().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

        if (!cameraLocked)
        {
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        try
        {
            if ( videoInputObject != NULL )
            {
                (*videoInputObject)->unlock(session.c_str());
                delete videoInputObject;

                cameraLocked = false;
            }
        }
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException, could not unlock camera", e.what() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
        catch ( const TA_Base_Bus::VideoSwitchAgentException & e )
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		catch( ... )
		{
            m_remark = RemarkCanNotLockCamera;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", dumpContext().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

        if (cameraLocked)
        {
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        //On successful completion of the step
        m_flowControl.setNextStep();

		return stepState;
	}

} // TA_Base_App

#endif //  STEPTYPE_ACTIVATE_CAMERA_PRESET

