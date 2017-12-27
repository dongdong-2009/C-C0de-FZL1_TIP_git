/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointProxyManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointProxyManager.cpp
//  Implementation of the Class DataPointProxyManager
//  Created on:      28-Jun-2004 09:39:37 AM
///////////////////////////////////////////////////////////

#include <sys/timeb.h>

#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h"
#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataPointProxyManager::DataPointProxyManager( DataPointSyncManager& syncManager,
	                    						  ObserverUpdateQueueProcessor& observerUpdateQueueProcessor)
        : m_syncManager(syncManager),
          m_observerUpdateQueueProcessor(observerUpdateQueueProcessor)
	{
		for (unsigned long i = 0; i < 100; i++)
		{
            m_divideProxyMap[i] 	= new ProxyMap;
			m_divideEntityMap[i]	= new DpEntityMap;			
		}
	}


    DataPointProxyManager::~DataPointProxyManager()
    {
		for (unsigned long i = 0; i < 100; i++)
		{
			ProxyMap::MapVector proxyVector = m_divideProxyMap[i]->getAll();

			ProxyMap::MapVector::iterator proxyIt;
		    for ( proxyIt = proxyVector.begin(); proxyIt != proxyVector.end(); ++proxyIt )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "dataPoint(entityKey=%d) not deleted.", (*proxyIt)->getEntityKey());
			}

		    m_divideProxyMap[i]->clear();

			delete m_divideProxyMap[i];
			m_divideProxyMap[i] = NULL;
		}

		/*
		ProxyMap::MapVector proxyVector = m_proxyMap.getAll();

		ProxyMap::MapVector::iterator proxyIt;
	    for ( proxyIt = proxyVector.begin(); proxyIt != proxyVector.end(); ++proxyIt )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "dataPoint(entityKey=%d) not deleted.", (*proxyIt)->getEntityKey());
		}

	    m_proxyMap.clear();
		*/
    }


    void DataPointProxyManager::registerDataPointEntityData( TA_Base_Core::IEntityDataPtr EntityData )
    {
        //TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = m_entityMap.get(EntityData->getKey());
        TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = getDataPointEntityData(EntityData->getKey());
        if (sharedDpEntityData.get() == NULL)
        {
            if (EntityData.get() != NULL)
            {
                try
                {
                    sharedDpEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(EntityData);
                    //m_entityMap.insert(EntityData->getKey(), sharedDpEntityData);
                   insertDataPointEntityDataMap(EntityData->getKey(), sharedDpEntityData);
                }
			    catch (...)
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error casting DataPointEntityData %s (%lu)",
                        EntityData->getName().c_str(), EntityData->getKey() );
			    }
            }
        }
    }
	boost::shared_ptr<DataPointProxy> DataPointProxyManager::addProxy (unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData)
	{
		// Check if proxy is already present
		//boost::shared_ptr<DataPointProxy> proxy = m_proxyMap.get(entityKey);
		boost::shared_ptr<DataPointProxy> proxy = getDataPointProxy(entityKey);

		if (NULL != proxy.get())
		{
			return proxy;
		}

        //TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = m_entityMap.get(entityKey);
        TA_Base_Core::DataPointEntityDataPtr sharedDpEntityData = getDataPointEntityData(entityKey);
        
        if (sharedDpEntityData.get() == NULL)
        {
            if (entityData.get() == NULL)
            {
                // load entity information from database
                TA_Base_Core::IEntityData* loadedEntityData = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
        
                TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(loadedEntityData);
        
                if (dpEntityData == NULL)
                {
                    delete loadedEntityData;
					loadedEntityData = NULL;
                    std::ostringstream message;
                    message << "Error: entity [" << entityKey << "] is not a datapoint.";
                    TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
                }

                sharedDpEntityData = TA_Base_Core::DataPointEntityDataPtr(dpEntityData);
            }
            else
            {
                // valid data passed in
                sharedDpEntityData = entityData;
            }

            //m_entityMap.insert(entityKey, sharedDpEntityData);
            insertDataPointEntityDataMap(entityKey, sharedDpEntityData);
        }
        
        unsigned long locationKey = sharedDpEntityData->getLocation();
        if (locationKey == 0)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid location.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        std::string agentName = sharedDpEntityData->getAgentName();
        if (agentName.length() < 1)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid agent key.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
                
		// Add to map
		proxy = boost::shared_ptr<DataPointProxy>(new DataPointProxy(sharedDpEntityData, m_syncManager));

		TA_ASSERT( NULL != proxy.get(), "DataPointProxyManager::DataPointProxyManager() - Failed to create the proxy");
		
        //if more than one thread created more proxy after safetymap::get() function
        //insert function will reserve the first one, discard others and return the first one
		//proxy = m_proxyMap.insert(entityKey, proxy);
		proxy = insertDataPointProxyMap(entityKey, proxy);

		return proxy;
	}


	void DataPointProxyManager::getProxySmartPointer(unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData,
																	   IEntityUpdateEventProcessor& observer,
																	   DataPointProxySmartPtr & dataPointProxySmartPtr)
	{
		boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
		
		//proxy = m_proxyMap.get(entityKey);
		proxy = getDataPointProxy(entityKey);
		
		//[[zhiqiang++ TD_20597
		if (NULL == proxy.get())
		{
			proxy = addProxy(entityKey, entityData);

		    // Ensure synchronisation of this node
            //m_syncManager.add(m_entityMap.get(entityKey));	
            m_syncManager.add(getDataPointEntityData(entityKey));	
			
			dataPointProxySmartPtr.setProxyInfo(proxy, observer, *this);
		}
		else
		{			
			dataPointProxySmartPtr.setProxyInfo(proxy, observer, *this);
			// the data point have be existence
			//proxy.get()->queueNotification(AgentConnected);
			boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
			dataPointUpdate->entityKey = entityKey;
			dataPointUpdate->specialObserver = &observer;
			dataPointUpdate->specialUpdateType = AgentConnected;

			insertUpdateQueue(dataPointUpdate);
		}
		//]]
	}


    void DataPointProxyManager::onCreateProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);

        // if the proxy is created and configured already, notify observer
        DataPointProxy* dataPointProxy = dynamic_cast<DataPointProxy*>(observerNotifier);
        TA_ASSERT(dataPointProxy != NULL, "non datapoint proxy pointer is passed into datapoint proxy manager");

        if (dataPointProxy->isConfigValid())
        {
            //dataPointProxy->queueNotification(ConfigAvailable);
			boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
			dataPointUpdate->entityKey = dataPointProxy->getEntityKey();;
			dataPointUpdate->specialObserver = &observer;
			dataPointUpdate->specialUpdateType = ConfigAvailable;

			insertUpdateQueue(dataPointUpdate);
        }
    }


	void DataPointProxyManager::onCopyProxy( IProxyObserverNotifier* observerNotifier,
                                                 IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);
    }


	void DataPointProxyManager::onDeleteProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        observerNotifier->removeObserver(observer);
        
        // after proxy is created, it will be cached and never be deleted
    }


    /**
     * DataPoint Configuration
     */
    void DataPointProxyManager::onLoadDataPointConfiguration(unsigned long entityKey)
    {
		boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
		
		//proxy = m_proxyMap.get(entityKey);
		proxy = getDataPointProxy(entityKey);

		if( NULL == proxy.get() )
		{
			//if we haven't found it, this just means the proxy has been deleted so the 
			//update is invalid
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onLoadDataPointConfiguration() Proxy for entity - %lu does not exist",
				entityKey );

			return;
		}

		proxy->loadConfiguration();
        //notify config available until get complete update or fail to connect to agent
        //proxy->queueNotification(ConfigAvailable);
    }


    /**
     * DataPoint Agent Interface not available
     */
    void DataPointProxyManager::onDataPointAgentInterfaceNotAvailable (unsigned long entityKey)
    {
        boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
        //proxy = m_proxyMap.get(entityKey);
		proxy = getDataPointProxy(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataPointAgentInterfaceNotAvailable() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }        

		boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::shared_ptr<DpProxyDynamicSyncUpdate>(new DpProxyDynamicSyncUpdate);
        dataPointUpdate->entityKey = entityKey;

		dataPointUpdate->bAgentInterfaceNotAvailable = true;
		//m_observerUpdateQueueProcessor.queueNotification(this, entityKey, dataPointUpdate);
		insertUpdateQueue(dataPointUpdate);
    }


	//TD15939
	void DataPointProxyManager::insertUpdateQueue(boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate)
	{
		//m_observerUpdateQueueProcessor.queueNotification(this, dataPointUpdate->entityKey, dataPointUpdate);
		//Before observerUpdateQueueProcessor start, will processing it directly.
		if (m_observerUpdateQueueProcessor.isProcessing())
		{
			m_observerUpdateQueueProcessor.queueNotification(this, dataPointUpdate->entityKey, dataPointUpdate);
		}
		else
		{
			processUpdateEvent(dataPointUpdate);
		}		
	}
	
	void DataPointProxyManager::processUpdateEvent(boost::shared_ptr<ProxyDynamicSyncUpdate> updateValue)
	{
		boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate = boost::dynamic_pointer_cast<DpProxyDynamicSyncUpdate> (updateValue);
		DpProxyDynamicSyncUpdate* update = dataPointUpdate.get();
	        
		TA_ASSERT( NULL != update, "DpProxyDynamicSyncUpdate is NULL in processUpdateEvent" );

		try
		{
			unsigned long  entityKey = dataPointUpdate->entityKey;
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"processUpdateEvent: entity [%lu] Type[%d], AgentInterfaceNotAvailable[%s],SpecialUpdateType[%d],SpecialObserver[%u]",
				entityKey,
				dataPointUpdate->detail.updateType,
				dataPointUpdate->bAgentInterfaceNotAvailable?"Yes":"No",
				dataPointUpdate->specialUpdateType,
				dataPointUpdate->specialObserver);

            boost::shared_ptr<DataPointProxy> proxy ((DataPointProxy*)NULL);
            //proxy = m_proxyMap.get(entityKey);
			proxy = getDataPointProxy(entityKey);
            
            if( NULL == proxy.get() )
            {
                //if we haven't found it, this just means the proxy has been deleted so the 
                //update is invalid
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "processUpdateEvent() Proxy for entity - %lu does not exist",
                    entityKey );
                
                return;
            }
			if (update->bAgentInterfaceNotAvailable)
			{
				bool isValid = proxy->isConfigValid();
				proxy->updateAgentNotAvailableStatus();
				if (isValid)
				{
					proxy->notifyObservers(QualitySummaryUpdate);
				}
				else
				{
					proxy->notifyObservers(ConfigAvailable);
				}
				return;
			}
			else if (update->specialObserver)
			{
				proxy->notifyObserver(*(update->specialObserver), update->specialUpdateType);
				return;
			}
			else if (NotApplicable != update->specialUpdateType)
			{
				proxy->notifyObservers(update->specialUpdateType);
				return;
			}

            switch (dataPointUpdate->detail.updateType)
			{
            case TA_Base_Bus::CompleteDynamicSyncStateUpdate:
                {
                    bool isAlreadyConfigValid = proxy->isConfigValid();
                    //this function will set config valid to 'true'
                    proxy->updateInternalState(dataPointUpdate->detail.syncCompleteState);
					
                    if (isAlreadyConfigValid)
                    {
						proxy->notifyObservers(AgentConnected); //zhiqiang++TD_20597
                        proxy->notifyObservers(ValueStateUpdate);
                        proxy->notifyObservers(ControlStateUpdate);
                        proxy->notifyObservers(InhibitStateUpdate);
                        proxy->notifyObservers(ConfigStateLastOperatorUpdate);
						// peter low: make sure observer ack state is updated during startup stage
                        proxy->notifyObservers(AckSummaryUpdate);
                    }
                    else
                    {
                        proxy->notifyObservers(ConfigAvailable);
                    }
                    
                    break;
                }
            case TA_Base_Bus::ValueUpdate:
                    proxy->updateValueStatus(dataPointUpdate->detail.value);
                    proxy->notifyObservers(ValueStateUpdate);
                    break;

            case TA_Base_Bus::AlarmStateUpdate:
                    proxy->updateAlarmStatus(dataPointUpdate->detail.alarmState);
					//liuyu++
                    //proxy->queueNotification(ValueStateUpdate);
					proxy->notifyObservers(AlarmSummaryUpdate);
                    break;

            case TA_Base_Bus::AlarmAckStateUpdate:
                    proxy->updateAlarmAckStatus(dataPointUpdate->detail.alarmAckState);
					//liuyu++
                    //proxy->queueNotification(ValueStateUpdate);
					proxy->notifyObservers(AckSummaryUpdate);
                    break;
                
            case TA_Base_Bus::AlarmPropertiesUpdate:
                    proxy->updateAlarmProperties(dataPointUpdate->detail.alarmProperties);
                    proxy->notifyObservers(ValueStateUpdate);
                    break;

            case TA_Base_Bus::ControlUpdate:
                    proxy->updateControlStatus(dataPointUpdate->detail.controlState);
                    proxy->notifyObservers(ControlStateUpdate);
                    break;

            case TA_Base_Bus::InhibitUpdate:
                    proxy->updateInhibitedStatus(dataPointUpdate->detail.inhibitState);
                    proxy->notifyObservers(InhibitStateUpdate);
                    break;

            case TA_Base_Bus::LastOperatorUpdate:
                    proxy->updateLastOperatorInfo(dataPointUpdate->detail.lastOperatorInfo);
                    proxy->notifyObservers(ConfigStateLastOperatorUpdate);
                    break;

            default:
					TA_ASSERT( false, "Proxy manager received an unknown update type" );
					break;

            }
		}
		catch ( const TA_Base_Core::ScadaProxyException & e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException", e.what() );
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown Exception Catched when processing update");
		}

	}

	boost::shared_ptr<DataPointProxy>  DataPointProxyManager::getDataPointProxy(unsigned long entityKey)
	{
		unsigned long entityKeyMod = entityKey % 100;
		std::map < unsigned long, ProxyMap* >::iterator itr;
		itr = m_divideProxyMap.find(entityKeyMod);

		if ( itr != m_divideProxyMap.end())
		{
			return itr->second->get(entityKey);
		}
		return boost::shared_ptr<DataPointProxy>((DataPointProxy*)NULL);
	}

	TA_Base_Core::DataPointEntityDataPtr DataPointProxyManager::getDataPointEntityData(unsigned long entityKey)
	{
		unsigned long entityKeyMod = entityKey % 100;
		std::map < unsigned long, DpEntityMap* >::iterator itr;
		itr = m_divideEntityMap.find(entityKeyMod);

		if ( itr != m_divideEntityMap.end())
		{
			return itr->second->get(entityKey);
		}
		return TA_Base_Core::DataPointEntityDataPtr((TA_Base_Core::DataPointEntityData*)NULL);
	}

	boost::shared_ptr<DataPointProxy> DataPointProxyManager::insertDataPointProxyMap(unsigned long entityKey,boost::shared_ptr<DataPointProxy> proxy)
	{
		unsigned long entityKeyMod = entityKey % 100;
		std::map < unsigned long, ProxyMap* >::iterator itr;
		itr = m_divideProxyMap.find(entityKeyMod);

		if ( itr != m_divideProxyMap.end())
		{
			return itr->second->insert(entityKey,proxy);
		}
		return boost::shared_ptr<DataPointProxy>((DataPointProxy*)NULL);
	}

	TA_Base_Core::DataPointEntityDataPtr DataPointProxyManager::insertDataPointEntityDataMap(unsigned long entityKey,TA_Base_Core::DataPointEntityDataPtr entityData)
	{
		unsigned long entityKeyMod = entityKey % 100;
		std::map < unsigned long, DpEntityMap* >::iterator itr;
		itr = m_divideEntityMap.find(entityKeyMod);

		if ( itr != m_divideEntityMap.end())
		{
			return itr->second->insert(entityKey,entityData);
		}
		return TA_Base_Core::DataPointEntityDataPtr((TA_Base_Core::DataPointEntityData*)NULL);
	}


}
