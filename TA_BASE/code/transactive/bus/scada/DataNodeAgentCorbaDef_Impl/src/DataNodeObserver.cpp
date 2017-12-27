/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.cpp $
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
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"


using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataNodeObserver::DataNodeObserver ( TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent, unsigned long observerID )
	:
	TA_Base_Core::QueueProcessor< DnObserverUpdate > (),
	m_observerID ( observerID ),
	m_purgeExistingUpdatesTimer ( NULL ),
	m_specificDataNodeAgent( NULL )
	{
		m_listOfObservedDataNodes.clear();
		m_listOfUpdateDetails.clear();

		// save the pointer to the data node agent for later usage
		m_specificDataNodeAgent = specificDataNodeAgent;

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();

		//TD13275
		start();
	}


	DataNodeObserver::~DataNodeObserver()
	{
		//TD13275
		terminateAndWait();
		
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed DataNodes, without deleting
		// the actual DataNodes since they are not created and owned
		// by this class

		std::map < unsigned long, DataNode * >::iterator dpItr;
		for ( dpItr = m_listOfObservedDataNodes.begin();
			  dpItr != m_listOfObservedDataNodes.end();
			  dpItr++ )
		{
			try
			{
				dpItr->second->deregisterForUpdateDetails ( this );
			}
			catch ( ... )
			{
				// do nothing as the data node might have been already removed
			}
		}
		m_listOfObservedDataNodes.clear();

		// remove existing TA_Base_Bus::DataNodeCorbaTypes::DataNodeUpdate
		std::vector < TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * >::iterator itr;
		for ( itr = m_listOfUpdateDetails.begin();
			  itr != m_listOfUpdateDetails.end();
			  itr++ )
		{
			delete (*itr);
			(*itr) = 0;
		}
		m_listOfUpdateDetails.clear();

		// clear the reference to the data node agent, without actually deleting
		// the object
		m_specificDataNodeAgent = 0;
	}


	unsigned long DataNodeObserver::getObserverID() const
	{
		return m_observerID;
	}


	DataNode * DataNodeObserver::findExistingDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data nodes while we are searching
		TA_THREADGUARD( m_observedDataNodeListLock );

		DataNode * dn = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data nodes in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			dn = itr->second;
		}
		// else do nothing

		return dn;
	}


	void DataNodeObserver::removeDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data nodes while we are modifying
		TA_THREADGUARD( m_observedDataNodeListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data nodes in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			// tell the DataNode we are not interest in receiving callback of
			// new update details
			itr->second->deregisterForUpdateDetails ( this );

			// remove the iterator pointing to the data node, without deleting
			// the data node itself since it is not created and managed by this class
			m_listOfObservedDataNodes.erase ( itr );
		}
		// else do nothing
	}


	void DataNodeObserver::addDataNodesToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys )
	{
		if (entityKeys.length() > 0)
		{
			boost::shared_ptr<DnObserverUpdate> dnKeys (new DnObserverUpdate);

			// for each of the entity key specified in the key sequence
			for (unsigned int i = 0; i < entityKeys.length(); i++)
			{
				dnKeys->keys.push_back(entityKeys[i]);
			}
			// else do nothing if already in the list
		
			insert(dnKeys);
		}
	}


	void DataNodeObserver::removeDataNodesFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys )
	{
		// for each of the entity key specified in the key sequence
		for (unsigned int i = 0; i < entityKeys.length(); i++)
		{
			// remove the data node whose pkey matches with pkey specified in the sequence
			removeDataNode ( entityKeys[i] );
		}
	}

	TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence *  DataNodeObserver::getUpdates()
	{
//		activateDataNodes();

		std::vector < TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * > localList;
		localList.clear();

		{
			// stop multi threads accessing the update details list
			TA_THREADGUARD( m_updateDetailsLock );

			if ( 0 < m_listOfUpdateDetails.size() )
			{
				// clear the update list
                localList.swap( m_listOfUpdateDetails );
			}
		}

		TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence * updateSequence = 0;
		updateSequence = new TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence();
		updateSequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * >::iterator currentUpdateItr;
		for ( currentUpdateItr = localList.begin();
			  currentUpdateItr != localList.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence
			(*updateSequence)[sequenceIndex++] = **currentUpdateItr;

			// delete the update details from the list
			delete (*currentUpdateItr);
			(*currentUpdateItr) = 0;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer ID %lu : has %d updates in the returned sequence",
            m_observerID, updateSequence->length() );

		// restart the timer
		m_purgeExistingUpdatesTimer->startTiming();

		localList.clear();

		return updateSequence;
	}

	void DataNodeObserver::processEvent( boost::shared_ptr<DnObserverUpdate> localList )
	{
        std::stringstream debugMessage;
        debugMessage << "processEvent() : Processing " << localList->keys.size() << " DataNode updates";
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, debugMessage.str().c_str() );

		// for each of the data nodes in the local list
		unsigned int counter = 0;
		std::vector < unsigned long > ::iterator keyItr;
		for ( keyItr = localList->keys.begin();
			  keyItr != localList->keys.end();
			  keyItr++ )
		{
			unsigned long entityKey = (*keyItr);

			// First find out if this dn is already observed
			TA_Base_Bus::DataNode * existingDataNode = 0;
			existingDataNode = findExistingDataNode ( entityKey );

			// if found none
			if ( 0 == existingDataNode )
			{
				// get the data node from the specific agent
				TA_Base_Bus::DataNode * datanode = 0;
				datanode = m_specificDataNodeAgent->getDataNode ( entityKey );

				// if found a data node with matched pkey
				if ( 0 != datanode )
				{
					// peter low: register first then only get updates
					registerForUpdates ( datanode );
					
					// get its complete internal state			
					try
					{
						boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> dnCompleteDynamicSyncState = datanode->getCompleteDynamicStates();
						
						// save the data point complete state to updateDetails
						TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate updateDetail;
						convertDnCompleteUpdate(*dnCompleteDynamicSyncState, updateDetail);
						
						saveDataNodeUpdateDetails ( datanode->getEntityKey(), updateDetail );
						
						counter++;
					}
					catch ( ... )
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataNode State for DataNode key %d",
							entityKey);
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "processEvent() : No datanode for entity: %d",
								 entityKey);
				}
				// else do nothing

				// take a little break after processing 20 data nodes
				if ( 20 <= counter )
				{
					TA_Base_Core::Thread::sleep  ( 1 );

					counter = 0;
				}
			}
		}

		localList->keys.clear();
	}


	void DataNodeObserver::registerForUpdates( TA_Base_Bus::DataNode * datanode )
	{
		// stop multi threads adding/removing data nodes while we are modifying
		unsigned long dnkey = datanode->getEntityKey();
		TA_THREADGUARD( m_observedDataNodeListLock );

		// save the reference of data node to internal list
		m_listOfObservedDataNodes [ dnkey ] = datanode;

		// register to the data node for callback of latest update details
		datanode->registerForUpdateDetails ( this );
	}


	void DataNodeObserver::processDataNodeUpdate(unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails)
	{
		//
		// callback from one of the DataNode supposedly being assigned to this observer
		//

		std::stringstream ss;
		ss << "processDataNodeUpdate: Observer ID "  << m_observerID << "--> received an update from data node " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// only proceed if the specified pkey matches with the pkey of the
		// data node assigned to this observer
		DataNode * existingDataNode = 0;
		existingDataNode = findExistingDataNode ( pkey );

		// if found one
		if ( 0 != existingDataNode )
		{
			//convet it from cpp to corba first.
			TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate dnUpdate;
			
			convertDnDynamicSyncUpdate(*updateDetails, dnUpdate);

			saveDataNodeUpdateDetails ( pkey, dnUpdate );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


	void DataNodeObserver::saveDataNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate & updateDetails )
	{
		std::stringstream ss;
		ss << "saveDataNodeUpdateDetails: Observer ID "  << m_observerID << "--> saving updates from data node " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );

		// create instance of DataNodeUpdate
		TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * dnUpdate = 0;
		dnUpdate = new TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate();

		// populate the new DataNodeUpdate with the info from the DataNode
		dnUpdate->dnKey = pkey;
		dnUpdate->syncUpdate = updateDetails;

		// save the new DataNodeUpdate to the list of current updates
		m_listOfUpdateDetails.push_back ( dnUpdate );

		FUNCTION_EXIT;
	}


	unsigned long DataNodeObserver::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}
}
