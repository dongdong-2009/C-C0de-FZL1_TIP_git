/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Bart Golab
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Represents a Station Traveller Info System step in an executing plan.
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM )

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/ActiveStationTisStep.h"
#include "app/response_plans/common/src/StringConstants.h"


// namespace alias for Corba structures/types

namespace TA_Base_App
{
    ActiveStationTisStep::ActiveStationTisStep(const TA_Base_Core::StepDetail& stepDetail,
        const TA_Base_Core::StationTravellerInfoSystemParameters& params, IActivePlanFlow& flowControl ) :
	ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveStationTisStep::executeImpl( void )
	{
        // TODO: BartG 23/6/05
        //       TD6840 - to be implemented when the STIS subsystem has been extended to meet all the requirements of this step.
        //
        // The following issues have been raised with the Visual team.
        // 1. Sending STIS messages by station level is, apparently, unimplemented. The ISTISManagerCorbaDef 
        //    enables you to target specific station level(s), however, these not seem to be used for anything.
        // 2. The requirement for the STIS plan step is such that the TIS Agent displays whatever it can even 
        //    if a particular PID/level is not applicable to a particular station. In the current implementation, 
        //    the TIS Agent will throw an exception as soon as it encounters an error. It would be better if it 
        //    were to consolidate all errors and return them after completing the request.
        // 3. Similar to #2, if a request is sent to "all" stations (station name = "AST") and one of the stations
        //    has a mismatching library version, the TIS Agent throws an exception straight away. Again, it would 
        //    be preferable if it could do as many stations as possible and then return any errors afterwards.
        //
        // Also, a comment from Adam Radics re message start/end times:
        // "When we give the start and end times to the STIS server they are full YYYYMMDDHHMMSS strings, so potentially 
        //  they could be any time, although if they are in the past I think it whinges." 
        //
        // For the STIS plan step, the start and end times are based on the duration parameter, i.e. start = now and
        // end = start + duration. In light of Adam's comment it might be necessary to increment the start time slightly
        // to avoid it becoming in the past when the message display request reaches the server. In any event, this needs 
        // investigating...

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Station TIS step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = RemarkPlanStepExecuted;

        //wenbo++,GF-158
        //if no station select
        if ( m_params.destinationList.length() < 1 )
        {
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not select station." );
            return stepState;
        }

        //if message is empty
        if ( m_params.messageContent.length() < 1 )
        {
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Message is empty." );
            return stepState;
        }


        //call pidsagent agent to send the message
        short nRet = 0;

        //need a convent
        TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList tmpList;
        tmpList.length( m_params.destinationList.length() );
        for ( unsigned int i = 0; i < m_params.destinationList.length(); i++ )
        {
            tmpList[i].station = m_params.destinationList[i].station;
            tmpList[i].downTrack = m_params.destinationList[i].downTrack;
            tmpList[i].upTrack = m_params.destinationList[i].upTrack;
            tmpList[i].concourse = m_params.destinationList[i].concourse;
        }

        TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage tmpMessage;
        tmpMessage.length( m_params.messageContent.length() );
        for ( unsigned int i = 0; i < m_params.messageContent.length(); i++ )
        {
            tmpMessage[i] = m_params.messageContent[i];
        }

        TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime tmpStartTime;
        {
            tmpStartTime.year = m_params.startTime.year;
            tmpStartTime.month = m_params.startTime.month;
            tmpStartTime.day = m_params.startTime.day;
            tmpStartTime.hour = m_params.startTime.hour;
            tmpStartTime.minute = m_params.startTime.minute;
            tmpStartTime.second = m_params.startTime.second;
        }

        TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime tmpEndTime;
        {
            tmpEndTime.year = m_params.endTime.year;
            tmpEndTime.month = m_params.endTime.month;
            tmpEndTime.day = m_params.endTime.day;
            tmpEndTime.hour = m_params.endTime.hour;
            tmpEndTime.minute = m_params.endTime.minute;
            tmpEndTime.second = m_params.endTime.second;
        }


        //wenbo,GF-158  get pidsagent
        TA_Base_Core::NamedObject < TA_Base_Bus::IPIDSManagerCorbaDef, TA_Base_Bus::IPIDSManagerCorbaDef_ptr,
            TA_Base_Bus::IPIDSManagerCorbaDef_var > pidsAgent;

        const std::string PIDAGENT_TYPE( "PidsAgent" );
        try
        {
            unsigned long myLocationKey = PlanAgentUser::getInstance()->getAgentLocationKey();
            TA_Base_Core::IEntityDataList pidAgentEntities =  TA_Base_Core::EntityAccessFactory::getInstance().
                                                getEntitiesOfTypeAtLocation( PIDAGENT_TYPE, myLocationKey );  // may throw
            TA_ASSERT( pidAgentEntities.size() == 1, "Invalid number of local pid agents" );
            pidsAgent.setEntityName( pidAgentEntities[0]->getName(), false );
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception",
                "Failed to initialize the m_pidsAgent parameter after calling the setEntityName method." );
        }

        try
        {
            CORBA_CALL_RETURN( nRet, pidsAgent, submitAdhocDisplayRequestWithMulityByteMsgContent, ( tmpList, tmpMessage,
                               m_params.msgType, m_params.showType, m_params.isTrain,
                               tmpStartTime, tmpEndTime, m_params.interval, m_params.isCyclic ) );

            if ( nRet != 0 )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Can not connect to FEP." );
            }

            //On successful completion of the step
            m_flowControl.setNextStep();
        }
        catch ( const std::exception& expWhat )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what();
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );

            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;

        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        return stepState;
    }

} // TA_Base_App


#endif //  STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
