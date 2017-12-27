/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/CODE_TA_BASE_DDB/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentSynHandler.cpp $
  * @author  Liang Weimin
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2007/01/30 22:26:07 $
  * Last modified by : $Author: scadateam $
  *
  *	DataNodeAgentSynHandler is an abstract base class that implements
  * the DataNodeAgentCorbaDef interface, providing DataNode configurations
  * and updates to remote clients.
  *
  */

#include "time.h"

#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentSynHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const unsigned short DataNodeAgentSynHandler::BULK_POLL_AGENT_SYN_PERIOD_MSECS = 5000;
    const unsigned short DataNodeAgentSynHandler::BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS = 15000; // 15 Sec

	DataNodeAgentSynHandler::DataNodeAgentSynHandler (std::string& agentname, TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent)
        :
        m_toBeTerminated(false),
        m_isSynchronizing(false),
        m_agentInterface(NULL),
        m_observerId(0)
        // m_corbaErrorPollSkipCount(0)
    {
        m_agentInterface = new DataNodeAgentInterface();
        m_agentInterface->setCorbaName (agentname, agentname + TA_Base_Bus::DN_BULK_POLLED_INTERFACE);
        m_agentInterface->setObjectTimeout(BULK_POLL_AGENT_SYN_PERIOD_MSECS / 1000);
        if (specificDataNodeAgent != NULL)
        {
            specificDataNodeAgent->getAllDataNodes(m_listOfLocalDataNodes);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"No local DataNodes have been obtained from the agent!");
        }
        start();   
	}


	DataNodeAgentSynHandler::~DataNodeAgentSynHandler()
	{
        // terminate thread first
        terminateAndWait();

        if (m_agentInterface != NULL)
        {
            delete m_agentInterface;
            m_agentInterface = NULL;
        }
	}
    
    void DataNodeAgentSynHandler::run()
    {
        // initialise the timers

        while(false == m_toBeTerminated )
        {
			if(true == m_isSynchronizing)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Begin process bulkpoll control agent");        
				TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence* dataNodeUpdates = NULL;
				try
				{
                    // if (m_corbaErrorPollSkipCount < 1)
                    {
					    if (0 == m_observerId)
					    {
                            CORBA_CALL_RETURN( m_observerId, (*m_agentInterface), regiesterDataNodeSynState, () );
					    }
					    if (m_observerId > 0)
					    {
							CORBA_CALL_RETURN( dataNodeUpdates, (*m_agentInterface), getUpdates, ( m_observerId ) );//limin
					    }
                    }
                    // else
                    // {
				    //    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s skip poll, m_corbaErrorPollSkipCount[%d]",
                    //        m_agentInterface->getAgentName().c_str(), m_corbaErrorPollSkipCount);
				    //    --m_corbaErrorPollSkipCount;
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
					// clear the named object and retry
					(*m_agentInterface).clear();
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				}
				catch ( const CORBA::Exception & ex )
				{
					m_observerId = 0;
                    // clear the named object and retry
					(*m_agentInterface).clear();
					LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				
				}
				catch (...) 
				{
					m_observerId = 0;
					LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
				}
							
				// if got data point updates from the agent
				if (NULL != dataNodeUpdates)
				{
					unsigned int updates = 0;
					updates = dataNodeUpdates->length();
			
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"Observer ID %lu received %lu data node updates from data node control agent",
						m_observerId, updates);
			
					try
					{
						for (unsigned int i = 0; i < updates; ++ i)
						{
							TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate&  dataNodeUpdate = (*dataNodeUpdates)[i];
							std::map< unsigned long, DataNode * >::iterator itr = m_listOfLocalDataNodes.find(dataNodeUpdate.dnKey);

							if(itr != m_listOfLocalDataNodes.end())
							{
								TA_Base_Bus::DnSyncUpdate toupdate;
								TA_Base_Bus::convertDnDynamicSyncUpdate( dataNodeUpdate.syncUpdate , toupdate);
								(*itr).second->updateSynState(toupdate);
							}
			
						}
					}
					catch (...) 
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
					}

					if ( NULL != dataNodeUpdates )
					{
						delete dataNodeUpdates;
						dataNodeUpdates = NULL;
					}
				}
			}//end of if m_isSynchronizing

			if (!m_toBeTerminated)
			{
				m_timerCondition.timedWait(BULK_POLL_AGENT_SYN_PERIOD_MSECS);
			}
		}
    }

    void DataNodeAgentSynHandler::terminate()
    {
        m_toBeTerminated = true;
        m_timerCondition.signal();
    }

    void DataNodeAgentSynHandler::startSynchronize()
    {
        if ( false == m_isSynchronizing )
        {
            m_isSynchronizing = true;    
            // make it go now
            m_timerCondition.signal();
        }
    }
    
    void DataNodeAgentSynHandler::stopSynchronize()
    {
        if ( true == m_isSynchronizing )
        {
            m_isSynchronizing = false;
        }
    }
}
