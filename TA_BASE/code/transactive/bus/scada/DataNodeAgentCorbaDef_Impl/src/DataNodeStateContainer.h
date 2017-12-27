/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/12/22 12:33:51 $
  * Last modified by:  $Author: dhanshri $
  *
  *	Description
  *
  */

#ifndef DATANODE_STATE_CONTAINER_H
#define DATANODE_STATE_CONTAINER_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.h"



namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataNode;
	class DataNodeDisplayClient;
	class IDataNodeAgent;

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class DataNodeStateContainer : public TA_Base_Bus::IDataNodeObserverNotification
	{
	public:


		DataNodeStateContainer ( TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent);
		virtual ~DataNodeStateContainer();


		//
		// Operations required by IDataNodeObserverNotification
		//


        virtual void processDataNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails );

		//
		// operations specific to DataNodeStateContainer
		//

	    /**
	      * addDisplayClient
	      * 
	      * added new client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : DataNodeDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void addDisplayClient(unsigned long clientID, DataNodeDisplayClient * displayClient);

	    /**
	      * removeDisplayClient
	      * 
	      * removed existing client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : DataNodeDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void removeDisplayClient(unsigned long clientID);

        /**
	      * >* DataNodeDisplayStateSequence
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* getAllDataNodeDisplayState();
        
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

        void saveDataNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState & updateDetails );

        void registerForUpdates ( TA_Base_Bus::DataNode * dataNode );

        void loadAllDataNodes();
        	
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::IDataNodeAgent * m_specificDataNodeAgent;

        TA_Base_Core::ReEntrantThreadLockable m_observedDataNodeListLock;
		std::map < unsigned long, DataNode * > m_listOfObservedDataNodes;
        TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        std::map < unsigned long, DataNodeDisplayClient * > m_listOfDataNodeClients;

        TA_Base_Core::ReEntrantThreadLockable m_listOfDataNodeDisplayStateLock;
		std::map < unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * > m_listOfDataNodeDisplayState;

		bool m_bDebugFailover;
		bool m_bOverallQualityStatus;
		int	 m_iBadQualityCount;
		TA_Base_Core::ReEntrantThreadLockable m_listOfDataNodeQualityStateLock;
		std::map < unsigned long, bool > m_listOfDataNodeQualityState;

		DataNodeStateContainer() {};
		DataNodeStateContainer ( const DataNodeStateContainer & theInstance ) {};
		DataNodeStateContainer & operator= ( const DataNodeStateContainer & rhs ) {};
	};
}

#endif

