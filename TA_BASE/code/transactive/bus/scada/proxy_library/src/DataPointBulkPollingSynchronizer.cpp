/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointBulkPollingSynchronizer.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2017/09/13 16:40:07 $
  * Last modified by:  $Author: wang.wang $
  *
  * Implementation of the BulkPolled Synchronisation Interface
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "time.h"
#include "boost/cast.hpp"

#include "bus/scada/proxy_library/src/DataPointBulkPollingSynchronizer.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/ScadaProxyException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataPointBulkPollingSynchronizer::DataPointBulkPollingSynchronizer ( 
                                                                        const std::string& agentName,
                                                                        unsigned long locationKey,
                                                                        DataPointSyncManager& manager,
                                                                        DataPointProxyManager& callback)
          :	AgentSynchronisationInterface(agentName, locationKey),
          m_manager(manager),
          m_callback(callback),
          m_observerId( 0 ),
          // m_corbaErrorPollSkipCount( 0 ),
          m_agentInterface( NULL ),
          m_pollPeriod(500),
          m_timeLastConnected(time(NULL))
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating DataPointBulkPollingSynchronizer for %s", agentName.c_str() );
        
        // check if polling period is set as a runtime parameter
        if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "BulkPeriod" ) )
        {
            // get the polling period from command line
            m_pollPeriod = atoi ( TA_Base_Core::RunParams::getInstance().get ( "BulkPeriod" ).c_str() );

            // if polling period is not defined
            if ( 0 >= m_pollPeriod )
            {
                // default to this msecs
                m_pollPeriod = 500;
            }
        }
        
        //
        // setup corba name of the interface
        //

        m_agentInterface = new DataPointAgentInterface();
        m_agentInterface->setCorbaName ( m_agentName, m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE );
        m_agentInterface->setObjectTimeout ( BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS ); // in secs

        //
        // initialise the lists
        //

        m_addList.clear();
        m_removeList.clear();
    }


    DataPointBulkPollingSynchronizer::~DataPointBulkPollingSynchronizer()
    {
        // Assert if the background thread is still running
        TA_ASSERT( getCurrentState() != THREAD_STATE_RUNNING, "Thread still running");

        //TA_Base_Core::SingletonTimerUtil::removeInstance();

        delete m_agentInterface;
        m_agentInterface = NULL;
    }

    
    void DataPointBulkPollingSynchronizer::terminate()
    {
        AgentSynchronisationInterface::terminate();
        m_timeCondition.signal();
    }


    /**
     * Define a run() method to do your work. This should block until terminate() is
     * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
     * call this method directly. It needs to be public so that it can be called from
     * the thread-spawning function
     */
    void DataPointBulkPollingSynchronizer::run()
    {
        while (!isTerminating())
        {
            try
            {
                RequestItem currentRequest = getNextRequest();

                while ( !isTerminating() && currentRequest.requestType != EmptyQueue )
                {   
                    switch (currentRequest.requestType)
                    {
                        case Add:
                            addEntity ( currentRequest );
                            break;
                        
                        case Remove:
                            removeEntity ( currentRequest );
                            break;

                        default:
                            // do nothing
                            break;
                    }
					
					if(isTerminating())
					{
						break;
					}
					else
					{
                    	currentRequest = getNextRequest();
					}
                }

                if ( !isTerminating() )
                {
                    // if synchronisation is enabled
                    if ( true == isSynchroniseEnabled() )
                    {
                        // get data from the agent
                        queryAgentInterface();
                    }
				}

				if(!isTerminating())
				{
                    m_timeCondition.timedWait(m_pollPeriod);
                }
            }
            catch( const TA_Base_Core::TransactiveException & te)
            {
                std::ostringstream desc;
                desc << "[DataPointBulkPollingSynchronizer::run] " << te.what();
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "[DataPointBulkPollingSynchronizer::run]" );
            }
        }

	
    }


    void DataPointBulkPollingSynchronizer::queryAgentInterface()
    {

        LOG_GENERIC(SourceInfo, DebugUtil::DebugPeriodic, "%s start poll, %d new items", m_agentName.c_str(), m_addList.size());

        try
        {
            // if (m_corbaErrorPollSkipCount < 1)
            {
                // check if we need to obtain the observer ID from the agent
                if ( 0 == m_observerId )
                {
                    initialiseObserver();
                }

                if ( 0 < m_observerId )
                {
                    // add and remove datapoints to the agent interface
                    processRemoveList();
                    processAddList();
                
                    // query for updates
                    queryAgentForUpdates();

				    //TD15939
				    m_isConnected = true;
                    m_timeLastConnected = time(NULL);
                }

                clearAddRemoveLists();
            }
            // else
            // {
                // LOG_GENERIC(SourceInfo, DebugUtil::DebugPeriodic, "%s skip poll, m_corbaErrorPollSkipCount[%d], %d new items", m_agentName.c_str(), m_corbaErrorPollSkipCount, m_addList.size());
                
                // clearAddRemoveLists();
                
                // --m_corbaErrorPollSkipCount;
            // }
        }
        catch ( const TA_Base_Core::ObjectResolutionException & ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );

            setToBadNotConnectedQuality ( OBJECT_RESOLUTION_EXCEPTION );
        }
        catch ( const TA_Base_Core::ScadaProxyException & ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );

            setToBadNotConnectedQuality ( SCADA_PROXY_EXCEPTION );
        }
        catch ( const TA_Base_Core::BadParameterException & )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
            // set all data point to bad quality status and restart the polling cycle
            // by requesting the agent for new observer ID
            setToBadNotConnectedQuality ( BAD_PARAMETER_EXCEPTION );
        }
        catch ( const TA_Base_Core::OperationModeException & )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
            
            // force naming service to resolve object again.
            (*m_agentInterface).clear();
            // set all data point to bad quality status and restart the polling cycle
            // by requesting the agent for new observer ID
            setToBadNotConnectedQuality ( OPERATION_MODE_EXCEPTION );
        }
        catch ( const CORBA::Exception & ex )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
            // force naming service to resolve object again.
            (*m_agentInterface).clear();

            setToBadNotConnectedQuality ( CORBA_EXCEPTION );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");

            setToBadNotConnectedQuality ( UNKNOWN_EXCEPTION );
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "%s wait for next poll", m_agentName.c_str());
    }


    void DataPointBulkPollingSynchronizer::initialiseObserver()
    {
        // request for new observer ID
		CORBA_CALL_RETURN( m_observerId,
		                   (*m_agentInterface),
		                   createObserver,
		                   () );
        // if valid ID is obtained from the agent interface
        if ( 0 < m_observerId )
        {
            //
            // since we have requested a new observer, there is a need to extract
            // data points currently in the synchronisation list of the callback
            // interface and then resubmit them to the agent for online updates
            //

            std::vector<unsigned long> completeAddList;

            // for each of the data points in the list
            for (std::set<unsigned long>::iterator itr = m_syncEntities.begin(); itr != m_syncEntities.end(); ++itr )
            {
                // put it in the add list
                completeAddList.push_back(*itr);
            }

            // now process the add list
            processAddList( completeAddList );
        }
    }


    void DataPointBulkPollingSynchronizer::processRemoveList()
    // throws. All exceptions caught by the calling function
    {
        if ( 0 < m_removeList.size() )
        {
            ScadaCorbaTypes::KeySequence keySequence;
            keySequence.length(m_removeList.size());

            for (unsigned long i = 0; i < keySequence.length(); ++ i)
            {
                keySequence[i] = m_removeList[i];
            }
            // request the agent to remove the data points from the observer
            CORBA_CALL( (*m_agentInterface),
                        removeFromObserver,
                        ( m_observerId, keySequence ) );
	        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                 "DataPointBulkPoll %s - Observer ID %d removed %d data points",
                 m_agentName.c_str(), m_observerId, keySequence.length());
        }
    }


    void DataPointBulkPollingSynchronizer::processAddList()
    {
        processAddList( m_addList );
    }


    void DataPointBulkPollingSynchronizer::processAddList( std::vector<unsigned long>& addList )
    {
        if ( 0 < addList.size() )
        {
            ScadaCorbaTypes::KeySequence keySequence;
            keySequence.length(addList.size());

            for (unsigned long i = 0; i < keySequence.length(); ++ i)
            {
                keySequence[i] = addList[i];
            }

			// request the agent to add the data points to the observer
			CORBA_CALL( (*m_agentInterface),
			            addToObserver,
			            ( m_observerId, keySequence ) );
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "Observer ID %u added %d data points to data point agent %s",
                m_observerId, keySequence.length(), m_agentName.c_str() );
        }
    }

    void DataPointBulkPollingSynchronizer::clearAddRemoveLists()
    {
        // reset the add list
        m_addList.clear();

        // reset the removal list
        m_removeList.clear();     
    }

    void DataPointBulkPollingSynchronizer::queryAgentForUpdates()
    // throws. All exceptions caught by the calling function
    {
        DataPointCorbaTypes::DynamicSyncUpdateSequence* dataPointUpdates = NULL;
		CORBA_CALL_RETURN( dataPointUpdates,
		                   (*m_agentInterface),
		                   getUpdates,
		                   ( m_observerId ) );
        if (NULL == dataPointUpdates)
        {
            //get nothing, do nothing
            return;
        }

        // if got data point updates from the agent
        unsigned int updates = dataPointUpdates->length();

        if (updates == 0)
        {
            // no updates, do nothing
            delete dataPointUpdates;
            dataPointUpdates = NULL;
            return;
        }

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Observer ID %u received %u data point updates from data point agent %s",
            m_observerId, updates, m_agentName.c_str() );

        if ( NULL != dataPointUpdates )
        {
            // Send to proxy
            for ( unsigned int iPoint = 0; iPoint < updates; ++iPoint )
            {
				try
                {
                    DataPointCorbaTypes::SDynamicSyncUpdate&  dataPointUpdate = (*dataPointUpdates)[iPoint];
                    
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPoll %s - received an update [%d] for entity %d",
						m_agentName.c_str(), dataPointUpdate.syncUpdate._d(), dataPointUpdate.dpKey);
					
					boost::shared_ptr<DpProxyDynamicSyncUpdate> dpProxyUpdate = 
                        boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate());
                    
                    dpProxyUpdate->entityKey = dataPointUpdate.dpKey;

                    convertDynamicSyncUpdate(dataPointUpdate.syncUpdate, dpProxyUpdate->detail);

                    m_callback.insertUpdateQueue(dpProxyUpdate);
                }
				catch ( ... )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Caught when processing updates");
				}
            }

            delete dataPointUpdates;
            dataPointUpdates = NULL;
        }
    }


    void DataPointBulkPollingSynchronizer::resetObserver()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Reset Observer id %u for agent %s",
            m_observerId, m_agentName.c_str() );

        // set the observer ID to 0
        m_observerId = 0;

        clearAddRemoveLists();
    }


    void DataPointBulkPollingSynchronizer::addEntity ( const RequestItem & requestItem )
    {
        std::set<unsigned long>::iterator itr = m_syncEntities.find(requestItem.entityKey);
        if (itr == m_syncEntities.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, " AddDataPoint %s:%u",
                getAgentName().c_str(), requestItem.entityKey );
            
            // Put in add list
            m_addList.push_back(requestItem.entityKey);
            m_syncEntities.insert(requestItem.entityKey);

            //load config info from database
            m_callback.onLoadDataPointConfiguration(requestItem.entityKey);
            
            if (!m_isConnected)
            {
                m_callback.onDataPointAgentInterfaceNotAvailable (requestItem.entityKey);
            }
        }
    }


    void DataPointBulkPollingSynchronizer::removeEntity ( const RequestItem & requestItem )
    {
        // Add to remove list
        m_removeList.push_back(requestItem.entityKey);

        std::set<unsigned long>::iterator itr = m_syncEntities.find(requestItem.entityKey);
        if (itr != m_syncEntities.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RemoveDataPoint %s:%u",
                getAgentName().c_str(), requestItem.entityKey );
            
            m_syncEntities.erase(itr);
        }
    }
    
    
    void DataPointBulkPollingSynchronizer::setToBadNotConnectedQuality ( TA_Base_Bus::ExceptionType exception )
    {
        resetObserver();
		// bool isNotifyImmediated = !(TA_Base_Core::RunParams::getInstance().isSet ("DisableNotifyResolutionExImmediate"));
		// bool isNotifyBadQuality = (isNotifyImmediated || 
			// (!isNotifyImmediated && (time(NULL) > (m_timeLastConnected + m_qualityBadPeriod))));

        switch ( exception )
        {
            // only proceed if one of the following exceptions has occurred
            case OBJECT_RESOLUTION_EXCEPTION:
				// notify callback when can't resolve agent interface immediately
				// if (m_isConnected && isNotifyBadQuality)
				{
					m_isConnected = false;
                    // for each synchronized entity, set proxy to agent not avaiable
                    for (std::set<unsigned long>::iterator it = m_syncEntities.begin();
                    it != m_syncEntities.end(); ++ it)
                    {
                        m_callback.onDataPointAgentInterfaceNotAvailable (*it);
                    }

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                        "Have notified DataPointProxyManager that BulkInterface of data point agent %s is not available.",
                        m_agentName.c_str() );
                }

                // m_corbaErrorPollSkipCount = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_MSECS / m_pollPeriod;

                break;

            case CORBA_EXCEPTION:
                
            // in this case, the agent bulk interface has indicated that it does not
            // have the observer ID currently used by this object
            case BAD_PARAMETER_EXCEPTION:
                
            // following exceptions indicate that we are able to contact the agent
            // bulk interface, but have exceptions because of operational data
            case UNKNOWN_EXCEPTION:
            case OPERATION_MODE_EXCEPTION:
            case SCADA_PROXY_EXCEPTION:
            default:
                // notify callback after has retried specified times
                if (m_isConnected && time(NULL) > (m_timeLastConnected + m_qualityBadPeriod))
                {
					m_isConnected = false;
                    // for each synchronized entity, set proxy to agent not avaiable
                    for (std::set<unsigned long>::iterator it = m_syncEntities.begin();
                    it != m_syncEntities.end(); ++ it)
                    {
                        m_callback.onDataPointAgentInterfaceNotAvailable (*it);
                    }
                    
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                        "Have notified DataPointProxyManager that BulkInterface of data point agent %s is not available.",
                        m_agentName.c_str() );
                }

                break;
        }
    }

}
