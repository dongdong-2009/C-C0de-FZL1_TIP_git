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
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

// namespace alias for Corba structures/types

#if defined ( STEPTYPE_VIDEOWALL )

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "app/cctv/video_wall_agent/IDL/src/IVideoWallAgentCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveVideoWallStep.h"

namespace TA_Base_App
{

	ActiveVideoWallStep::ActiveVideoWallStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::VideoWallParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveVideoWallStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        TA_Base_Core::NamedObject<TA_Base_App::IVideoWallAgentCorbaDef, TA_Base_App::IVideoWallAgentCorbaDef_ptr, TA_Base_App::IVideoWallAgentCorbaDef_var> vidiWallAgent; 
        
		try
		{
            TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity( m_params.wallBoard );
            vidiWallAgent.setCorbaName( name.getAgentName(), name.getAgentName());

            std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());

			CORBA_CALL( vidiWallAgent, activateLayout, ( m_params.layout, 1 ) ); //TODO: get sessionid type fixed for activateLayout()

            //On successful completion of the step
            m_flowControl.setNextStep();
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": " << e.what();

			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": ";

			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return( stepState );
	}
} // TA_Base_App

#endif //  STEPTYPE_VIDEOWALL
