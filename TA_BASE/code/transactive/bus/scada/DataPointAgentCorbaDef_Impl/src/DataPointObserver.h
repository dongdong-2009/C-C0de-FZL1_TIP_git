/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	Description
  *
  */

#ifndef DATAPOINT_OBSERVER_H
#define DATAPOINT_OBSERVER_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/datapoint_library/src/IDataPointObserverNotification.h"

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataPoint;
	class IDataPointAgent;

	struct DpObserverUpdate
	{
		std::vector <unsigned long> keys;

		bool operator==( DpObserverUpdate &other )
		{
			return (keys == other.keys);
		};

		bool isReplacedBy(DpObserverUpdate* request)
		{
			return false;
		}
	};

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class DataPointObserver : public TA_Base_Bus::IDataPointObserverNotification,
							  public TA_Base_Core::QueueProcessor< DpObserverUpdate >
	{
	public:

		DataPointObserver ( TA_Base_Bus::IDataPointAgent * specificDataPointAgent, unsigned long observerID );
		virtual ~DataPointObserver();


		//
		// Operations required by IDataPointObserverNotification
		//


		virtual void processDataPointUpdate ( unsigned long pkey, 
			boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails );

		//
		// operations specific to DataPointObserver
		//


		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */	
		unsigned long getObserverID() const;


		/**
		  * addDataPointsToObserver
		  *
		  * This method adds datapoints to the observer and returns a sequence containing the current
		  * state and configuration of the datapoints	
		  *
		  * @param listOfObservedDataPoints		The list of DataPoints to add to the observer.
		  *
		  */	
		void addDataPointsToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );


		/**
          * removeDataPointsFromObserver
          *
          * This method removes datapoints from the observer
          *
          * @param entityKeys	A sequence of entity keys for DataPoints to remove from the observer.
		  *
          */
		void removeDataPointsFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );


		/**
          * getUpdates
          *
          * This method returns a sequence of DataPointUpdates for each item that
		  * has changed since the last time getUpdates was called.
          *
		  *	@return				A sequence containing the updates for DataPoints that have changed.
		  *						A zero length sequence is returned if there are no updates.
		  *
          */
		TA_Base_Bus::DataPointCorbaTypes::DynamicSyncUpdateSequence * getUpdates();


		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those DataPoints assigned to the observer
          *
          */
		unsigned long getIdleTime();

	protected:

		void processEvent( boost::shared_ptr<DpObserverUpdate> localList );

	private:


		/**
          * removeDataPoint
          *
          * This method removes datapoints whose pkey matches with the specified key
		  * from the observer
		  *
		  * @param entityKey	The entity key of the DataPoint to remove from the observer.
          *
          */
		void removeDataPoint ( unsigned long entityKey );


		/**
          * findExistingDataPoint
          *
          * This method finds and returns the reference to the data point whose pkey matches
		  * with the entity key specified in the parameter list
          *
          * @param entityKey	The entity key of the wanted DataPoint
		  *
          */
		DataPoint * findExistingDataPoint ( unsigned long entityKey );

		void saveDataPointUpdateDetails ( unsigned long pkey, 
			TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& dpUpdate );

		void registerForUpdates ( TA_Base_Bus::DataPoint * datapoint );

		unsigned long m_observerID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::IDataPointAgent * m_specificDataPointAgent;

		std::map < unsigned long, DataPoint * > m_listOfObservedDataPoints;
		TA_Base_Core::ReEntrantThreadLockable m_observedDataPointListLock;

		std::vector < TA_Base_Bus::DataPointCorbaTypes::SDynamicSyncUpdate* > m_listOfUpdateDetails;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		DataPointObserver() {};
		DataPointObserver ( const DataPointObserver & theInstance ) {};
		DataPointObserver & operator= ( const DataPointObserver & rhs ) {};
	};
}

#endif

