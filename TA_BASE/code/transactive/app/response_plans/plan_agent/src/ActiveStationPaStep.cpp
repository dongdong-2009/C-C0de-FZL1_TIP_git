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

#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveStationPaStep.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{

    ActiveStationPaStep::ActiveStationPaStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::StationPublicAddressParameters& params, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl),
    m_params(params)
    {
    }

    TA_Base_Core::EActiveStepState ActiveStationPaStep::executeImpl( void )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        try
        {
            std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
            TA_Base_Bus::PaNamedObj& refPaAgent = 
                TA_Base_Bus::PaAgentAccessFactory::getInstance().getPaAgentByLocationKey( m_params.station.locationKey );

            TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq dvaMessages;
            // One DVA message only
            dvaMessages.length( 1 );
            dvaMessages[0] = m_params.message.messageId;

            TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage bcZones;
            bcZones.length( m_params.zones.length() );
            for ( unsigned int unLoop = 0u; unLoop < m_params.zones.length(); ++unLoop )
            {
                bcZones[unLoop] = m_params.zones[unLoop];
            }

            std::string strBroadcasstId = "";
            if ( m_params.cyclic )
            {
                TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams stCyclic;
                stCyclic.startTime = 0u;
                stCyclic.stopTime = 0u;
                stCyclic.usCyclicTimes = static_cast<unsigned short>(m_params.cyclicTimes);
                CORBA_CALL_RETURN( strBroadcasstId, refPaAgent, broadcastStationScheduleDva, (dvaMessages, bcZones, 0, false, false, "", stCyclic, session.c_str()) );
            }
            else
            {
                CORBA_CALL_RETURN( strBroadcasstId, refPaAgent, broadcastStationDva, (dvaMessages, bcZones, 0, false, false, "", session.c_str()) );
            }
			   /*string broadcastStationDva 
            ( in MessageKeySeq messages, 
              in ZoneCoverage zones, 
              in unsigned short dwellInSecs, 
              in boolean hasChime, 
              in TA_Base_Core::DateTime startTime, 
              in TA_Base_Core::DateTime stopTime, 
              in unsigned short periodInSecs, 
              in boolean isSynchronisedWithTis, 
				  in string groupName,				// zone group selection for broadcast Id
              in string sessionId ) */
           
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
        catch ( TA_Base_Core::OperationModeException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, expWhat._name(), expWhat.reason._ptr );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( CORBA::SystemException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, expWhat._name(), "Unknown" );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, expWhat._name(), expWhat.strWhat.in() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": ";

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        m_remark = ( TA_Base_Core::FAILED_ACTIVE_STEP_STATE == stepState ) ? RemarkStepExecutionFailed : RemarkStepExecutionSucceed;

        return stepState;
    }
} // TA_Base_App

#endif //  STEPTYPE_STATION_PUBLIC_ADDRESS
