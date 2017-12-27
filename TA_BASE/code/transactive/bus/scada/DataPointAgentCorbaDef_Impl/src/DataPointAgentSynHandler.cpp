/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentSynHandler.cpp $
  * @author  Wu Mintao
  * @version $Revision: #4 $
  * Last modification : $DateTime: 2017/09/13 16:40:07 $
  * Last modified by : $Author: wang.wang $
  *
  *	DataPointAgentSynHandler is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */

#include "time.h"

#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentSynHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const unsigned short DataPointAgentSynHandler::BULK_POLL_AGENT_SYN_PERIOD_MSECS = 5000;
    const unsigned short DataPointAgentSynHandler::BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS = 15000; // 15 Sec

	DataPointAgentSynHandler::DataPointAgentSynHandler (std::string& agentname, TA_Base_Bus::IDataPointAgent * specificDataPointAgent)
        :
        m_toBeTerminated(false),
        m_isSynchronizing(false),
        m_agentInterface(NULL),
        m_observerId(0)
        // m_corbaErrorPollSkipCount(0)
    {
        m_agentInterface = new DataPointAgentInterface();
        m_agentInterface->setCorbaName (agentname, agentname + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);
        m_agentInterface->setObjectTimeout(BULK_POLL_AGENT_SYN_PERIOD_MSECS / 1000);
        if (specificDataPointAgent != NULL)
        {
            specificDataPointAgent->getAllDataPoints(m_listOfLocalDataPoints);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"No local datapoints have been obtained from the agent!");
        }
        start();   
	}


	DataPointAgentSynHandler::~DataPointAgentSynHandler()
	{
        // terminate thread first
        terminateAndWait();

        if (m_agentInterface != NULL)
        {
            delete m_agentInterface;
            m_agentInterface = NULL;
        }
	}
    
    void DataPointAgentSynHandler::run()
    {
        while(false == m_toBeTerminated )
        {
            // can only synchronise once we've been started, and if in monitor mode
			if(true == m_isSynchronizing)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Begin process bulkpoll control agent");        
				TA_Base_Bus::DataPointCorbaTypes::DynamicSyncUpdateSequence* dataPointUpdates = NULL;
				try
				{
                    // if (m_corbaErrorPollSkipCount < 1)
                    {
						if (0 == m_observerId)
						{
							CORBA_CALL_RETURN( m_observerId, ( *m_agentInterface ), regiesterDataPointSynState, () );
						}
						if (m_observerId > 0)
						{
							CORBA_CALL_RETURN( dataPointUpdates, ( *m_agentInterface ), getUpdates, ( m_observerId ) );
						}
					}
                    // else
                    // {
				        // LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s skip poll, m_corbaErrorPollSkipCount[%d]",
                            // m_agentInterface->getAgentName().c_str(), m_corbaErrorPollSkipCount);
				        // --m_corbaErrorPollSkipCount;
                    // }
				}
				catch ( const TA_Base_Core::ObjectResolutionException & ex )
				{
					m_observerId = 0;
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
                    // m_corbaErrorPollSkipCount = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS / BULK_POLL_AGENT_SYN_PERIOD_MSECS;
				}
				catch ( const TA_Base_Core::ScadaProxyException & ex )
				{
					m_observerId = 0;
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
				}
				catch ( const TA_Base_Core::BadParameterException & )
				{
					m_observerId = 0;
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
				}
				catch ( const TA_Base_Core::OperationModeException & )
				{
					m_observerId = 0;
                    //clear the named object
                    (*m_agentInterface).clear();
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				}
				catch ( const CORBA::Exception & ex )
				{
					m_observerId = 0;
                    //clear the named object
                    (*m_agentInterface).clear();
					LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				
				}
				catch (...) 
				{
					m_observerId = 0;
					LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
				}
							
				// if got data point updates from the agent
				if (NULL != dataPointUpdates)
				{
					unsigned int updates = 0;
					updates = dataPointUpdates->length();
			
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"Observer ID %lu received %lu data point updates from data point control agent",
						m_observerId, updates);
			
					try
					{
						for (unsigned int i = 0; i < updates; ++ i)
						{
							TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate&  dataPointUpdate = (*dataPointUpdates)[i];
							std::map< unsigned long, DataPoint * >::iterator itr = m_listOfLocalDataPoints.find(dataPointUpdate.dpKey);

							if(itr != m_listOfLocalDataPoints.end())
							{
								TA_Base_Bus::DpDynamicSyncUpdate toupdate;
								TA_Base_Bus::convertDynamicSyncUpdate( dataPointUpdate.syncUpdate , toupdate);
								(*itr).second->updateSynState(toupdate);
								// TODO: this brings too much overhead...
								// lym 2008-8-8
								/*boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
								dpupdate->updateType =  TA_Base_Bus::CompleteDynamicSyncStateUpdate;//toupdate.updateType;								
								boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeState = (*itr).second->getCompleteDynamicUpdate();
								dpupdate->syncCompleteState = *(completeState.get());								
								(*itr).second->notifyRegisteredObservers(dpupdate);*/
							}
			
						}
					}
					catch (...) 
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
					}

					if ( NULL != dataPointUpdates )
					{
						delete dataPointUpdates;
						dataPointUpdates = NULL;
					}
				}
            }

			if(!m_toBeTerminated)
			{
				m_timerCondition.timedWait(BULK_POLL_AGENT_SYN_PERIOD_MSECS);
			}     
        }
    }

    void DataPointAgentSynHandler::terminate()
    {
        m_toBeTerminated = true;
        m_timerCondition.signal();
    }

    void DataPointAgentSynHandler::startSynchronize()
    {
        if ( false == m_isSynchronizing )
        {
            m_isSynchronizing = true;              
            // make it go now
            m_timerCondition.signal();
        }
    }
    
    void DataPointAgentSynHandler::stopSynchronize()
    {
        if ( true == m_isSynchronizing )
        {
            m_isSynchronizing = false;
        }
    }
}
