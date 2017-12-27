/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxyManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeProxyManager.cpp
//  Implementation of the Class DataNodeProxyManager
//  Created on:      28-Jun-2004 09:39:37 AM
///////////////////////////////////////////////////////////


#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/naming/src/NamedObject.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataNodeProxyManager::DataNodeProxyManager(DataNodeSyncManager& syncManager,
	                   						   ObserverUpdateQueueProcessor& observerUpdateQueueProcessor)
        : m_syncManager(syncManager),
          m_observerUpdateQueueProcessor(observerUpdateQueueProcessor)
	{
	}


	DataNodeProxyManager::~DataNodeProxyManager()
	{
		ProxyMap::MapVector proxyVector = m_proxyMap.getAll();

		ProxyMap::MapVector::iterator proxyIt;
	    for ( proxyIt = proxyVector.begin(); proxyIt != proxyVector.end(); ++proxyIt )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "dataNode(entityKey=%d) not deleted.", (*proxyIt)->getEntityKey());
		}

		m_proxyMap.clear();
	}

    
    void DataNodeProxyManager::registerDataNodeEntityData( TA_Base_Core::IEntityDataPtr EntityData )
    {
        TA_Base_Core::DataNodeEntityDataPtr sharedDnEntityData = m_entityMap.get(EntityData->getKey());
        if (sharedDnEntityData.get() == NULL)
        {
            if (EntityData.get() != NULL)
            {
                try
                {
                    sharedDnEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData);
                    m_entityMap.insert(EntityData->getKey(), sharedDnEntityData);
                }
			    catch (...)
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error casting DataNodeEntityData %s (%lu)",
                        EntityData->getName().c_str(), EntityData->getKey() );
			    }
            }
        }
    }

	boost::shared_ptr<DataNodeProxy> DataNodeProxyManager::addProxy (unsigned long entityKey, TA_Base_Core::DataNodeEntityDataPtr entityData)
	{
		// Check if proxy is already present
		boost::shared_ptr<DataNodeProxy> proxy = m_proxyMap.get(entityKey);

		if (NULL != proxy.get())
		{
			return proxy;
		}

        TA_Base_Core::DataNodeEntityDataPtr sharedDnEntityData = m_entityMap.get(entityKey);
        
        if (sharedDnEntityData.get() == NULL)
        {
            if (entityData.get() == NULL)
            {
                // load entity information from database
                TA_Base_Core::IEntityData* loadedEntityData = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
        
                TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(loadedEntityData);
        
                if (dnEntityData == NULL)
                {
                    delete loadedEntityData;
					loadedEntityData = NULL;
                    std::ostringstream message;
                    message << "Error: entity [" << entityKey << "] is not a datanode.";
                    TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
                }

                sharedDnEntityData = TA_Base_Core::DataNodeEntityDataPtr(dnEntityData);
            }
            else
            {
                // valid data passed in
                sharedDnEntityData = entityData;
            }

            m_entityMap.insert(entityKey, sharedDnEntityData);
        }
        
        unsigned long locationKey = sharedDnEntityData->getLocation();
        if (locationKey == 0)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid location.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        std::string agentName = sharedDnEntityData->getAgentName();
        if (agentName.length() < 1)
        {
            std::ostringstream message;
            message << "Error: entity" << entityKey << "] does not have a valid agent key.";
            TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
        }
        
        // Add to map
		proxy = boost::shared_ptr<DataNodeProxy>(new DataNodeProxy(sharedDnEntityData, m_syncManager));

		TA_ASSERT( NULL != proxy.get(), "DataNodeProxyManager::DataNodeProxyManager() - Failed to create the proxy");

        //if more than one thread created more proxy after safetymap::get() function
        //insert function will reserve the first one, discard others and return the first one
        proxy = m_proxyMap.insert(entityKey, proxy);

		return proxy;
	}


	void DataNodeProxyManager::getProxySmartPointer(unsigned long entityKey, boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData,
													DataNodeProxySmartPtr & dataNodeProxySmartPtr, IEntityUpdateEventProcessor& observer)
	{
		boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
		
		proxy = m_proxyMap.get(entityKey);

		if (NULL == proxy.get())
		{
			proxy = addProxy(entityKey, entityData);

		    // Ensure synchronisation of this node
            m_syncManager.add(m_entityMap.get(entityKey));
		}

		TA_ASSERT(NULL != proxy.get(),"DataNode proxy not found.");

        dataNodeProxySmartPtr.setProxyInfo(proxy, observer, *this);
	}


    void DataNodeProxyManager::onCreateProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);

        // if the proxy is created and configured already, notify observer
        DataNodeProxy* dataNodeProxy = dynamic_cast<DataNodeProxy*>(observerNotifier);
        TA_ASSERT(dataNodeProxy != NULL, "non datanode proxy pointer is passed into datanode proxy manager");
        
        if (dataNodeProxy->isConfigValid())
        {
            //dataNodeProxy->queueNotification(ConfigAvailable);
			boost::shared_ptr<DnProxyDynamicSyncUpdate> update = boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
			update->entityKey = dataNodeProxy->getEntityKey();;
			update->specialObserver = &observer;
			update->specialUpdateType = ConfigAvailable;
			//m_observerUpdateQueueProcessor.queueNotification(this, dataNodeProxy->getEntityKey(), update);

			insertUpdateQueue(update);
        }
    }


	void DataNodeProxyManager::onCopyProxy( IProxyObserverNotifier* observerNotifier,
                                                 IEntityUpdateEventProcessor& observer )
    {
        // Add observer
        observerNotifier->addObserver(observer);
    }


	void DataNodeProxyManager::onDeleteProxy( IProxyObserverNotifier* observerNotifier,
                                                   IEntityUpdateEventProcessor& observer )
    {
        observerNotifier->removeObserver(observer);

        // after proxy is created, it will be cached and never be deleted
    }


	/**
	 * Data Node Agent Interface not available
	 */
	void DataNodeProxyManager::onDataNodeAgentInterfaceNotAvailable (unsigned long entityKey)
	{
        boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
        proxy = m_proxyMap.get(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataNodeAgentInterfaceNotAvailable() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }
        
        boost::shared_ptr<DnProxyDynamicSyncUpdate> update = boost::shared_ptr<DnProxyDynamicSyncUpdate>(new DnProxyDynamicSyncUpdate);
        update->entityKey = entityKey;

		update->bAgentInterfaceNotAvailable = true;
		//m_observerUpdateQueueProcessor.queueNotification(this, entityKey, update);
		insertUpdateQueue(update);
	}

	/**
	 * Data Node config load from DB required
	 */
	void DataNodeProxyManager::onloadDataNodeConfiguration ( unsigned long entityKey )
	{
        boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
        proxy = m_proxyMap.get(entityKey);
        
        if( NULL == proxy.get() )
        {
            //if we haven't found it, this just means the proxy has been deleted so the 
            //update is invalid
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "onDataNodeConfigDbLoadRequired() Proxy for entity - %lu does not exist",
                entityKey );
            
            return;
        }
        
        proxy->loadConfiguration();
        //notify config available until get complete update or fail to connect to agent
        //proxy->queueNotification(ConfigAvailable);
    }


    //TD15939
	void DataNodeProxyManager::insertUpdateQueue(boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate)
	{
		//m_observerUpdateQueueProcessor.queueNotification(this, dataNodeUpdate->entityKey, dataNodeUpdate);
		//Before observerUpdateQueueProcessor start, will processing it directly.
		if (m_observerUpdateQueueProcessor.isProcessing())
		{
			m_observerUpdateQueueProcessor.queueNotification(this, dataNodeUpdate->entityKey, dataNodeUpdate);
		}
		else
		{
			processUpdateEvent(dataNodeUpdate);
		}
	}

	void DataNodeProxyManager::processUpdateEvent(boost::shared_ptr<ProxyDynamicSyncUpdate> updateValue)
	{
		boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate = boost::dynamic_pointer_cast<DnProxyDynamicSyncUpdate> (updateValue);
		DnProxyDynamicSyncUpdate* update = dataNodeUpdate.get();
	        
		TA_ASSERT( NULL != update, "DnProxyDynamicSyncUpdate is NULL in processUpdateEvent" );

		try
		{
			unsigned long  entityKey = dataNodeUpdate->entityKey;
			
			//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,	"ProxyManager process update [%d] for entity %d",dataNodeUpdate->detail.updateType, entityKey);

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"processUpdateEvent: entity [%lu] Type[%d], AgentInterfaceNotAvailable[%s],SpecialUpdateType[%d],SpecialObserver[%u]",
				entityKey,
				dataNodeUpdate->detail.updateType,
				dataNodeUpdate->bAgentInterfaceNotAvailable?"Yes":"No",
				dataNodeUpdate->specialUpdateType,
				dataNodeUpdate->specialObserver);

            boost::shared_ptr<DataNodeProxy> proxy ((DataNodeProxy*)NULL);
            proxy = m_proxyMap.get(entityKey);
            
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
            
            switch (dataNodeUpdate->detail.updateType)
			{
			case DnCompleteSyncState:
				{
                    bool isAlreadyConfigValid = proxy->isConfigValid();

                    //this function will set config valid to 'true'
                    proxy->updateState(dataNodeUpdate->detail.completeUpdate);

                    if (isAlreadyConfigValid)
                    {
                        proxy->notifyObservers(TagStateUpdate);
                        proxy->notifyObservers(AlarmSummaryUpdate);
                        proxy->notifyObservers(AckSummaryUpdate);
                        proxy->notifyObservers(ScanInhibitSummaryUpdate);
                        proxy->notifyObservers(AlarmInhibitSummaryUpdate);
                        proxy->notifyObservers(MmsAlarmInhibitSummaryUpdate);
                        proxy->notifyObservers(ControlInhibitSummaryUpdate);
                        proxy->notifyObservers(OverrideSummaryUpdate);
                        proxy->notifyObservers(QualitySummaryUpdate);
                        proxy->notifyObservers(ControlSummaryUpdate);
						proxy->notifyObservers(CombinedInhibitMMSUpdate);
                        proxy->notifyObservers(NotesStateUpdate);
                        proxy->notifyObservers(InhibitStateUpdate);
                        proxy->notifyObservers(ConfigStateLastOperatorUpdate);
                    }
                    else
                    {
                        proxy->notifyObservers(ConfigAvailable);
                    }
                    
					break;
				}
			case DnTagStateUpdate:
                proxy->updateTagState(dataNodeUpdate->detail.tagState);
                proxy->notifyObservers(TagStateUpdate);
				break;
                
			case DnAlarmSummaryUpdate :
                proxy->updateAlarmSummaryState(dataNodeUpdate->detail.alarmSummary);
                proxy->notifyObservers(AlarmSummaryUpdate);
                break;

			case DnAlarmAckSummaryUpdate :
                proxy->updateAlarmAckSummaryState(dataNodeUpdate->detail.ackSummary);
                proxy->notifyObservers(AckSummaryUpdate);
                break;

			case DnScanInhibitSummaryUpdate :
                proxy->updateScanInhibitSummaryState(dataNodeUpdate->detail.scanInhibitSummary);
                proxy->notifyObservers(ScanInhibitSummaryUpdate);
                break;

			case DnAlarmInhibitSummaryUpdate :
                proxy->updateAlarmInhibitSummaryState(dataNodeUpdate->detail.alarmInhibitSummary);
                proxy->notifyObservers(AlarmInhibitSummaryUpdate);
                break;

			case DnMmsAlarmInhibitSummaryUpdate :
                proxy->updateMmsAlarmInhibitSummaryState(dataNodeUpdate->detail.mmsAlarmInhibitSummary);
                proxy->notifyObservers(MmsAlarmInhibitSummaryUpdate);
                break;

			case DnControlInhibitSummaryUpdate :
                proxy->updateControlInhibitSummaryState(dataNodeUpdate->detail.controlInhibitSummary);
                proxy->notifyObservers(ControlInhibitSummaryUpdate);
                break;

			case DnOverrideSummaryUpdate :
                proxy->updateOverrideSummaryState(dataNodeUpdate->detail.overrideSummary);
                proxy->notifyObservers(OverrideSummaryUpdate);
                break;

			case DnQualitySummaryUpdate :
                proxy->updateQualitySummaryState(dataNodeUpdate->detail.qualitySummary);
                proxy->notifyObservers(QualitySummaryUpdate);
                break;

			case DnControlPendingSummaryUpdate :
                proxy->updateControlPendingSummaryState(dataNodeUpdate->detail.controlPendingSummary);
                proxy->notifyObservers(ControlSummaryUpdate);
                break;

			case DnCombinedInhibitMMSUpdate:
				proxy->updateCombinedInhibitMMSState(dataNodeUpdate->detail.combinedInhibitMMSState);
				proxy->notifyObservers(CombinedInhibitMMSUpdate);
				break;

			case DnNotesUpdate:
                proxy->updateNotesState(dataNodeUpdate->detail.notes);
                proxy->notifyObservers(NotesStateUpdate);
                break;

            case DnInhibitMMSUpdate:
                proxy->updateInhibitState(dataNodeUpdate->detail.inhibitMMSState);
                proxy->notifyObservers(InhibitStateUpdate);
                break;

            case DnLastOperatorUpdate:
                proxy->updateLastOperator(dataNodeUpdate->detail.lastOperator);
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

}
