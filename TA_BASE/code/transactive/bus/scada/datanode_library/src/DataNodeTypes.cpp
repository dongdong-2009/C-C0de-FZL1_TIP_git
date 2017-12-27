#ifdef __WIN32__
#pragma warning(disable:4786 4250)
#endif

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/datanode_library/src/DataNodeTypes.h"

namespace TA_Base_Bus
{
	void DataNodeChildProxies::freeChildProxy()
	{
		freeChildDataPointProxy();
		freeChildDataNodeProxy();
		m_dataNodesHaveValidConfig = false;
		m_dataPointsHaveValidConfig = false;

	}

	void DataNodeChildProxies::freeChildDataPointProxy()
	{
		ChildDataPointMapIt dpIt;
		for(dpIt=m_dataPoints.begin(); dpIt!=m_dataPoints.end(); dpIt++)
		{
			DataPointProxySmartPtr * pDataPointProxySmartPtr = (dpIt->second);
			if ( pDataPointProxySmartPtr!= NULL)
			{
				delete pDataPointProxySmartPtr;
				pDataPointProxySmartPtr = NULL;
			}
		}
		m_dataPoints.clear();
	}

	void DataNodeChildProxies::freeChildDataNodeProxy()
	{
// 		ChildDataNodeVectorIt dnIt;
// 		for( dnIt=m_dataNodes.begin(); dnIt!=m_dataNodes.end();dnIt++)
// 		{
// 			DataNodeProxySmartPtr * pDataNodeProxySmartPtr = *(dnIt);
// 			if ( pDataNodeProxySmartPtr!=NULL )
// 			{
// 				delete pDataNodeProxySmartPtr;
// 			}
// 		}
		ChildDataNodeMapIt dpIt;
		for(dpIt=m_dataNodes.begin(); dpIt!=m_dataNodes.end(); dpIt++)
		{
			DataNodeProxySmartPtr * pDataNodeProxySmartPtr = (dpIt->second);
			if ( pDataNodeProxySmartPtr!= NULL)
			{
				delete pDataNodeProxySmartPtr;
				pDataNodeProxySmartPtr = NULL;
			}
		}

		m_dataNodes.clear();
	}


	bool DataNodeChildProxies::checkConfigValidForChildDataPoints()
	{
		if (false == m_dataPointsHaveValidConfig)
		{
			bool configIsGood = true;
			unsigned long entitykey;
			
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_dataPoints.begin();
			itDataPoint != m_dataPoints.end();
			itDataPoint++)
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;
				
				if (false == (*dp)->isConfigValid() )
				{
					configIsGood = false;
					entitykey = (*dp)->getEntityKey();
					break;
				}
			}
			
			if (false == configIsGood)
			{
				return false;
			}
			
			// got here, so all child dp have valid config
			m_dataPointsHaveValidConfig = true;
			return true;
		}
		return true;
	}

	bool DataNodeChildProxies::checkConfigValidForChildDataNodes()
	{
		if (false == m_dataNodesHaveValidConfig)
		{
			bool configIsGood = true;
			unsigned long entitykey;

			ChildDataNodeMapIt itDataNode;
			for ( itDataNode = m_dataNodes.begin();
			itDataNode != m_dataNodes.end();
			itDataNode++)
			{
				DataNodeProxySmartPtr* dp = (*itDataNode).second;
				
				if (false == (*dp)->isConfigValid() )
				{
					configIsGood = false;
					entitykey = (*dp)->getEntityKey();
					break;
				}
			}
			
			if (false == configIsGood)
			{
				return false;
			}
			
			// got here, so all child dn have valid config
			m_dataNodesHaveValidConfig = true;
			return true;
		}
		return true;
	}

	DataPointProxySmartPtr* DataNodeChildProxies::getChildDataPointProxy(unsigned long entityKey)
	{
		ChildDataPointMapIt it = m_dataPoints.find(entityKey);
		if( it!=m_dataPoints.end())
		{
			return it->second;
		}
		return NULL;
	}

	DataNodeProxySmartPtr* DataNodeChildProxies::getChildDataNodeProxy(unsigned long entityKey)
	{
		ChildDataNodeMapIt it = m_dataNodes.find(entityKey);
		if( it!=m_dataNodes.end())
		{
			return it->second;
		}
		return NULL;
	}

	
	bool DataNodeChildProxies::checkChilidProxiesCreated(TA_Base_Bus::DataNodeStaticConfiguration & staticConfig,
		TA_Base_Bus::ScadaProxyFactory& scadaProxyFactory, IEntityUpdateEventProcessor & entityEventProcessor)
	{
        m_dataNodesHaveValidConfig = false;
        m_dataPointsHaveValidConfig = false;
		
        TA_Base_Core::SharedIEntityDataList::iterator itr;
        

		//xinsong++
		//use established child proxy if recreate child proxy, do not know it is ok
        // Create child DataPoint Proxies
        for ( itr = staticConfig.childDataPointEntities.begin();
		itr != staticConfig.childDataPointEntities.end();
		++itr )
        {
			DataPointProxySmartPtr* dp = NULL;
            try
            {
				unsigned long entityKey = (*itr)->getKey();
                dp = getChildDataPointProxy(entityKey);
				if ( NULL == dp )
				{
					dp = new DataPointProxySmartPtr();
				}
				TA_Base_Core::DataPointEntityDataPtr dpEntityDataPtr = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(*itr);
				scadaProxyFactory.createDataPointProxy(dpEntityDataPtr, entityEventProcessor, *dp );

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataNode [%lu] %s add DataPoint proxy observer %u for [%lu] %s",
					staticConfig.entityKey, 
					staticConfig.name.c_str(),
					&entityEventProcessor,
					entityKey,
					(*dp)->getEntityName().c_str() );
				
                m_dataPoints[entityKey] = dp;
            }
            catch ( const TA_Base_Core::ScadaProxyException & se )
            {
				if (dp != NULL)
				{
					delete dp;
					dp = NULL;
				}
                std::ostringstream desc;
                desc << "Failed to create datapointproxy for "
                    << (*itr)->getAgentName() << "::" << (*itr)->getName()
                    << " Reason: " << se.what();
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", desc.str().c_str() );
            }
            catch (...)
            {
				if(dp != NULL)
				{
					delete dp;
					dp = NULL;
				}
                std::ostringstream desc;
                desc << "Failed to create datapointproxy for "
                    << (*itr)->getAgentName() << "::" << (*itr)->getName();
                LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", desc.str().c_str() );
            }
        }
		
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "createProxies() - %s: %u DataPoint proxies created",
            staticConfig.name.c_str(), m_dataPoints.size());
		
        // Create child DataNode Proxies
        for ( itr = staticConfig.childDataNodeEntities.begin();
		itr != staticConfig.childDataNodeEntities.end();
		++itr )
        {
			DataNodeProxySmartPtr* dn = NULL;
            try
            {
				unsigned long entityKey = (*itr)->getKey();
                dn = getChildDataNodeProxy(entityKey);
				if ( NULL == dn )
				{
					dn = new DataNodeProxySmartPtr();
				}

                scadaProxyFactory.createDataNodeProxy(boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(*itr), entityEventProcessor, *dn );

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataNode [%lu] %s add DataNode proxy observer %u for [%lu] %s",
					staticConfig.entityKey, 
					staticConfig.name.c_str(),
					&entityEventProcessor,
					entityKey,
					(*dn)->getEntityName().c_str() );
				
				m_dataNodes[entityKey] = dn;
            }
            catch ( const TA_Base_Core::ScadaProxyException & se )
            {
				if (dn != NULL)
				{
					delete dn;
					dn = NULL;
				}
                std::ostringstream desc;
                desc << "Failed to create datanodeproxy for "
                    << (*itr)->getAgentName() << "::" << (*itr)->getName()
                    << " Reason: " << se.what();
                LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", desc.str().c_str() );
            }
            catch (...)
            {
				if (dn != NULL)
				{
					delete dn;
					dn = NULL;
				}
                std::ostringstream desc;
                desc << "Failed to create datanodeproxy for "
                    << (*itr)->getAgentName() << "::" << (*itr)->getName();
                LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", desc.str().c_str() );
            }
        }
		
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "createProxies() - %s: %u DataNode proxies created",
            staticConfig.name.c_str(), m_dataNodes.size());	
		
		return true;
	}

}
