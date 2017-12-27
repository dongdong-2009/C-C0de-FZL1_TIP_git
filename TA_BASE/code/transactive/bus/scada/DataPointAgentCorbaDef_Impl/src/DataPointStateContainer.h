/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointStateContainer.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/12/22 12:33:51 $
  * Last modified by:  $Author: dhanshri $
  *
  *	Description
  *
  */

#ifndef DATAPOINT_STATE_CONTAINER_H
#define DATAPOINT_STATE_CONTAINER_H

#include <map>
#include <queue>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/datapoint_library/src/IDataPointObserverNotification.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataPoint;
	class IDataPointAgent;

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class DataPointStateContainer : public TA_Base_Bus::IDataPointObserverNotification,
									public TA_Base_Core::Thread
//									public TA_Base_Core::QueueProcessor< DpObserverUpdate >
	{
	public:


		DataPointStateContainer ( TA_Base_Bus::IDataPointAgent * specificDataPointAgent);
		virtual ~DataPointStateContainer();

		virtual void run();
		virtual void terminate();

		//
		// Operations required by IDataPointObserverNotification
		//


        virtual void processDataPointUpdate ( unsigned long pkey, 
			boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails );

		//
		// operations specific to DataPointStateContainer
		//

	    /**
	      * addDisplayClient
	      * 
	      * added new client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : DataPointDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void addDisplayClient(unsigned long clientID, DataPointDisplayClient * displayClient);

	    /**
	      * removeDisplayClient
	      * 
	      * removed existing client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : DataPointDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void removeDisplayClient(unsigned long clientID);

        /**
	      * >* getAllDataPointDisplayState
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataPointCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        DataPointCorbaTypes::DisplayStateSequence* getAllDataPointDisplayState();
        
	protected:


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

        void saveDataPointUpdateDetails ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> updateDetails );

        void registerForUpdates ( TA_Base_Bus::DataPoint * datapoint );

        void loadAllDataPoints();
        	
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::IDataPointAgent * m_specificDataPointAgent;

        TA_Base_Core::ReEntrantThreadLockable m_observedDataPointListLock;
		std::map < unsigned long, DataPoint * > m_listOfObservedDataPoints;
        TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        std::map < unsigned long, DataPointDisplayClient * > m_listOfDataPointClients;

        TA_Base_Core::ReEntrantThreadLockable m_listOfDataPointDisplayStateLock;
		std::map < unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * > m_listOfDataPointDisplayState;

		volatile bool	m_threadTerminated;
		TA_Base_Core::ReEntrantThreadLockable	m_updateQueueLock;
        std::queue< TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* >	m_updateQueue;
		TA_Base_Core::Semaphore					m_queueSemaphore;

		bool m_bDebugFailover;
		bool m_bOverallQualityStatus;
		int	 m_iBadQualityCount;
		TA_Base_Core::ReEntrantThreadLockable m_listOfDataPointQualityStateLock;
		std::map < unsigned long, bool > m_listOfDataPointQualityState;

		DataPointStateContainer() {};
		DataPointStateContainer ( const DataPointStateContainer & theInstance ) {};
		DataPointStateContainer & operator= ( const DataPointStateContainer & rhs ) {};
	};
}

#endif

