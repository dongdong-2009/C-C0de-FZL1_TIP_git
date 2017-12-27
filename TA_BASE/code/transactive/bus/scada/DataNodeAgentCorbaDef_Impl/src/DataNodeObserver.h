/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	Description
  *
  */

#ifndef DATANODE_OBSERVER_H
#define DATANODE_OBSERVER_H

#include <time.h>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataNode;
	class IDataNodeAgent;

	struct DnObserverUpdate
	{
		std::vector <unsigned long> keys;

		bool operator==( DnObserverUpdate &other )
		{
			return (keys == other.keys);
		};

		bool isReplacedBy(DnObserverUpdate* request)
		{
			return false;
		}
	};

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class DataNodeObserver : public TA_Base_Bus::IDataNodeObserverNotification,
							 public TA_Base_Core::QueueProcessor< DnObserverUpdate >
	{
	public:


		DataNodeObserver ( TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent, unsigned long observerID );
		virtual ~DataNodeObserver();


		//
		// Operations required by IDataNodeObserverNotification
		//


		virtual void processDataNodeUpdate(unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails);


		//
		// operations specific to DataNodeObserver
		//


		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */
		unsigned long getObserverID() const;

		/**
		  * addDataNodesToObserver
		  *
		  * This method adds DataNodes to the observer and returns a sequence containing the current
		  * state and configuration of the DataNodes
		  *
		  * @param listOfObservedDataNodes		The list of DataNodes to add to the observer.
		  *
		  */
		void addDataNodesToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );


		/**
          * removeDataNodesFromObserver
          *
          * This method removes DataNodes from the observer
          *
          * @param entityKeys	A sequence of entity keys for DataNodes to remove from the observer.
		  *
          */
		void removeDataNodesFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys );


		/**
          * getUpdates
          *
          * This method returns a sequence of DataNodeUpdates for each item that
		  * has changed since the last time getUpdates was called.
          *
		  *	@return				A sequence containing the updates for DataNodes that have changed.
		  *						A zero length sequence is returned if there are no updates.
		  *
          */
		TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence * getUpdates();


		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those DataNodes assigned to the observer
          *
          */
		unsigned long getIdleTime();

	protected:

		void processEvent( boost::shared_ptr<DnObserverUpdate> localList );

	private:


		/**
          * removeDataNode
          *
          * This method removes DataNodes whose pkey matches with the specified key
		  * from the observer
		  *
		  * @param entityKey	The entity key of the DataNode to remove from the observer.
          *
          */
		void removeDataNode ( unsigned long entityKey );


		/**
          * findExistingDataNode
          *
          * This method finds and returns the reference to the data point whose pkey matches
		  * with the entity key specified in the parameter list
          *
          * @param entityKey	The entity key of the wanted DataNode
		  *
          */
		DataNode * findExistingDataNode ( unsigned long entityKey );

		void saveDataNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate & updateDetails );

		void registerForUpdates ( TA_Base_Bus::DataNode * datanode );

//        void DataNodeObserver::activateDataNodes( void );
            
        DataNodeObserver();
		DataNodeObserver ( const DataNodeObserver & theInstance );
		DataNodeObserver & operator= ( const DataNodeObserver & rhs );

    private:

		unsigned long m_observerID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::IDataNodeAgent * m_specificDataNodeAgent;
        
		std::map < unsigned long, DataNode * > m_listOfObservedDataNodes;
		TA_Base_Core::ReEntrantThreadLockable m_observedDataNodeListLock;

		std::vector < TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * > m_listOfUpdateDetails;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;
	};
}

#endif
