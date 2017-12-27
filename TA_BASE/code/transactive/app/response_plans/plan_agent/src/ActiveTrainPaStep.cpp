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
  *  <description>
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_TRAIN_PUBLIC_ADDRESS )

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveTrainPaStep.h"

//the following come from 
//CODE_4669/code/transactive/app/pa/PAManager/src/StdAfx.h
const std::string STATION_PA_AGENT_ENTITY_TYPE_NAME = "StationPAAgent";
const std::string MASTER_PA_AGENT_ENTITY_TYPE_NAME = "MasterPAAgent";

namespace TA_Base_App
{

    ActiveTrainPaStep::ActiveTrainPaStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::TrainPublicAddressParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params)
    {
    }

    TA_Base_Core::EActiveStepState ActiveTrainPaStep::executeImpl( void )
    {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Train Public Address step %lu", getStepNumber() );
		 
		 TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
		 
		 try
		 {
			 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			 //6/7/05 Ismar 
			 //as per TD 6840 this step implementation is deferred until a new interface method on the PA Agent
			 //becomes available to accept TrainDestination parameter directly instead of TrainCoverage one.
			 //In this way the PA Agent will be responsible for train list filtering chich makes more sense given 
			 //that the list of trains may change every time step is run and as such is not kept in the STEP_PARAMETERS table
			 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


			 //set up PA Agent
			 /*TA_Base_Core::NamedObject <TA_Base_Bus::IPAAgentCorbaDef, TA_Base_Bus::IPAAgentCorbaDef_ptr, TA_Base_Bus::IPAAgentCorbaDef_var> paAgent; 
			 
			 //use current plan agent location for PA Agent
			 unsigned long locationKey = PlanAgentUser::getAgentLocationKey();
			 std::string sessionId = ActivePlanBulletinBoard::instance()->getSession (m_flowControl.getPlan());

			 std::string entityType;
			 if (locationKey == TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey())
			 {
				 entityType = MASTER_PA_AGENT_ENTITY_TYPE_NAME;
			 }
			 else
			 {
				 entityType = STATION_PA_AGENT_ENTITY_TYPE_NAME;
			 }
			 
			 
			 TA_Base_Core::CorbaNameList paAgentNames(
				 TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(entityType, locationKey));
			 
			 // Make sure there's exactly one Train Agent entity configured for this location.
			 if (paAgentNames.empty())
			 {
				 std::ostringstream msg;
				 msg << "There are no entities of type " << entityType << " configured for OCC.";
				 
				 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
				 
				 throw TA_Base_Core::EntityTypeException (msg.str().c_str (), entityType);
			 }
			 else if (paAgentNames.size() > 1)
			 {
				 std::ostringstream msg;
				 msg << "There are multiple entities of type " << entityType << " configured for OCC.";
				 
				 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
				 
				 throw TA_Base_Core::EntityTypeException(msg.str().c_str (), entityType);
			 }  
			 
			 paAgent.setCorbaName( paAgentNames [0]);
			 

			 TA_Base_Bus::IPATrainCommandsCorbaDef::TimeInterval announcementPeriod;

			 time_t now;
			 struct tm startTime;

		    time( &now );
			 startTime = *localtime( &now );
			 
			 //startTime = time (
			 // Package the low order bytes into the hour, and the high order into the minute
			 announcementPeriod.startHour = startTime.tm_hour;
			 announcementPeriod.startMin  = startTime.tm_min;
			 
			 unsigned long durHrs  = m_params.duration / 60;
			 unsigned long durMins = m_params.duration % 60;

			 announcementPeriod.stopHour  = startTime.tm_hour + durHrs;
			 announcementPeriod.stopMin   = startTime.tm_min + durMins;
			 
			 TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trains;
			 TA_Base_Core::ETrainGroup trainGroup = m_params.destination.trainGroup;
			 
			 //finally broadcast the train message			 
			 std::string bradcastID = paAgent->broadcastTrainDva (trains, m_params.message.messageId,
											  m_params.message.libVersion, announcementPeriod, sessionId.c_str ());*/
			 
			 //On successful completion of the step
			 m_flowControl.setNextStep();
		 }
		 /*catch (const TA_Base_Core::EntityTypeException& e)
		 {
			 std::ostringstream msg;
			 msg << dumpContext() << ": "
				 << e.what();
			 
			 LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::EntityTypeException", msg.str().c_str() );
			 stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		 }*/
        catch( const TA_Base_Core::ObjectResolutionException& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
		  /*catch (const TA_Base_Core::BadParameterException& e)
		  {
			  std::ostringstream msg;
			  msg << dumpContext() << ": "
				  << e.reason;
			  
			  LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::BadParameterException", msg.str().c_str() );
			  stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		  }
		  catch (TA_Base_Core::AccessDeniedException& e)
		  {
			  std::ostringstream msg;
			  msg << dumpContext() << ": "
				  << e.reason;
			  
			  LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_App::AccessDeniedException", msg.str().c_str() );
			  stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		  }*/
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

#endif //  STEPTYPE_TRAIN_PUBLIC_ADDRESS
