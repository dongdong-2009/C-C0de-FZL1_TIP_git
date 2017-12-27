/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataPointObserver::DataPointObserver ( TA_Base_Bus::IDataPointAgent * specificDataPointAgent, unsigned long observerID )
	:
	m_purgeExistingUpdatesTimer ( NULL ),
	m_observerID ( observerID ),
	m_specificDataPointAgent ( NULL ),
	TA_Base_Core::QueueProcessor< DpObserverUpdate > ()
	{
		m_listOfObservedDataPoints.clear();
		m_listOfUpdateDetails.clear();

		// save the pointer to the data point agent for later usage
		m_specificDataPointAgent = specificDataPointAgent;

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();

		//TD13275
		start();
	}


	DataPointObserver::~DataPointObserver()
	{
		//TD13275
		terminateAndWait();

		if ( NULL != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = NULL;
		}

		// reset the list of observed datapoints, without deleting
		// the actual datapoints since they are not created and owned
		// by this class

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


		// remove existing TA_Base_Bus::DataPointAgentCorbaDef::DataPointUpdate
		std::vector < TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate* >::iterator itr;
		for ( itr = m_listOfUpdateDetails.begin();
		itr != m_listOfUpdateDetails.end();
		itr++ )
		{
			delete (*itr);
			(*itr) = 0;
		}
		m_listOfUpdateDetails.clear();

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataPointAgent = NULL;
	}


	unsigned long DataPointObserver::getObserverID() const
	{
		return m_observerID;
	}


	DataPoint * DataPointObserver::findExistingDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedDataPointListLock );

		DataPoint * dp = NULL;

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


	void DataPointObserver::removeDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataPointListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataPoint * >::iterator itr = m_listOfObservedDataPoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataPoints.end() )
		{
			// tell the DataPoint we are not interest in receiving callback of
			// new update details
			itr->second->deregisterForUpdateDetails ( this );

			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataPoints.erase ( itr );
		}
		// else do nothing
	}


	void DataPointObserver::addDataPointsToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys )
	{
		if (entityKeys.length() > 0)
		{
			boost::shared_ptr<DpObserverUpdate> dpKeys (new DpObserverUpdate);

			// for each of the entity key specified in the key sequence
			for (unsigned int i = 0; i < entityKeys.length(); i++)
			{
				dpKeys->keys.push_back(entityKeys[i]);
			}
			// else do nothing if already in the list

			insert(dpKeys);
		}		
	}

	void DataPointObserver::removeDataPointsFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys )
	{
		// for each of the entity key specified in the key sequence
		for (unsigned int i = 0; i < entityKeys.length(); i++)
		{
			// remove the data point whose pkey matches with pkey specified in the sequence
			removeDataPoint ( entityKeys[i] );
		}
	}

	TA_Base_Bus::DataPointCorbaTypes::DynamicSyncUpdateSequence * DataPointObserver::getUpdates()
	{
        std::vector < TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate* > localList;
		localList.clear();

		{
			// stop multi threads accessing the update details list
			TA_THREADGUARD( m_updateDetailsLock );

			if ( 0 < m_listOfUpdateDetails.size() )
			{
				// clear the update list
                localList.swap( m_listOfUpdateDetails );
			}
			// else do nothing
		}

		TA_Base_Bus::DataPointCorbaTypes::DynamicSyncUpdateSequence * updateSequence = NULL;
		updateSequence = new TA_Base_Bus::DataPointCorbaTypes::DynamicSyncUpdateSequence();
		updateSequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate* >::iterator currentUpdateItr;
		for ( currentUpdateItr = localList.begin();
			  currentUpdateItr != localList.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence. 
			//the updates are shared_ptr so no need to care about cleaning.
			(*updateSequence)[sequenceIndex++] = **currentUpdateItr;

			delete *currentUpdateItr;
			(*currentUpdateItr) = NULL;
		}

        // only log if there is something
        if (updateSequence->length() > 0)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,
                         "Observer ID %d: has %d updates in the returned sequence",
                         m_observerID, updateSequence->length() );
        }

		// restart the timer
		m_purgeExistingUpdatesTimer->startTiming();

		localList.clear();
		return updateSequence;
	}

	void DataPointObserver::processEvent( boost::shared_ptr<DpObserverUpdate> localList )
	{
        std::stringstream logMessage;
        logMessage << "processDataPointTempList : Processing " << localList->keys.size() << " DataPoints";
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

		// for each of the data points in the local list
		unsigned int counter = 0;
		std::vector < unsigned long > ::iterator keyItr;
		for ( keyItr = localList->keys.begin();
			  keyItr != localList->keys.end();
			  keyItr++ )
		{
			unsigned long entityKey = (*keyItr);
			// First find out if this dn is already observed
			TA_Base_Bus::DataPoint * existingDataPoint = 0;
			existingDataPoint = findExistingDataPoint ( entityKey );

			// if found none
			if ( 0 == existingDataPoint )
			{
				// get the data point from the specific agent
				TA_Base_Bus::DataPoint * datapoint = 0;
				datapoint = m_specificDataPointAgent->getDataPoint (entityKey);

				// if found a data point with matched pkey
				if ( 0 != datapoint )
				{
					// peter low: register first then only get updates
					registerForUpdates ( datapoint );
					
					// get its complete internal state			
					try
					{
						boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> dpCompleteInternalState = datapoint->getCompleteDynamicUpdate();

						if ( NULL != dpCompleteInternalState.get() )
						{
							// save the data point complete state to updateDetails
							TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate updateDetail;
							convertDpCompleteUpdate(*dpCompleteInternalState, updateDetail);							
							saveDataPointUpdateDetails ( datapoint->getEntityKey(), updateDetail );
                            counter++;
						}				
						else
						{
							LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "DataPoint::getState returned NULL pointer for pkey %d",
										 entityKey);
						} 
					}
					catch ( ... )
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataPoint State for DataPoint key %d",
									 entityKey);
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "processEvent() : No datapoint for entity: %d",
								 entityKey);
				}
				// else do nothing

				// take a little break after processing 20 data points
				if ( 20 <= counter )
				{
					TA_Base_Core::Thread::sleep  ( 1 );

					counter = 0;
				}
			}
		}

		localList->keys.clear();
	}


	void DataPointObserver::registerForUpdates ( TA_Base_Bus::DataPoint * datapoint )
	{
		//here is no need to check whether the dp is a Existing one. 
		//this checking already done by processEvent()
		unsigned long dpkey = datapoint->getEntityKey();

		{
			// stop multi threads adding/removing data points while we are modifying
			TA_THREADGUARD( m_observedDataPointListLock );
			// save the reference of data points to internal list
			m_listOfObservedDataPoints [ dpkey ] = datapoint;
		}

		// register to the DataPoint for callback of latest update details
		datapoint->registerForUpdateDetails ( this );
	}


	void DataPointObserver::processDataPointUpdate( unsigned long pkey, 
		boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
	{
		//
		// callback from one of the DataPoint supposedly being assigned to this observer
		//
		std::stringstream ss;
		ss << "processDataPointUpdate: Observer ID "  
		   << m_observerID << "--> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );


		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		DataPoint * existingDataPoint = 0;
		existingDataPoint = findExistingDataPoint ( pkey );

		// if found one
		if ( NULL != existingDataPoint )
		{
			//convet it from cpp to corba first.
			TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate dpUpdate;

			convertDynamicSyncUpdate(*updateDetails, dpUpdate);

			saveDataPointUpdateDetails ( pkey, dpUpdate );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


	void DataPointObserver::saveDataPointUpdateDetails( unsigned long pkey, 
		TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& dpUpdate )
	{
		std::stringstream ss;
		ss << "saveDataPointUpdateDetails: Observer ID "  << m_observerID << "--> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// create instance of DataPointUpdate
		TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate* dpUpdateDetail = 
			new TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate;
		// populate the new DataPointUpdate with the info from the DataPoint
		dpUpdateDetail->dpKey		= pkey;
		dpUpdateDetail->syncUpdate	= dpUpdate;
		
		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );

		// save the new DataPointUpdate to the list of current updates
		m_listOfUpdateDetails.push_back ( dpUpdateDetail );

		FUNCTION_EXIT;
	}


	unsigned long DataPointObserver::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}
}
