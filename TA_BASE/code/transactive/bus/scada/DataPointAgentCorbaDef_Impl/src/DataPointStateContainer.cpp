/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointStateContainer.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/12/22 12:33:51 $
  * Last modified by:  $Author: dhanshri $
  *
  *	Description
  *
  */

#include <sstream>


#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointStateContainer.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataPointStateContainer::DataPointStateContainer ( TA_Base_Bus::IDataPointAgent * specificDataPointAgent)
	:
	m_purgeExistingUpdatesTimer ( NULL ),
	m_specificDataPointAgent ( NULL ),
	m_threadTerminated(false),
	m_queueSemaphore(0),
	m_bDebugFailover(false),
	m_bOverallQualityStatus(false),
	m_iBadQualityCount(0)
	{
		m_listOfObservedDataPoints.clear();
        m_listOfDataPointDisplayState.clear();
		m_listOfDataPointQualityState.clear();
		// save the pointer to the data point agent for later usage
		m_specificDataPointAgent = specificDataPointAgent;

		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "DebugFailover" ) )
		{
			int bDebugFailover = atoi ( TA_Base_Core::RunParams::getInstance().get( "DebugFailover" ).c_str() );
			m_bDebugFailover = bDebugFailover > 0 ? true : false;
			m_bOverallQualityStatus = true;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "m_bOverallQualityStatus %d, m_bDebugFailover %d", m_bOverallQualityStatus , m_bDebugFailover );
		}
        loadAllDataPoints();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
		start();
	}


	DataPointStateContainer::~DataPointStateContainer()
	{
        terminateAndWait();
		{
            TA_THREADGUARD( m_listOfDataPointQualityStateLock );
			m_listOfDataPointQualityState.clear();
		}

		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed datapoints, without deleting
		// the actual datapoints since they are not created and owned
		// by this class
        {
            TA_Base_Core::ThreadGuard guard( m_observedDataPointListLock );
		    std::map < unsigned long, DataPoint * >::iterator dpItr;
		    for ( dpItr = m_listOfObservedDataPoints.begin();
			      dpItr != m_listOfObservedDataPoints.end();
			      dpItr++ )
		    {
			    try
			    {
				    dpItr->second->deregisterForUpdateDetails ( this );
			    }
			    catch ( ... )
			    {
				    // do nothing as the data point might have been already removed
			    }
		    }
		    m_listOfObservedDataPoints.clear();
        }


        {
            TA_Base_Core::ThreadGuard guard( m_clientListLock );
            //clear the clinet list first
            m_listOfDataPointClients.clear();
        }

        {
            TA_Base_Core::ThreadGuard guard( m_listOfDataPointDisplayStateLock );
            //remove the datapoint observer and display list
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itrds;
            for (itrds = m_listOfDataPointDisplayState.begin(); itrds != m_listOfDataPointDisplayState.end(); itrds++)
            {
                removeDataPoint(itrds->first);
			    delete (itrds->second);
			    itrds->second = NULL;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataPointAgent = NULL;
	}

	void DataPointStateContainer::terminate()
    {
        m_threadTerminated = true;
        m_queueSemaphore.post();
    }

	void DataPointStateContainer::run()
    {
		unsigned int runTimes = 0;
		bool bGot = false;
		TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* update;
		while(false == m_threadTerminated )
        {
            m_queueSemaphore.wait();
			if ( m_threadTerminated )
			{
				break;
			}
			bGot = false;
            {
                TA_Base_Core::ThreadGuard guard( m_updateQueueLock );
				if (false == m_updateQueue.empty())
				{
					update = m_updateQueue.front();
					m_updateQueue.pop();
					bGot = true;
				}
            }
            if (bGot)
			{
				boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> updateDetails(update);
				saveDataPointUpdateDetails (update->entityKey, updateDetails );
			}

			++runTimes;
			if (0 == runTimes % 1000)
			{
				TA_Base_Core::ThreadGuard guard( m_updateQueueLock );
                int nSize = m_updateQueue.size();
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "there are %d items waiting to process", nSize);
			}
        }

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}
    DataPoint * DataPointStateContainer::findExistingDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are searching
		TA_Base_Core::ThreadGuard guard( m_observedDataPointListLock );

		DataPoint * dp = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataPoint * >::iterator itr = m_listOfObservedDataPoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataPoints.end() )
		{
			dp = itr->second;
		}
		// else do nothing

		return dp;
	}

	void DataPointStateContainer::removeDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_Base_Core::ThreadGuard guard( m_observedDataPointListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataPoint * >::iterator itr = m_listOfObservedDataPoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataPoints.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataPoints.erase ( itr );
		}
		// else do nothing
	}

    void DataPointStateContainer::registerForUpdates ( TA_Base_Bus::DataPoint * datapoint )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_Base_Core::ThreadGuard guard( m_observedDataPointListLock );

        unsigned long dpkey = datapoint->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedDataPoints [ dpkey ] = datapoint;

		// register to the DataPoint for callback of latest update details
		datapoint->registerForUpdateDetails ( this );
	}


	void DataPointStateContainer::processDataPointUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
	{
		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		DataPoint * existingDataPoint = 0;
		existingDataPoint = findExistingDataPoint ( pkey );

		// if found one
		if ( 0 != existingDataPoint )
		{
            TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* update = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState;
            try
            {
				boost::shared_ptr< TA_Base_Bus::DataPointDisplayState > dpDisplay = existingDataPoint->getDisplayState();
				
				if ( 0 != dpDisplay.get() )
				{
					convertDpDisplayUpdate(*dpDisplay, *update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataPoint State for DataPoint key %d",
							 pkey);
			}
			if(m_bDebugFailover == true)
			{
				{
					TA_THREADGUARD( m_listOfDataPointQualityStateLock );
					std::map< unsigned long, bool>::iterator itr = m_listOfDataPointQualityState.find(pkey);
					if(itr != m_listOfDataPointQualityState.end())
					{
						if( update->dataPointStatus >= QUALITY_GOOD_NO_SPECIFIC_REASON )
						{
							// Count down the bad Quality entities only for change from Bad quality -> Good quality	
							if( itr->second != true) 
							{
								itr->second = true;
								if(m_iBadQualityCount>0) 
								{	
									m_iBadQualityCount--; 
								}
							}
							//No need to do 'else' part: remains Good quality
						}
						// Count up the bad Quality entities only for change from Good quality ->Bad quality	
						else
						{
							// Good quality -> Bad quality	
							if( itr->second != false) 
							{
								itr->second = false;
								m_iBadQualityCount++; 
							}
							//No need to do 'else' part: remains Bad quality
							m_bOverallQualityStatus &= itr->second;
						}
	
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
							"DatapointUpdate for EK %d, Name %s, Quality %d, OverallQualityStatus %d BadQualityCount %d, totalCount %d", 
							pkey, existingDataPoint->getDataPointName().c_str(), update->dataPointStatus, m_bOverallQualityStatus, 
							m_iBadQualityCount, m_listOfDataPointQualityState.size() );
					}
				}
			}

            //saveDataPointUpdateDetails ( pkey, update );
			{
				TA_Base_Core::ThreadGuard guard( m_updateQueueLock );
				m_updateQueue.push(update);
			}
			m_queueSemaphore.post();
		}
		// else do nothing

	}


    void DataPointStateContainer::saveDataPointUpdateDetails ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> updateDetails )
    {
        {
		    TA_Base_Core::ThreadGuard guard( m_listOfDataPointDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itr;
			itr = m_listOfDataPointDisplayState.find(pkey);
			if(itr != m_listOfDataPointDisplayState.end())
			{
				*(itr->second) = *updateDetails;
			}
        }

        // notify all DataPointDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, DataPointDisplayClient * >::iterator itr;
		for ( itr = m_listOfDataPointClients.begin();
			  itr != m_listOfDataPointClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveDataPointDisplayUpdateDetails(updateDetails);
		}

    }

    void DataPointStateContainer::addDisplayClient(unsigned long clientID, DataPointDisplayClient * displayClient)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfDataPointClients[clientID] = displayClient;
    }

    void DataPointStateContainer::removeDisplayClient(unsigned long clientID)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, DataPointDisplayClient * >::iterator itr;
        itr = m_listOfDataPointClients.find(clientID);
        if (itr != m_listOfDataPointClients.end())
        {
            m_listOfDataPointClients.erase(itr);
        }
    }

    void DataPointStateContainer::loadAllDataPoints()
    {
        FUNCTION_ENTRY("loadAllDataPoints");
        {
		    TA_Base_Core::ThreadGuard guard( m_observedDataPointListLock );
            m_specificDataPointAgent->getAllDataPoints(m_listOfObservedDataPoints);
        }
        
        {
		    TA_Base_Core::ThreadGuard guard( m_listOfDataPointDisplayStateLock );
            if (m_listOfObservedDataPoints.size() > 0)
            {
                std::map< unsigned long, DataPoint * >::iterator itr;
                for (itr = m_listOfObservedDataPoints.begin(); itr != m_listOfObservedDataPoints.end(); ++itr)
                {
                    TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * dpUpdate = 0;
                    dpUpdate = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );
                    
                    try
                    {
						boost::shared_ptr< TA_Base_Bus::DataPointDisplayState > dpDisplay = (*itr).second->getDisplayState();
						
						if ( 0 != dpDisplay.get() )
						{
							convertDpDisplayUpdate(*dpDisplay, *dpUpdate);
						}

                        m_listOfDataPointDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>(itr->first, dpUpdate));
						if(m_bDebugFailover == true)
						{
							TA_THREADGUARD( m_listOfDataPointQualityStateLock );
							m_listOfDataPointQualityState .insert(std::pair<unsigned long, bool>(itr->first, 
																	((dpUpdate->dataPointStatus) >= QUALITY_GOOD_NO_SPECIFIC_REASON ? true:false)) );
							if( (dpUpdate->dataPointStatus) < QUALITY_GOOD_NO_SPECIFIC_REASON) 
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"First quality is %d: EK %u, m_iBadQualityCount = %d", dpUpdate->dataPointStatus, itr->first, m_iBadQualityCount);
								m_iBadQualityCount++;
								m_bOverallQualityStatus = false;
							}
						}
					}
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught unknown exception while trying to retrieve DataPoint State for DataPoint key %d",
							         itr->first);
			        }
                }
			}
			if(m_bDebugFailover == true)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Maps: DataPointQualityState.size() =%d,  DataPointDisplayState.size()=%d", m_listOfDataPointQualityState.size(),  m_listOfDataPointDisplayState.size());
			}
        }

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* DataPointStateContainer::getAllDataPointDisplayState()
    {
        FUNCTION_ENTRY("getAllDataPointDisplayState");
        //todo: need to delete the new sequence or not
		TA_Base_Core::ThreadGuard guard( m_listOfDataPointDisplayStateLock );
        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( m_listOfDataPointDisplayState.size() );

        if (m_listOfDataPointDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfDataPointDisplayState.begin(); itr != m_listOfDataPointDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
