/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.cpp $
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
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataNodeStateContainer::DataNodeStateContainer ( TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent)
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_specificDataNodeAgent ( 0 ),
	m_bDebugFailover(false),
	m_bOverallQualityStatus(false),
	m_iBadQualityCount(0)
	{
		m_listOfObservedDataNodes.clear();
        m_listOfDataNodeDisplayState.clear();
		m_listOfDataNodeQualityState.clear();

		// save the pointer to the data point agent for later usage
		m_specificDataNodeAgent = specificDataNodeAgent;
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "DebugFailover" ) )
		{
			int bDebugFailover = atoi ( TA_Base_Core::RunParams::getInstance().get( "DebugFailover" ).c_str() );
			m_bDebugFailover = bDebugFailover > 0 ? true : false;
			m_bOverallQualityStatus = true;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "m_bOverallQualityStatus %d, m_bDebugFailover %d", m_bOverallQualityStatus, m_bDebugFailover);
		}

        loadAllDataNodes();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataNodeStateContainer::~DataNodeStateContainer()
	{
		{
            TA_THREADGUARD( m_listOfDataNodeQualityStateLock );
			m_listOfDataNodeQualityState.clear();
		}
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed DataNodes, without deleting
		// the actual DataNodes since they are not created and owned
		// by this class
        {
            TA_Base_Core::ThreadGuard guard( m_observedDataNodeListLock );
		    std::map < unsigned long, DataNode * >::iterator dnItr;
		    for ( dnItr = m_listOfObservedDataNodes.begin();
			      dnItr != m_listOfObservedDataNodes.end();
			      dnItr++ )
		    {
			    try
			    {
				    dnItr->second->deregisterForUpdateDetails ( this );
			    }
			    catch ( ... )
			    {
				    // do nothing as the data point might have been already removed
			    }
		    }
		    m_listOfObservedDataNodes.clear();
        }


        {
            TA_Base_Core::ThreadGuard guard( m_clientListLock );
            //clear the clinet list first
            m_listOfDataNodeClients.clear();
        }

        {
            TA_Base_Core::ThreadGuard guard( m_listOfDataNodeDisplayStateLock );
            //remove the DataNode observer and display list
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itrds;
            for (itrds = m_listOfDataNodeDisplayState.begin(); itrds != m_listOfDataNodeDisplayState.end(); itrds++)
            {
                removeDataNode(itrds->first);
			    delete (itrds->second);
			    itrds->second = 0;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataNodeAgent = 0;
	}

    DataNode * DataNodeStateContainer::findExistingDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are searching
		TA_Base_Core::ThreadGuard guard( m_observedDataNodeListLock );

		DataNode * dp = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			dp = itr->second;
		}
		// else do nothing

		return dp;
	}

	void DataNodeStateContainer::removeDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_Base_Core::ThreadGuard guard( m_observedDataNodeListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataNodes.erase ( itr );
		}
		// else do nothing
	}

    void DataNodeStateContainer::registerForUpdates( TA_Base_Bus::DataNode * dataNode )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_Base_Core::ThreadGuard guard( m_observedDataNodeListLock );

        unsigned long dpkey = dataNode->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedDataNodes [ dpkey ] = dataNode;

		// register to the DataNode for callback of latest update details
		dataNode->registerForUpdateDetails ( this );
	}


	void DataNodeStateContainer::processDataNodeUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
	{
		//
		// callback from one of the DataNode supposedly being assigned to this observer
		//


		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		DataNode * existingDataNode = 0;
		existingDataNode = findExistingDataNode ( pkey );

		// if found one
		if ( 0 != existingDataNode )
		{
            TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState update;
            try
            {
				boost::shared_ptr< TA_Base_Bus::DataNodeDisplayState > dpDisplay = existingDataNode->getDisplayState();
				
				if ( 0 != dpDisplay.get() )
				{
					convertDnDisplayUpdate(*dpDisplay, update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataNode State for DataNode key %d",
							 pkey);
			}
			if(m_bDebugFailover == true)
			{
				{
					TA_THREADGUARD( m_listOfDataNodeQualityStateLock );
					std::map< unsigned long, bool>::iterator itr = m_listOfDataNodeQualityState.find(pkey);
					if(itr != m_listOfDataNodeQualityState.end())
					{
						if( update.quality.value >= QUALITY_GOOD_NO_SPECIFIC_REASON )
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
							"DatanodeUpdate for EK %d, Name %s, Quality %d, OverallQualityStatus %d BadQualityCount %d, totalCount %d", 
							pkey, existingDataNode->getName().c_str(), update.quality.value, m_bOverallQualityStatus, 
							m_iBadQualityCount, m_listOfDataNodeQualityState.size() );
					}
				}
			}
            saveDataNodeUpdateDetails ( pkey, update );
		}
		// else do nothing

	}


    void DataNodeStateContainer::saveDataNodeUpdateDetails( unsigned long pkey, 
		const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState & updateDetails )
    {
        {
		    TA_Base_Core::ThreadGuard guard( m_listOfDataNodeDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itr;
			itr = m_listOfDataNodeDisplayState.find(pkey);
			if(itr != m_listOfDataNodeDisplayState.end())
			{
				*(itr->second) = updateDetails;
			}
        }


		// create instance of DataNodeUpdate
        boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> dpUpdate (new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState);

		// populate the new DataNodeUpdate with the info from the DataNode
		*dpUpdate = updateDetails;

        // notify all DataNodeDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, DataNodeDisplayClient * >::iterator itr;
		for ( itr = m_listOfDataNodeClients.begin();
			  itr != m_listOfDataNodeClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveDataNodeDisplayUpdateDetails(dpUpdate);
		}

    }

    void DataNodeStateContainer::addDisplayClient(unsigned long clientID, DataNodeDisplayClient * displayClient)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfDataNodeClients[clientID] = displayClient;
    }

    void DataNodeStateContainer::removeDisplayClient(unsigned long clientID)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, DataNodeDisplayClient * >::iterator itr;
        itr = m_listOfDataNodeClients.find(clientID);
        if (itr != m_listOfDataNodeClients.end())
        {
			//no need delete this pointer, as the DataNodeAgentCorbaDef_Impl will do it.
            m_listOfDataNodeClients.erase(itr);
        }
    }

    void DataNodeStateContainer::loadAllDataNodes()
    {
        FUNCTION_ENTRY("loadAllDataNodes");
        {
		    TA_Base_Core::ThreadGuard guard( m_observedDataNodeListLock );
            m_specificDataNodeAgent->getAllDataNodes(m_listOfObservedDataNodes);
        }
        
        {
		    TA_Base_Core::ThreadGuard guard( m_listOfDataNodeDisplayStateLock );
            if (m_listOfObservedDataNodes.size() > 0)
            {
                std::map< unsigned long, DataNode * >::iterator itr;
                for (itr = m_listOfObservedDataNodes.begin(); itr != m_listOfObservedDataNodes.end(); itr++)
                {
                    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * dnUpdate = 0;
                    dnUpdate = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );

                    try
                    {
						boost::shared_ptr< TA_Base_Bus::DataNodeDisplayState > dnDisplay = (*itr).second->getDisplayState();
						
						if ( 0 != dnDisplay.get() )
						{
							convertDnDisplayUpdate(*dnDisplay, *dnUpdate);
						}
						
                        m_listOfDataNodeDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>(itr->first, dnUpdate));
						if(m_bDebugFailover == true)
						{
							TA_THREADGUARD( m_listOfDataNodeQualityStateLock );
							m_listOfDataNodeQualityState .insert(std::pair<unsigned long, bool>(itr->first, 
																	((dnUpdate->quality.value) >= QUALITY_GOOD_NO_SPECIFIC_REASON ? true:false)) );

							if( (dnUpdate->quality.value) < QUALITY_GOOD_NO_SPECIFIC_REASON) 
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"First quality is %d: EK %u, m_iBadQualityCount = %d", dnUpdate->quality.value, itr->first, m_iBadQualityCount);
								m_iBadQualityCount++;
								m_bOverallQualityStatus = false;
							}

						}
					}
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataNode State for DataNode key %d",
							         itr->first);
			        }
                }
			}
			if(m_bDebugFailover == true)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Maps: DNQualityState.size() =%d,  DNDisplayState.size()=%d", m_listOfDataNodeQualityState.size(),  m_listOfDataNodeDisplayState.size());
			}
		}
        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* DataNodeStateContainer::getAllDataNodeDisplayState()
    {
        FUNCTION_ENTRY("getAllDataNodeDisplayState");
        //todo: need to delete the new sequence or not
		TA_Base_Core::ThreadGuard guard( m_listOfDataNodeDisplayStateLock );
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence * displaySequence = 0;

		displaySequence = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence();
		displaySequence->length (m_listOfDataNodeDisplayState.size());

        if (m_listOfDataNodeDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfDataNodeDisplayState.begin(); itr != m_listOfDataNodeDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
