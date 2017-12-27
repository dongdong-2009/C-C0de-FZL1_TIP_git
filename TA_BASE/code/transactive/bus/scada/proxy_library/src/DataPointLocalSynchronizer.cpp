/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointLocalSynchronizer.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/30 13:36:14 $
  * Last modified by:  $Author: dhanshri $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointLocalSynchronizer.cpp
//  Implementation of the Class DataPointLocalSynchronizer
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4284 4786 4146 4018 4250 4290 4503)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataPointLocalSynchronizer.h"
#include "bus/scada/proxy_library/src/DataPointLocalAccess.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    DataPointLocalSynchronizer::DataPointLocalSynchronizer( const std::string& agentName,
																	  unsigned long locationKey,
																	  DataPointSyncManager& manager,
																	  DataPointProxyManager& callback )
    :	AgentSynchronisationInterface(agentName, locationKey),
        m_manager(manager),
        m_callback(callback)
    {
    	m_isLocal = true;
    }


    DataPointLocalSynchronizer::~DataPointLocalSynchronizer()
    {
    }


    void DataPointLocalSynchronizer::processDataPointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
    {
        FUNCTION_ENTRY ("processDataPointUpdate");

        //deal with update directly
        boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
        dataPointUpdate->entityKey = pkey;
        dataPointUpdate->detail = *(updateDetails.get());
        
        // add the updates into queue for proxy manager later processing
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "DataPointLocalSync %s - received an update [%d] for entity %d",
                m_agentName.c_str(), dataPointUpdate->detail.updateType, dataPointUpdate->entityKey);
            
            m_callback.insertUpdateQueue(dataPointUpdate);

		
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Caught during processDataPointUpdate");
        }
        
        FUNCTION_EXIT;
    }


    void DataPointLocalSynchronizer::run()
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

	void DataPointLocalSynchronizer::processRequest(RequestItem requestItem)
	{
		switch (requestItem.requestType)
		{
		case EmptyQueue:
			// No work to do simply finish
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "EmptyQueue" );
			break;
			
		case Add :
			{
				// first load configuration
				m_callback.onLoadDataPointConfiguration(requestItem.entityKey);

				// then get dynamic status
				TA_Base_Bus::IEntity* localEntity = m_manager.getLocalEntity(requestItem.entityKey);
				TA_Base_Bus::DataPoint* localDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>(localEntity);
				
				TA_ASSERT(localDataPoint != NULL, "non-local datapoint is added into local datapoint synchroniser");

				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPointLocalSynchronizer AddDataPoint %s:[%lu] %s",
				getAgentName().c_str(), requestItem.entityKey,localDataPoint->getDataPointName().c_str() );
		
		
		
				localDataPoint->registerForUpdateDetails(this);
		
				try
				{
					boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> internalState = 
						localDataPoint->getCompleteDynamicUpdate();
						
					boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = 
						boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
					
					dataPointUpdate->entityKey = requestItem.entityKey;
					dataPointUpdate->detail.updateType = CompleteDynamicSyncStateUpdate;
					dataPointUpdate->detail.syncCompleteState = *(internalState.get());

					m_callback.insertUpdateQueue( dataPointUpdate);
					
					//internal state deletes itself when it falls out of context
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
				TA_Base_Bus::DataPoint* localDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>(localEntity);
				localDataPoint->deregisterForUpdateDetails(this);

				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPointLocalSynchronizer RemoveDataPoint %s: [%lu] %s",
					getAgentName().c_str(), requestItem.entityKey,localDataPoint->getDataPointName().c_str() );
			}
			break;
			
		default:
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unhandled requestType: %d in DataPointLocalSynchronizer::processRequest()",
				requestItem.requestType );
				
			break;
		}

	}
		
};

