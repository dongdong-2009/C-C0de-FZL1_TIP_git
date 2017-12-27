/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeLocalSynchronizer.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/05/30 13:36:14 $
  * Last modified by:  $Author: dhanshri $
  *
  */
///////////////////////////////////////////////////////////
//  DirectAgentSynchronisationInterface.cpp
//  Implementation of the Class DirectAgentSynchronisationInterface
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4284)
#pragma warning (disable:4786)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataNodeLocalSynchronizer.h"
#include "bus/scada/proxy_library/src/DataPointLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    DataNodeLocalSynchronizer::DataNodeLocalSynchronizer( const std::string& agentName,
                                                                    unsigned long locationKey,
																	DataNodeSyncManager& manager,
																	DataNodeProxyManager& callback)
    :	AgentSynchronisationInterface(agentName, locationKey),
        m_manager(manager),
        m_callback(callback)
    {
    	m_isLocal = true;
    	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating DataPointLocalSynchronizer for %s", agentName.c_str() );
    }


    DataNodeLocalSynchronizer::~DataNodeLocalSynchronizer()
    {
    }

    void DataNodeLocalSynchronizer::processDataNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
    {
        FUNCTION_ENTRY ("processDataNodeUpdate");

        boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate = boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
        dataNodeUpdate->entityKey = pkey;
        dataNodeUpdate->detail = *(updateDetails.get());
        
        //add the updates into queue for proxy manager later processing
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "DataNodeLocalSync %s - received an update [%d] for entity %d",
                m_agentName.c_str(), dataNodeUpdate->detail.updateType, dataNodeUpdate->entityKey);
            
            m_callback.insertUpdateQueue(dataNodeUpdate);
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Catched");
        }
        
        FUNCTION_EXIT;
    }


    void DataNodeLocalSynchronizer::run()
    {
        while ( false == isTerminating() )
        {

			// Wait for signal that we have a request to process
			waitForRequest();

			if ( true == isTerminating() )
			{
				break;
			}
        }

    }
	void DataNodeLocalSynchronizer::processRequest(RequestItem requestItem)
	{
		switch (requestItem.requestType)
		{
		case EmptyQueue:
			// No work to do simply finish
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "EmptyQueue" );
			break;
        
		case Add:
			{
                // first load configuration
                m_callback.onloadDataNodeConfiguration(requestItem.entityKey);
                
                TA_Base_Bus::IEntity* localEntity = m_manager.getLocalEntity(requestItem.entityKey);
                TA_Base_Bus::DataNode* localDataNode = dynamic_cast<TA_Base_Bus::DataNode*>(localEntity);
                TA_ASSERT(localDataNode != NULL, "non-local datanode is added into local datanode synchroniser");

				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNodeLocalSynchronizer AddDataNode %s: [%lu] %s",
				getAgentName().c_str(), requestItem.entityKey,localDataNode->getName().c_str() );
			
				localDataNode->registerForUpdateDetails(this);
				
				try
				{
					boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> dataNodeState = 
                        localDataNode->getCompleteDynamicStates();
                    
                    boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate = 
                        boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
                    
                    dataNodeUpdate->entityKey = requestItem.entityKey;
                    dataNodeUpdate->detail.updateType = DnCompleteSyncState;
                    dataNodeUpdate->detail.completeUpdate = *(dataNodeState.get());
                    
					m_callback.insertUpdateQueue(dataNodeUpdate);

                }
				catch(TA_Base_Core::ScadaProxyException& ex)
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError,ex.what());
				}
			}
			break;
        
		case Remove:
			{
				TA_Base_Bus::IEntity* localEntity = m_manager.getLocalEntity(requestItem.entityKey);
				TA_Base_Bus::DataNode* localDataNode = dynamic_cast<TA_Base_Bus::DataNode*>(localEntity);
				localDataNode->deregisterForUpdateDetails(this);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNodeLocalSynchronizer RemoveDataNode %s: [%lu] %s",
					getAgentName().c_str(), requestItem.entityKey,localDataNode->getName().c_str() );
			}
			break;

		default:
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled Request.requestType: %d in DataNodeLocalSynchronizer::processRequest()",
				requestItem.requestType );
			break;
		}		
	}

}

