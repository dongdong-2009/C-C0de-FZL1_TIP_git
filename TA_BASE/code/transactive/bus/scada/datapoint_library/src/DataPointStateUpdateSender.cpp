/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: $
 * @author:  
 * @version: $Revision: $
 *
 * Last modification: $Date: $
 * Last modified by:  $Author: $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/timers/src/SingletonTimerUtil.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/DataPointStateUpdateSender.h"

namespace TA_Base_Bus
{
    
    DataPointStateUpdateSender::DataPointStateUpdateSender(const std::string& agentName, DataPointFactory& dataPointFactory)
        :m_isMonitorAgentAvailabe(false),
		m_namedObjectControlAgent(NULL),
        m_monitorAgentStateUpdateRef(NULL),
        m_myDataPointStateUpdateServantRef(NULL),
		m_agentName(agentName),
		m_operationMode(TA_Base_Core::NotApplicable),
        m_isTerminating(false),
		m_dataPointFactory(dataPointFactory)
    {
        m_uuid = TA_Base_Core::TA_Uuid::generateUUID();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "UUID [%s] created for the DataPointStateUpdateSender.",
            m_uuid.c_str());

        m_namedObjectControlAgent = new DataPointAccessInterface();
        m_namedObjectControlAgent->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DP_ACCESS_INTERFACE);
    }

    DataPointStateUpdateSender::~DataPointStateUpdateSender()
    {
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );

        if (m_namedObjectControlAgent != NULL)
        {
            delete m_namedObjectControlAgent;
            m_namedObjectControlAgent = NULL;
        }
    }
    
    void DataPointStateUpdateSender::threadedTimerExpired(long timerId, void* userData)
    {
        // refresh the registeration if in monitor mode
        if ((m_myDataPointStateUpdateServantRef != NULL) &&
			(TA_Base_Core::Monitor == m_operationMode))
        {
            registerWithControlAgentForStateUpdates();
        }
    }

    void DataPointStateUpdateSender::registerWithControlAgentForStateUpdates()
    {
        TA_ASSERT(m_myDataPointStateUpdateServantRef != NULL, "m_myDataPointStateUpdateServantRef is not set yet.");
        
        if (TA_Base_Core::Monitor != m_operationMode)
        {
            return;
        }

        try
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Registerring monitor mode agent with uuid [%s].",
                m_uuid.c_str());
        
			CORBA_CALL( (*m_namedObjectControlAgent),
			            registerMonitorAgentForStateUpdate,
			            ( m_uuid.c_str(), m_myDataPointStateUpdateServantRef ) );
        }
        //ingore any exception
        catch ( const CORBA::Exception & ex )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
        }
        catch( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown exception" );
        }
    }

    void DataPointStateUpdateSender::sendDataPointStateUpdate(DataPointStateUpdate& dpStateUpdate)
    {
        if (NULL != m_monitorAgentStateUpdateRef)
        {
            TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate dpWriteStateUpdate;
            
            TA_Base_Bus::convertDataPointStateUpdate(dpStateUpdate, dpWriteStateUpdate);

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending datapoint state update to monitor mode agent, commandId [%u].",
                dpWriteStateUpdate.commandId);

            m_monitorAgentStateUpdateRef->sendDataPointStateUpdate(dpWriteStateUpdate);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No monitor mode agent registered, send datapoint state update ingored.");
        }
    }

    void DataPointStateUpdateSender::sendDataPointStateUpdates(const std::vector<DataPointStateUpdate>& dpStateUpdates)
    {
        if (dpStateUpdates.empty())
        {
            return;
        }

        if (NULL != m_monitorAgentStateUpdateRef)
        {
            TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdateSequence dpWriteStateUpdates;
            dpWriteStateUpdates.length(dpStateUpdates.size());

            for (unsigned long i = 0; i < dpStateUpdates.size(); ++ i)
            {
                TA_Base_Bus::convertDataPointStateUpdate(dpStateUpdates[i], dpWriteStateUpdates[i]);
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending datapoint state updates to monitor mode agent, total [%u].",
                dpWriteStateUpdates.length());
            
            m_monitorAgentStateUpdateRef->sendDataPointStateUpdates(dpWriteStateUpdates);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No monitor mode agent registered, send datapoint state update ingored.");
        }
    }

    
    void DataPointStateUpdateSender::setMonitorAgentStateUpdateRef(TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr monitorAgentStateUpdateRef)
    {
        m_monitorAgentStateUpdateRef = TA_Base_Bus::IDataPointStateUpdateCorbaDef::_duplicate(monitorAgentStateUpdateRef);
    }
    
    void DataPointStateUpdateSender::setToControl()
    {
		FUNCTION_ENTRY("setToControl");

        m_operationMode = TA_Base_Core::Control;

        //stop the refresh timer
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );

		FUNCTION_EXIT;
    }

    void DataPointStateUpdateSender::setToMonitor()
    {
		FUNCTION_ENTRY("setToMonitor");

		m_myDataPointStateUpdateServantRef = m_dataPointFactory.getDataPointStateUpdateServant();
        m_operationMode = TA_Base_Core::Monitor;

        // register for the first time
        registerWithControlAgentForStateUpdates();

        // schedule a timer to refresh the registeration every 60 seconds
        TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 60000, false );

		FUNCTION_EXIT;
    }

	void DataPointStateUpdateSender::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		m_operationMode = mode;
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
		if (TA_Base_Core::Monitor == m_operationMode)
		{
			m_myDataPointStateUpdateServantRef = m_dataPointFactory.getDataPointStateUpdateServant();
        	registerWithControlAgentForStateUpdates();

        	TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 60000, false );
		}
	}
	
}
