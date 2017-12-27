/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/MmsScadaUtility.cpp $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */

//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif // defined _MSC_VER

#include "bus/scada/common_library/src/MmsScadaUtility.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datanode_library/src/DataNodeTypes.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsGlobalInhibitCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	const std::string MmsScadaUtility::MMSDataCustomToken = "and e.pkey IN (select entitykey from entityparametervalue where parameterkey = (select pkey from \
										    entityparameter where name = 'MMSReportType') and value IN ('CPT', 'VAL'))";
	const int MmsScadaUtility::RETRY_TIME = 75;
	const int MmsScadaUtility::RETRY_TIME_SLICE = 20;

	MmsScadaUtility::MmsScadaUtility()
    :
    m_localAgentName (""),
    m_localEntityMap (NULL),
    m_globalInhibitProxyCreated(false),
    m_proxyFactory(NULL),
	m_globalInhibitProxy(NULL),
	m_globalInhibitDpKey(0)
	{
        // listen for entity updates so we can refresh the cache maps
        // TD11885 - Online update only allows a single registration to all entity updates
        // TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ENTITY, *this);
        // Mintao++(TD13650)
        initScadaProxyFactory();
		// Mintao++(TD13650)
	}

	MmsScadaUtility::MmsScadaUtility(std::string agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap)
    :
    m_localAgentName (""),
    m_localEntityMap (NULL),
    m_globalInhibitProxyCreated(false),
    m_proxyFactory(NULL),
	m_globalInhibitProxy(NULL),
	m_globalInhibitDpKey(0)
	{
        // listen for entity updates so we can refresh the cache maps

        // TD11885 - Online update only allows a single registration to all entity updates
        // TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ENTITY, *this);
        // Mintao++(TD13650)
        initScadaProxyFactory();
		registerLocalEntities(agentName, entityMap);
	}

    void MmsScadaUtility::initScadaProxyFactory()
    {
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
		//m_proxyFactory->setProxiesToControlMode();
    }

	MmsScadaUtility::~MmsScadaUtility()
	{
		cleanProxyCache();

		{
			TA_THREADGUARD(m_globalInhibitLock);
			if(m_globalInhibitProxy != NULL)
			{
				delete m_globalInhibitProxy;
				m_globalInhibitProxy = NULL;
			}
		}

		releaseMMSEntityMap();

		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_proxyFactory = NULL;
		// Mintao++ (TD13650)
	}
	
	void MmsScadaUtility::cleanProxyCache()
	{
		{
			TA_THREADGUARD(m_subsystemNodesLock);
			std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it;
			for(it = m_subsystemNodes.begin(); it != m_subsystemNodes.end(); it++)
			{
				DataNodeProxySmartPtr * dnProxy = it->second;
				if(dnProxy != NULL)
				{
					delete dnProxy;
					dnProxy = NULL;
				}
			}
			m_subsystemNodes.clear();
		}

		{
			TA_THREADGUARD(m_dpProxyCacheLock);
			std::map<unsigned long, DataPointProxySmartPtr*>::iterator it;
			for(it = m_dpProxyCache.begin(); it != m_dpProxyCache.end(); it++)
			{
				DataPointProxySmartPtr* dpProxy = it->second;
				if(dpProxy != NULL)
				{
					delete dpProxy;
					dpProxy = NULL;
				}
			}
			m_dpProxyCache.clear();
		}
	}

    void MmsScadaUtility::processOnlineUpdateOfMaintenanceData()
    {
        // synchronize access to the map
		// clear all the maps, so next time the functions are called, we will refresh
        // data from the database
		releaseMMSEntityMap();   
    }

    void MmsScadaUtility::registerLocalEntities(const std::string& agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap)
	{
		m_localAgentName = agentName;
		m_localEntityMap = entityMap;
	}

	bool MmsScadaUtility::isDataPointMmsInhibited(unsigned long dataPointKey)
	{	
        // First check Global Inhibit
		if (isMmsGlobalInhibited())
		{
			return true;
		}

        std::string agentname;
        unsigned long dataNodeKey = 0;
        try
        {
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(dataPointKey);
            if (entityData.get() != NULL)
            {
                agentname = entityData->getAgentName();
                dataNodeKey = entityData->getParent();
            }
            else
            {
                // get entity data from database
                std::auto_ptr<TA_Base_Core::IEntityData> pEntitydata(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointKey, true));
                agentname = pEntitydata->getAgentName();
                dataNodeKey = pEntitydata->getParent();
            }
        }
        catch (...) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get the datapoint agent name[%d] from the database",dataPointKey );            
            return false;
        }

		// Then DataPoint Inhibit
		if (isMmsDpInhibited(dataPointKey, agentname))
		{
			return true;
		}
        // Do we need to check subsystem inhibition for each dp?
        // How to cater for the case that parent DN was MMS inhibited while child DP is not.
        // Do we consider that this child DP is also MMS inhibited?
		// Then Subsystem Inhibit
		return isMmsSubsystemInhibited(dataNodeKey);
	}


    bool MmsScadaUtility::isDataPointMmsInhibited(unsigned long dataPointKey, TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess)
    {
		// First check Global Inhibit
		if (isMmsGlobalInhibited())
		{
			return true;
		}

		// Then DataPoint Inhibit
		if (isMmsDpInhibited(dataPointKey, dataPointEntityAccess->getAgentName()))
		{
			return true;
		}
		
		// Then Subsystem Inhibit
		return isMmsSubsystemInhibited(dataPointEntityAccess->getParent());
	}

	bool MmsScadaUtility::isDataPointMmsInhibited(const std::string& dataPointName)
	{
		// First check Global Inhibit
		if (isMmsGlobalInhibited())
		{
			return true;
		}

        unsigned long dataPointKey = 0;
		std::string  agentName("");
		unsigned long dataNodeKey = 0;
        try
        {
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(dataPointName);
            if (entityData.get() != NULL)
            {
                dataPointKey = entityData->getKey();
				agentName = entityData->getAgentName();
				dataNodeKey = entityData->getParent();
            }
            else
            {
                // get entity data from database
                std::auto_ptr<TA_Base_Core::IEntityData> pEntitydata(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointName, true));
				dataPointKey = pEntitydata->getKey();
				agentName = pEntitydata->getAgentName();
				dataNodeKey = pEntitydata->getParent();
            }
        }
        catch (...) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get the datapoint key[%s] from the database",dataPointName.c_str() );            
            return false;
        }

		// Then DataPoint Inhibit
		if (isMmsDpInhibited(dataPointKey, agentName))
		{
			return true;
		}

		// Then Subsystem Inhibit
		return isMmsSubsystemInhibited(dataNodeKey);
	}

	bool MmsScadaUtility::isMmsGlobalInhibited()
	{
		bool inhibit = false;
		if (m_globalInhibitProxy != NULL)
		{
			try
			{
				inhibit = (*m_globalInhibitProxy)->getValueAsBool();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Mintao++(TD13650): Global Inhibit Datapoint status: %d", inhibit);    
			}
			catch(TA_Base_Core::ScadaProxyException& e)
			{
				std::string text = "isMmsGlobalInhibited() - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text.c_str());
			}
		}
		else if( !m_globalInhibitProxyCreated )
		{
			//this is the first time create global datapoint proxy
			createMmsGlobal();

			if (m_globalInhibitProxy != NULL)
			{
				unsigned int retries = 0;
				//here we will wait at most 1.5s, because proxy synchronize dynamic 
				//info need time when proxy is create at the first time
				while (retries < RETRY_TIME)
				{
					try
					{
						inhibit = (*m_globalInhibitProxy)->getValueAsBool();
						break;
					}
					catch(TA_Base_Core::ScadaProxyException& e)
					{
						TA_Base_Core::Thread::sleep(RETRY_TIME_SLICE);
						++retries;
					}
				}  

				if (retries >= RETRY_TIME)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "get global Inhibit value ScadaProxyException");
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "success to get global Inhibit value, retrytime:%d", retries);
				}
			}
		}

		return inhibit;
	}

    
	void MmsScadaUtility::processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType) 
    {
        // do nothing
    }


	bool MmsScadaUtility::isMmsDpInhibited(unsigned long dataPointKey, const std::string& agentName)
	{
		bool inhibit = false;
		
		if (dataPointKey == 0) return inhibit;
		
		//first check if this entity is in local cache
		if (m_localEntityMap != NULL && 0 == m_localAgentName.compare(agentName))
		{
			std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator iter = m_localEntityMap->find(dataPointKey);

			if (iter != m_localEntityMap->end())
			{
				TA_Base_Bus::DataPoint* dataPoint = NULL;
				dataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>(iter->second);
				if (dataPoint != NULL)
				{
					inhibit = (dataPoint->getMMSAlarmInhibitedState() == true);
				}
				return inhibit;
			}
		}

		//second check cache
		DataPointProxySmartPtr* dpProxy = NULL;
		{
			TA_THREADGUARD(m_dpProxyCacheLock);
			std::map<unsigned long, DataPointProxySmartPtr*>::iterator iter = m_dpProxyCache.find(dataPointKey);
			if (iter != m_dpProxyCache.end())
			{
				dpProxy = iter->second;
			}
		}

		if (NULL == dpProxy)
		{
			bool proxyAdded = false;
			DataPointProxySmartPtr* dpProxy = createMmsDpProxy(dataPointKey);
			if (dpProxy != NULL)
			{
				{
					TA_THREADGUARD(m_dpProxyCacheLock);
					std::map<unsigned long, DataPointProxySmartPtr*>::iterator iter = m_dpProxyCache.find(dataPointKey);
					if (iter == m_dpProxyCache.end())
					{
						m_dpProxyCache[dataPointKey] = dpProxy;
						proxyAdded = true;
					}
				}

				//here we will wait at most 1.5s, because proxy synchronize dynamic info need time
				int retryTimes = 0;
				while (retryTimes < RETRY_TIME)
				{
					try
					{
						inhibit = (*dpProxy)->isMmsAlarmInhibited();
						break;
					}
					catch (TA_Base_Core::ScadaProxyException& e)
					{
						TA_Base_Core::Thread::sleep(RETRY_TIME_SLICE);
						++retryTimes;
					}
				}

				if (retryTimes >= RETRY_TIME)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Failed to get MMSInhibited status for entity: %lu, due to ScadaProxyException:", dataPointKey);
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"success to get MMSInhibited status for entity: %lu, MmsInhibit:%d, retryTime:%d", dataPointKey, inhibit, retryTimes);
				}	

				if (!proxyAdded)
				{
					delete dpProxy;
					dpProxy = NULL;
				}
			}
		}
		else
		{
			try
			{
				inhibit = (*dpProxy)->isMmsAlarmInhibited();
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				// do nothing
			}
		}

		return inhibit;
	}
		
    bool MmsScadaUtility::isMmsSubsystemInhibited(unsigned long subsystemkey)
	{
		bool inhibit = false;

		if( subsystemkey == 0 )	return inhibit;

		// first check if this entity is in local entity cache
		if (m_localEntityMap != NULL)
		{
			std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator iter = m_localEntityMap->find(subsystemkey);
			if (iter != m_localEntityMap->end())
			{
				TA_Base_Bus::DataNode* dataNode = NULL;
				dataNode = dynamic_cast<TA_Base_Bus::DataNode*>(iter->second);
				if (dataNode != NULL)
				{
					BooleanTag combineMms = dataNode->getCombinedInhibitMMS();
					if (combineMms.quality == QUALITY_GOOD_NO_SPECIFIC_REASON)
					{
						inhibit = combineMms.value;
					}
				}

				return inhibit;
			}
		}

		//second check m_subsystemNodes cache
		DataNodeProxySmartPtr *ssProxy = 0;
		{
			TA_THREADGUARD(m_subsystemNodesLock);
			std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it = m_subsystemNodes.find(subsystemkey);
			if (it != m_subsystemNodes.end())
			{
				ssProxy = it->second;
			}
		}

		if (0 == ssProxy)
		{
			bool proxyAdded = false;
			ssProxy = createMmsDnPorxy(subsystemkey);
			if (ssProxy != NULL)
			{
				{
					TA_THREADGUARD(m_subsystemNodesLock);
					std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it = m_subsystemNodes.find(subsystemkey);
					if (it == m_subsystemNodes.end())
					{
						m_subsystemNodes[subsystemkey] = ssProxy;
						proxyAdded = true;
					}
				}

				//here we will wait at most 1.5s, because proxy synchronize dynamic info need time
				unsigned int retryTime = 0;
				while (retryTime < RETRY_TIME)
				{
					try
					{
						BooleanTag combineMms = (*ssProxy)->getCombinedInhibitMms();
						if (combineMms.quality == QUALITY_GOOD_NO_SPECIFIC_REASON  || 
							combineMms.quality == QUALITY_BAD_LAST_KNOWN_VALUE)
						{
							inhibit = combineMms.value;
						}
						break;
					}
					catch (TA_Base_Core::ScadaProxyException& e)
					{
						TA_Base_Core::Thread::sleep(RETRY_TIME_SLICE);
						++retryTime;
					}
				}

				if (retryTime >= RETRY_TIME)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Failed to get MMSInhibited status for entity: %lu, due to ScadaProxyException:", subsystemkey);
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"success to get MMSInhibited status for entity: %lu, MmsInhibit:%d, retryTime:%d", subsystemkey, inhibit, retryTime);
				}
			}
			
			if (!proxyAdded)
			{
				delete ssProxy;
				ssProxy = 0;
			}
		}
		else
		{
			try
			{
				BooleanTag combineMms = (*ssProxy)->getCombinedInhibitMms();
				if (combineMms.quality == QUALITY_GOOD_NO_SPECIFIC_REASON || 
					combineMms.quality == QUALITY_BAD_LAST_KNOWN_VALUE)
				{
					inhibit = combineMms.value;
				}
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				std::string text = "isMmsSubsystemInhibited() - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "isMmsSubsystemInhibited() - Unknown Exception");
			}
		}

		return inhibit;
	}

	std::vector<ScadaUtilities::MaintenanceData*> MmsScadaUtility::getMaintenanceDataObject(unsigned long locationKey)
	{
		TA_Base_Core::IEntityDataList dpEntities;
		if (0 == locationKey)
		{
			std::vector<TA_Base_Core::ILocation*>::iterator itLocation;
			std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllAgentLocations();	
					
			{
				TA_THREADGUARD(m_mmsDataLock);
				for(itLocation = locations.begin(); itLocation != locations.end(); ++itLocation)
				{
					TA_Base_Core::Location* pLocation = dynamic_cast<TA_Base_Core::Location*>(*itLocation);
					if(pLocation != NULL)
					{
						unsigned long tmpKey = pLocation->getKey();
						std::map<unsigned long, TA_Base_Core::IEntityDataList>::iterator itMMSEntity = m_mmsEntityMap.find(tmpKey);
						if (itMMSEntity != m_mmsEntityMap.end())
						{
							dpEntities.insert(dpEntities.end(), (itMMSEntity->second).begin(), (itMMSEntity->second).end());
						}	
						else
						{
							try
							{
								TA_Base_Core::IEntityDataList tmpEntities;
								tmpEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithCustomToken(
										TA_Base_Core::DataPointEntityData::getStaticType(),
										tmpKey,
										MMSDataCustomToken);
								dpEntities.insert(dpEntities.end(), tmpEntities.begin(), tmpEntities.end());
								m_mmsEntityMap[locationKey] = tmpEntities;
								m_locationKeys.push_back(locationKey);
							}
							catch (TA_Base_Core::DatabaseException& e)
							{
								std::string text = "getMaintenanceDataObject() - " + std::string(e.what());
								LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

								TA_THROW(TA_Base_Core::ScadaCommonException("Can not find find DataPoint Entities in this location"));
							}
							catch (TA_Base_Core::DataException& e)
							{
								std::string text = "getMaintenanceDataObject() - " + std::string(e.what());
								LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

								TA_THROW(TA_Base_Core::ScadaCommonException("Can not find DataPoint Entities in this location"));
							}
							catch (...)
							{
								TA_THROW(TA_Base_Core::ScadaCommonException("Can not find DataPoint Entities in this location for unknow exception"));
							}
						}
					}
				}
			}
		}
		else
		{
			TA_THREADGUARD(m_mmsDataLock);
			std::map<unsigned long, TA_Base_Core::IEntityDataList>::iterator itMMSEntity = m_mmsEntityMap.find(locationKey);
			if (itMMSEntity != m_mmsEntityMap.end())
			{
				dpEntities = itMMSEntity->second;
			}	
			else
			{
				try
				{
					dpEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithCustomToken(
							TA_Base_Core::DataPointEntityData::getStaticType(),
							locationKey,
							MMSDataCustomToken);
					m_mmsEntityMap[locationKey] = dpEntities;
					m_locationKeys.push_back(locationKey);
				}
				catch (TA_Base_Core::DatabaseException& e)
		        {
			        std::string text = "getMaintenanceDataObject() - " + std::string(e.what());
 			        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

			        TA_THROW(TA_Base_Core::ScadaCommonException("Can not find find DataPoint Entities in this location"));
		        }
		        catch (TA_Base_Core::DataException& e)
		        {
			        std::string text = "getMaintenanceDataObject() - " + std::string(e.what());
 			        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

			        TA_THROW(TA_Base_Core::ScadaCommonException("Can not find DataPoint Entities in this location"));
		        }
				catch (...)
				{
					TA_THROW(TA_Base_Core::ScadaCommonException("Can not find DataPoint Entities in this location for unknow exception"));
				}
			}

		}

		TA_Base_Core::IEntityDataList::iterator itEntity;
		std::vector<ScadaUtilities::MaintenanceData*> dataList;

		for (itEntity = dpEntities.begin(); itEntity != dpEntities.end(); ++itEntity)
		{	
			ScadaUtilities::MaintenanceData* pMaintData = getMmmMaintData(*itEntity);
			if (pMaintData != NULL)
			{
				dataList.push_back(pMaintData);
			}
		}
		
		return dataList;
	}	   
	ScadaUtilities::MaintenanceData* MmsScadaUtility::getMmmMaintData(TA_Base_Core::IEntityData* entityData)
	{
		ScadaUtilities::MaintenanceData* pMaintData = NULL;

		TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData);
		if (dpEntityData != NULL)
		{
			TA_Base_Core::EntityHelper* pHelper = dpEntityData->getHelper();
			
			int mmsPeriod = 0;
			int mmsPeriodTime = -1;
			
			try
			{
				 mmsPeriod = TA_Base_Core::EntityHelper::getIntegerData(
					pHelper->getParameter(TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_REPORT_PERIOD));
			}
			catch (TA_Base_Core::DataException& e)
			{
				mmsPeriod = 0;
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
						"get MMSReportType of %d Error : %s", entityData->getKey(), e.what());
			}
			
			try
			{
				mmsPeriodTime = TA_Base_Core::EntityHelper::getIntegerData(
							pHelper->getParameter(TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_REPORT_TIME));
			}
			catch (TA_Base_Core::DataException& e)
			{
				mmsPeriodTime = -1;
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
						"get MMSReportTime of %d Error : %s", entityData->getKey(), e.what());
			}
			
			std::string mmsType = pHelper->getParameter(TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_REPORT_TYPE);

			//we should check the data's vaild
			// mmsType IN('CPT', 'VAL')
			// mmsPeriod IN(1, 24)
			// mmsPeriodTime (0 <= x <= 23)
			if(	(mmsPeriod == 1 || mmsPeriod == 24) && 
				(mmsType == "CPT" || mmsType == "VAL") &&
				(mmsPeriodTime >= 0 && mmsPeriodTime <= 23))
			{
				pMaintData = new ScadaUtilities::MaintenanceData();
				
				if(mmsType == "CPT")
				{					
					pMaintData->reportType = ScadaUtilities::MaintenanceData::CPT;
				}
				else
				{					
					pMaintData->reportType = ScadaUtilities::MaintenanceData::VAL;
				}
				
				if (mmsPeriod == 1)
				{
					pMaintData->reportPeroidType = ScadaUtilities::MaintenanceData::HOURLY_BASIS;				
				}
				else
				{
					pMaintData->reportPeroidType = ScadaUtilities::MaintenanceData::DAILY_BASIS;
				}			
				pMaintData->reportPeriodTime = mmsPeriodTime;
				pMaintData->agentName = entityData->getAgentName().c_str();
				pMaintData->entityKey = entityData->getKey();
				pMaintData->entityTypeKey = entityData->getTypeKey();
				pMaintData->entityTypeName = entityData->getType().c_str();

				std::string objectName = entityData->getName();
				std::string::size_type pos = objectName.find_last_of ( ".", objectName.length() );
				if ( pos > 0 )
				{
					pMaintData->pointName = objectName.substr ( pos+1, objectName.length() );
				}
				else
				{
					pMaintData->pointName = "";
				}
				
				pMaintData->assetName = "Error Unkown";
				try
				{	
					unsigned long parentKey = entityData->getParent();
					TA_Base_Core::IEntityDataPtr dnEntityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(parentKey);
					// first try to get the entity from DataNodeAccessFactory cache
					if (dnEntityData.get() != NULL)
					{
						if(dnEntityData->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) 
						{
							TA_Base_Core::DataNodeEntityData* dndata = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(dnEntityData.get());
							pMaintData->assetName = dndata->getAssetName();
							pMaintData->dnDescription = dndata->getDescription();
						}
					}
					else
					{
						TA_THREADGUARD(m_mmsDataLock);
						// second try to get the entity from our datanode cache					
						std::map<unsigned long, boost::shared_ptr<TA_Base_Core::DataNodeEntityData> >::iterator iter = m_mmsDNEntityMap.find(parentKey);
						if (iter != m_mmsDNEntityMap.end())
						{
							pMaintData->assetName = (iter->second)->getAssetName();
							pMaintData->dnDescription = (iter->second)->getDescription();
						}
						// third we must load from db
						else
						{
							// get configuration data from database
							TA_Base_Core::IEntityData* dn = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(parentKey);
							if(dn->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) 
							{
								TA_Base_Core::DataNodeEntityData* dndata = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(dn);
								pMaintData->assetName = dndata->getAssetName();
								pMaintData->dnDescription = dndata->getDescription();
								m_mmsDNEntityMap[parentKey] = boost::shared_ptr<TA_Base_Core::DataNodeEntityData>(dndata);
							}
						}
					}
					pMaintData->dpDescription = entityData->getDescription();
				}
				catch (...)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Error occur while trying to get asset name of %d", entityData->getKey());
				}
				
				pMaintData->value = 0;
				pMaintData->readFailed = true;
			}
			else
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"MMS Data of datapoint : %d configure wrong", entityData->getKey());
			}
		}
		
		return pMaintData;
	}

	void MmsScadaUtility::getMaintenanceDataValue(std::vector<ScadaUtilities::MaintenanceData*>& dataObjects )
	{
		std::vector<ScadaUtilities::MaintenanceData*>::iterator iter;
		for(iter = dataObjects.begin(); iter != dataObjects.end(); ++iter)
		{
			getMmsMaintDataValue(*(*iter));
		}
	}

	void MmsScadaUtility::getMmsMaintDataValue (ScadaUtilities::MaintenanceData& data)
	{
		try
		{
            boost::shared_ptr<TA_Base_Bus::DataPointValue> dpState;
			data.readFailed = true;

			if (m_localEntityMap == NULL || m_localAgentName != data.agentName)
			{
                TA_Base_Bus::DataPointAgentInterface dataPointNamedObject;
                std::string angentcorbaname = data.agentName + TA_Base_Bus::DP_ACCESS_INTERFACE;
                dataPointNamedObject.setCorbaName(data.agentName , angentcorbaname);
		
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				    "get the raw data value");

                dpState = boost::shared_ptr<TA_Base_Bus::DataPointValue>(new TA_Base_Bus::DataPointValue);

				TA_Base_Bus::ScadaCorbaTypes::UTag* rawdpState = NULL;

				CORBA_CALL_RETURN( rawdpState, dataPointNamedObject, getFieldValue, (data.entityKey) );//limin
                
                convertDataPointValue(*rawdpState, *dpState);
                delete rawdpState;
				rawdpState = NULL;
						    
			}
			else
			{
				std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator entityIt = 
					m_localEntityMap->find(data.entityKey);

				if (entityIt != m_localEntityMap->end())
				{
					TA_Base_Bus::DataPoint* dataPointLocalObject = NULL;
					dataPointLocalObject = 
						dynamic_cast<TA_Base_Bus::DataPoint*>((*entityIt).second);

					if (dataPointLocalObject != NULL)
                    {						
						
						dpState	= dataPointLocalObject->getFieldValue();		// TD18924 - use engineering value instead of raw value
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				            "get the raw data value");						
					}
				}
            }

            //now get the value
			if (dpState.get()!= NULL)
			{
				switch ( dpState->dpType )
				{
				case TA_Base_Bus::DPT_INT16_TYPE:
				case TA_Base_Bus::DPT_INT32_TYPE:
				case TA_Base_Bus::DPT_INT16S_TYPE:
				case TA_Base_Bus::DPT_INT32S_TYPE:
				case TA_Base_Bus::DPT_UINT16_TYPE:
				case TA_Base_Bus::DPT_UINT32_TYPE:
				case TA_Base_Bus::DPT_IEEE32_TYPE:
					data.value = dpState->floatValue;
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "The raw data float value is %f" , data.value);
					data.readFailed = false;
					break;
				case TA_Base_Bus::DPT_ENUM_TYPE:
					data.value = dpState->enumValue;
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "The raw data float value is %f" , data.value);
					data.readFailed = false;
                    break;

				case TA_Base_Bus::DPT_BOOL_TYPE:
					if (dpState->booleanValue)
					{
						data.value = 1;
					}
					else
					{
						data.value = 0;
					}
					data.readFailed = false;
					break;

				case TA_Base_Bus::DPT_TEXT_TYPE:
				default:
					data.value = 0;
					data.readFailed = true;
					break;
				}

			}
        }
		catch (TA_Base_Core::OperationModeException& e)
		{
			std::string text = "getMmsMaintDataValue() - " + std::string(e.reason);
 			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", text);

			data.readFailed = true;		
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "getMmsMaintDataValue() - Exception thrown while calling a CorbaFunction: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", exceptionMsg);

			data.readFailed = true;			
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getMmsMaintData() - Unknown Exception");
			data.readFailed = true;
			//TA_THROW(TA_Base_Core::ScadaCommonException("Can not read MMS Maint Data"));
		}
	}

	void MmsScadaUtility::releaseMMSEntityMap()
	{
		TA_THREADGUARD(m_mmsDataLock);

		std::vector<unsigned long>::iterator itKey;
		for(itKey = m_locationKeys.begin(); itKey != m_locationKeys.end(); ++itKey)
		{
			std::map<unsigned long, TA_Base_Core::IEntityDataList>::iterator itMmsMap = m_mmsEntityMap.find(*itKey);
			if(itMmsMap != m_mmsEntityMap.end())
			{
				std::vector<TA_Base_Core::IEntityData*>::iterator itData;
				for(itData = (itMmsMap->second).begin(); itData != (itMmsMap->second).end(); ++itData)
				{
					delete *itData;
					*itData = NULL;
				}
				itMmsMap->second.clear();
			}
		}
		
		m_locationKeys.clear();
		m_mmsEntityMap.clear();
		m_mmsDNEntityMap.clear();
	}


	bool MmsScadaUtility::loadMmsGlobalDp()
	{
		bool isExist = false;
		TA_Base_Core::IData* dbdata = NULL;
		std::string globalInhibitDpName("");

		try
		{
			std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			dbdata = TA_Base_Core::EntityAccessFactory::getInstance().getParameter (agentName, 
				TA_Base_Core::DataPointEntityData::GLOBAL_MMS_INHIBITED_DATAPOINT );

			if (dbdata != NULL && dbdata->getNumRows() > 0)
			{
				globalInhibitDpName = dbdata->getStringData ( 0, "VALUE" );
				if (!globalInhibitDpName.empty())
				{
					TA_Base_Core::IEntityData* entitydata( TA_Base_Core::EntityAccessFactory::getInstance().getEntity(globalInhibitDpName) );
					TA_Base_Core::DataPointEntityData* pData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entitydata);
					if (pData != NULL)
					{
						isExist = true;
						m_globalInhibitDpKey = pData->getKey();
					}
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"load Global Inhibit Data point:%s, entity exist:%d", globalInhibitDpName.c_str(), isExist);  
			}
		}
		catch ( TA_Base_Core::DatabaseException & e)
		{
			std::string text = "loadMmsGlobalDp() - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text.c_str());
		}
		catch ( TA_Base_Core::DataException & e)
		{
			std::string text = "loadMmsGlobalDp() - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text.c_str());
		}
		catch(...)
		{
			std::string text = "loadMmsGlobalDp() - unknow reason";
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknowException", text.c_str());
		}

		delete dbdata;
		dbdata = 0;

		return isExist;
	}

	void MmsScadaUtility::createMmsGlobal()
	{
		TA_THREADGUARD(m_globalInhibitLock);

		if ( !m_globalInhibitProxyCreated )
		{
			bool globalInhibitDpExist = loadMmsGlobalDp();
			if (globalInhibitDpExist)
			{
				DataPointProxySmartPtr* tmpProxy = new DataPointProxySmartPtr();
				try
				{
					m_proxyFactory->createDataPointProxy(m_globalInhibitDpKey, *this, *tmpProxy);
					m_globalInhibitProxy = tmpProxy;
				}
				catch(...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"create Global Inhibit Data point:%d Proxy failed", m_globalInhibitDpKey);   
					delete tmpProxy;
					tmpProxy = NULL;
				}
			}

			m_globalInhibitProxyCreated = true;
		}
	}

	DataNodeProxySmartPtr* MmsScadaUtility::createMmsDnPorxy(unsigned long key)
	{
		unsigned long		   subsystemKey = key;
		DataNodeProxySmartPtr* ssProxy      = NULL;
		
		if(subsystemKey != 0) 
		{
			try
			{
				ssProxy = new DataNodeProxySmartPtr();
				m_proxyFactory->createDataNodeProxy(subsystemKey, *this, *ssProxy);

				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"MmsScadaUtility create Subsystem %d proxy success" , subsystemKey);
			}
			catch (...)
			{
				if (ssProxy != NULL) 
				{
					delete ssProxy;
					ssProxy = NULL;
				}
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"MmsScadaUtility create Subsystem %d proxy failed" , subsystemKey);
			}
		}

		return ssProxy;
	}

	
	void MmsScadaUtility::createMmsDnPorxy(TA_Base_Core::DataNodeEntityDataPtr dnData)
	{
		unsigned long		   subsystemKey = dnData->getKey();
		DataNodeProxySmartPtr* ssProxy      = NULL;

		if(subsystemKey != 0) 
		{
			TA_THREADGUARD(m_subsystemNodesLock);
			std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it = m_subsystemNodes.find(subsystemKey);
			if (it == m_subsystemNodes.end())
			{
				try
				{
					ssProxy = new DataNodeProxySmartPtr();
					m_proxyFactory->createDataNodeProxy(dnData, *this, *ssProxy);
					m_subsystemNodes[subsystemKey] = ssProxy;

					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
						"MmsScadaUtility create Subsystem %d proxy success" , subsystemKey);
				}
				catch (...)
				{
					if (ssProxy != NULL) 
					{
						delete ssProxy;
						ssProxy = NULL;
					}
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
						"MmsScadaUtility create Subsystem %d proxy failed" , subsystemKey);
				}
			}
		}
	}

	DataPointProxySmartPtr* MmsScadaUtility::createMmsDpProxy(unsigned long key)
	{
		unsigned long			dpKey	= key;
		DataPointProxySmartPtr* dpProxy = NULL;

		if( dpKey != 0 ) 
		{
			try
			{
				dpProxy = new DataPointProxySmartPtr();
				m_proxyFactory->createDataPointProxy(dpKey, *this, *dpProxy);

				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"MmsScadaUtility create Data Point: %d proxy success" , dpKey);
			}
			catch (...)
			{
				if (dpProxy != NULL) 
				{
					delete dpProxy;
					dpProxy = NULL;
				}

				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"MmsScadaUtility create Data Point: %d proxy failed" , dpKey);
			}
		}

		return dpProxy;
	}

	void MmsScadaUtility::initDNProxyNotLocal(unsigned long agentKey)
	{
		try
		{
			TA_Base_Core::IEntityDataList dnList = 
				TA_Base_Core::EntityAccessFactory::getInstance().getDataNodeNotInAgentButChildDPInAgent(agentKey);

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"MmsScadaUtility initDNProxyNotLocal load DataNode:%d from db", dnList.size());

			TA_Base_Core::IEntityDataList::iterator iterDn;
			for (iterDn = dnList.begin(); iterDn != dnList.end(); ++iterDn)
			{
				TA_Base_Core::DataNodeEntityData* pDnData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(*iterDn);
				if (pDnData != NULL)
				{
					TA_Base_Core::DataNodeEntityDataPtr dnDataPtr(pDnData);
					createMmsDnPorxy(dnDataPtr);
				}
			}
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"MmsScadaUtility initDNProxyNotLocal load db exception");
		}
	}
}




