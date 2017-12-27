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

// namespace alias for Corba structures/types

#if defined ( STEPTYPE_VMS_BLANKOUT )

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "app/signs/vms_agent/IDL/src/SignControlCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveVmsBlankoutStep.h"

namespace TA_Base_App
{

	ActiveVmsBlankoutStep::ActiveVmsBlankoutStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::VmsBlankoutParameters& params, IActivePlanFlow& flowControl ) :
	ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveVmsBlankoutStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = RemarkPlanStepExecuted;
		
		std::string message;

        std::ostringstream msg;

        TA_Base_Core::NamedObject< TA_Base_App::TA_Signs::ISignFactoryCorbaDef, TA_Base_App::TA_Signs::ISignFactoryCorbaDef_ptr, TA_Base_App::TA_Signs::ISignFactoryCorbaDef_var> signFactory; 
        TA_Base_Core::CorbaName name = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity( "VmsAgent");
        signFactory.setCorbaName( name.getAgentName(), "VMS/SignFactory" );

        try
        {
			TA_Base_App::TA_Signs::SignObjectSeq* signObjSeq ;
			CORBA_CALL_RETURN( signObjSeq, signFactory, getAll, () );
            int numberOfSigns = signObjSeq->length();
            bool foundSign = false;

            for ( int i=0; i < numberOfSigns; i++ )
            {
                TA_Base_App::TA_Signs::SignObject so = signObjSeq->operator[](i);
                if ( so.entityKey == m_params.vmsSign )
                {
                    std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
                    so.id->blank(session.c_str());
					foundSign = true;
                    break;
                }
            }

            if ( !foundSign )
            {
                m_remark = "The sign was not found.";
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                return stepState;
            }

            //On successful completion of the step
            m_flowControl.setNextStep();  
            return( stepState );
        }
        catch ( const TA_Base_App::TA_Signs::SignOperationFailed& e)
        {
            m_remark = "Sign operation failed";
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::SignOperationFailed", e.reason );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                                   
        }
        catch ( TA_Base_App::TA_Signs::SignCommunicationError& e)
        {
            m_remark = "Sign communication error";
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::SignCommunicationError", e.reason );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;            
        }
        catch ( TA_Base_App::TA_Signs::PermissionDenied& e)
        {
            m_remark = "Permission denied";
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::PermissionDenied", e.reason );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                        
        }
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			msg << dumpContext() << ": "
			    << e.what();

            m_remark = "The sign is not accessible.";

			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			msg << dumpContext() << ": ";
            m_remark = "Failed to perform sign operation.";

			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception caught", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return( stepState );
	}
} // TA_Base_App

#endif //  STEPTYPE_VMS_BLANKOUT
