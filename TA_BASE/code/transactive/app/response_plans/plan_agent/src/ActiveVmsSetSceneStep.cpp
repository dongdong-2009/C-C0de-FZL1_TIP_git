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

#if defined ( STEPTYPE_VMS_SET_SCENE )

#include "core/utilities/src/DebugUtil.h"
#include "app/signs/vms_agent/IDL/src/SignControlCorbaDef.h"
#include "app/signs/vms_agent/IDL/src/SceneCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveAbortStep.h"

namespace TA_Base_App
{

	ActiveVmsSetSceneStep::ActiveVmsSetSceneStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::VmsSetSceneParameters& params, IActivePlanFlow& flowControl ) :
	ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveVmsSetSceneStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

	    m_remark = "Step executed.";
	    
	    std::string message;

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
                    std::ostringstream reason;

                    reason << "Note: The current scene has been set by the Plan["
                           << m_flowControl.getPlan().plan << ":" << m_flowControl.getPlan().instance << "].";

                    std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
                    TA_Base_App::TA_Signs::ISceneOpsCorbaDef_ptr signOps = TA_Base_App::TA_Signs::ISceneOpsCorbaDef::_narrow( so.id->getOpsInterface(TA_Base_App::TA_Signs::OPS_SCENE));
//                        signOps->setCurrentSceneOnSign( session.c_str(),  m_params.scene, reason.str().c_str(), 5);
                    signOps->setCurrentSceneOnSign( session.c_str(),  m_params.scene, 5);
					foundSign = true;
                    break;
                }
            }

            if ( !foundSign )
            {
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                return stepState;
            }

            //On successful completion of the step
            m_flowControl.setNextStep();  
            return( stepState );
        }
	    catch( const TA_Base_Core::ObjectResolutionException& e )
	    {
		    std::ostringstream msg;
		    msg << dumpContext() << ": "
		        << e.what();

			    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
	    }
        catch( CORBA::TRANSIENT& ex)
        {
            //  Most probable cause: the Agent is no longer available (but was OK before)
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::TRANSIENT", std::string( ex.NP_minorString() ) );            
            std::stringstream ss;
            ss << "VmsEnquirer::getSigns::CORBA::TRANSIENT(" << ex.NP_minorString() << ")";
            TA_THROW( TA_Base_Core::TransactiveException( ss.str().c_str() ));
        }
        catch(TA_Base_App::TA_Signs::SignOperationFailed& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "SignOperationFailed", std::string( ex.reason) );
        }

        catch(TA_Base_App::TA_Signs::SignCommunicationError& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "SignCommunicationError", std::string( ex.reason) );
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

#endif //  STEPTYPE_VMS_SET_SCENE
