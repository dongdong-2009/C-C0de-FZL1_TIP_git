/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_Review/transactive/bus/scada/rtu_library/src/RTUFactory.cpp $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2017/02/13 14:56:06 $
  * Last modified by : $Author: Dhanshri $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER
#include "bus/scada/rtu_library/src/RTUFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/rtu_library/src/SteRTUManager.h"
//#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "core/threads/src/IThreadPoolBase.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
//#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
//#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

    RTUFactory*              RTUFactory::m_singleton = NULL;
    unsigned int             RTUFactory::m_refCounter = 0;
    TA_Base_Core::ReEntrantThreadLockable RTUFactory::m_singletonLock;

    RTUFactory::RTUFactory():m_currentMode(TA_Base_Core::Standby)
    {
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In RTUFactory::RTUFactory()");
	
		m_rtuManagers.clear();
    }


    RTUFactory::~RTUFactory()
    {
		RTUManagerMap::iterator itRTU;
		for (itRTU = m_rtuManagers.begin(); itRTU != m_rtuManagers.end(); ++itRTU)
		{
			if(itRTU->second != NULL)
			{
				delete itRTU->second;
				itRTU->second = NULL;
			}
		}
        
        // the log will cause crash when called removeInstance method, so removed it.
        // LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In RTUFactory::~RTUFactory()");

		m_rtuManagers.clear();
	}
    
    /**
     * RTUFactory::getInstance()
     *
     * Returns a pointer to the single instance
     * of SingeltonQueue, creating if necessary.
     *
     * @return  a pointer to the RTUFactory
     *
     */
    RTUFactory& RTUFactory::getInstance()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In RTUFactory::getInstance()");
        
		TA_THREADGUARD( m_singletonLock );

        if ( m_singleton == NULL )
        {
            m_singleton = new RTUFactory();
        }

        // increment reference counter
        m_refCounter++;

        return *m_singleton;
    }   

    void RTUFactory::removeInstance()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In RTUFactory::removeInstance()");

		TA_THREADGUARD( m_singletonLock );

        TA_ASSERT(m_refCounter > 0, "m_refCounter <= 0");

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            if ( m_singleton != NULL )
            {
                delete m_singleton;
                m_singleton = NULL;
            }
        }
    }


    void RTUFactory::addStationEntities(TA_Base_Core::IEntityDataPtr EntityData)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In RTUFactory::addStationEntities()");
		//Whole function needs guard as the map is accessed/modified number of times.
		TA_Base_Core::ThreadGuard guard(m_StnSysLock);

		if("Station" == EntityData->getType())
		{
			std::map< unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> >::iterator itr = m_LocalStationSystemEntityMap.find(EntityData->getKey());
			if(  itr == m_LocalStationSystemEntityMap.end())
			{
				/*std::vector<TA_Base_Core::IEntityDataPtr> vED;
				vED.clear();
				*///m_LocalStationSystemEntityMap.insert(EntityData->getKey(), vED);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In RTUFactory::addStationEntities() - Station added already!");			
			}
		}
		else if ("StationSystem" == EntityData->getType())
		{
			std::map< unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> >::iterator itr = m_LocalStationSystemEntityMap.find(EntityData->getParent());
			if(  itr == m_LocalStationSystemEntityMap.end())
			{	
				std::vector<TA_Base_Core::IEntityDataPtr> vED;
				vED.push_back(EntityData);
				m_LocalStationSystemEntityMap.insert(std::map<unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> >::value_type(EntityData->getParent(), vED));
			}
			else
			{
				(itr->second).push_back(EntityData);
			}
		}
		
		if(m_LocalStationSystemEntityMap.size() == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In RTUFactory::addStationEntities() - No Stations for this agent!");
			return;
		}
		std::map< unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> >::iterator itr = m_LocalStationSystemEntityMap.begin();
		while(itr != m_LocalStationSystemEntityMap.end())
		{
			//std::vector<TA_Base_Core::IEntityDataPtr>::iterator itrChild = itr->second;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In RTUFactory::addStationEntities() - Total %d StationSystem in Station %u!", itr->second.size(), itr->first);
			itr++;
		}
	}


	void RTUFactory::createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData)
    {
		// create and return the RTU entity
		TA_Base_Core::RTUEntityData *rtuEntityData = dynamic_cast<TA_Base_Core::RTUEntityData*>(EntityData.get());
		int groupID = rtuEntityData->getGroupID();
		RTUManagerMap::iterator it;
		{
			TA_Base_Core::ThreadGuard guard(m_RTUManagerLock);
			if( (it = m_rtuManagers.find(groupID) ) == m_rtuManagers.end())
			{
				m_rtuManagers[groupID] = new SteRTUManager(groupID,true,false,SINGLE_PRIMARY_RTU_PATTERN);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create RTUManager:%d of name (EKey)= %s (%ul)",groupID, EntityData->getName().c_str(), EntityData->getKey());
			}

			m_rtuManagers[groupID]->createRTUEntity(EntityData);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create RTUEntity %d", groupID);
	}
    

    void RTUFactory::createRTUWorkers()
    {
        // create rtu workers
        RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
        	pos->second->createRTUWorkers();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager %d has created RTU workers", pos->first);
		}

        // add station systems to rtu manager
        std::map< unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> >::iterator itrEntityData = m_LocalStationSystemEntityMap.begin();
        // for each of the child entity of the PMSAgent
        for (itrEntityData = m_LocalStationSystemEntityMap.begin();
                itrEntityData != m_LocalStationSystemEntityMap.end();
                itrEntityData++)
        {
            // access to child entities of the Station
			std::vector<TA_Base_Core::IEntityDataPtr> stationChildrenEntities = itrEntityData->second;
            // for each of the child entity of the Station
            std::vector<TA_Base_Core::IEntityDataPtr>::iterator stationChildItr;
            for (stationChildItr = stationChildrenEntities.begin();
                stationChildItr != stationChildrenEntities.end();
                stationChildItr++)
            {
                RTUManagerMap::iterator pos;
       			for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
				{	
					SteRTUManager* rtuManager = dynamic_cast<SteRTUManager*>(pos->second);
					bool ret = rtuManager->addStationSystem(*(*stationChildItr));
					if(ret == true)							
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully  addStationSystem [%s] to RTUManager [%d] ", (*stationChildItr)->getName().c_str(),pos->first);
					else
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed addStationSystem [%s] to RTUManager [%d]  ",(*stationChildItr)->getName().c_str(),pos->first);
				}
				//delete (*stationChildItr);
			}
            //delete (*itrEntityData);
        }
    }

    
    void RTUFactory::setRTUToMonitorMode()
    {
        //do nothing
		if( m_currentMode == TA_Base_Core::Monitor)
		{
			return; 
		}
		//Set mode
		m_currentMode = TA_Base_Core::Monitor;

		RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
        	pos->second->setToMonitor();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to monitor.", pos->first);
		}

    }


    void RTUFactory::setRTUToControlMode()
    {
        //do nothing
		if( m_currentMode == TA_Base_Core::Control)
		{
			return; 
		}
		//Set mode
		m_currentMode = TA_Base_Core::Control;

		RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{	
			pos->second->setToControl();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to control.", pos->first);
		}
    }
    
	void RTUFactory::setAgentAssetName(std::string& assetName)
	{
		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			pos->second->setAgentAssetName(assetName);
		}	
	}
    	
	void RTUFactory::setStationName(std::string& stationName)
	{
		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			pos->second->setStationName(stationName);
		}	
	}
    
	// set running parameter
    void RTUFactory::setIsRTUSwitchingDisabled(bool isDisabled)
	{
		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			pos->second->setIsRTUSwitchingDisabled(isDisabled);
		}
	}

	void RTUFactory::setAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
	{
        RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			pos->second->setAnalogueDpUpdateInterval(analogueDpUpdateInterval);
		}
	}
	

	void RTUFactory::addDataPointList(std::map < unsigned long, TA_Base_Bus::DataPoint *> &dataPointList)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In RTUFactory::addDataPointList(), Total DP count = %d.", dataPointList.size());
		// TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		// std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		// workItems.reserve(dataPointList.size());
		for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = dataPointList.begin(); itrDp != dataPointList.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
				// workItems.push_back(new CreateDPWorkItem(*this, itrDp->second, pItemSync));
                addDataPoint(itrDp->second);
			}
		}
	    // LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();		
		// threadPool->queueWorkItemList(workItems);
		// pItemSync->wait();
		// delete pItemSync;
	}

	void RTUFactory::addDataPoint(DataPoint * dp)
	{
    	RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			bool ret = pos->second->addDataPoint(dp);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager %d addDataPoint address %s ", pos->first,dp->getDataPointAddress().c_str(), ret==true?"success":"fail");
		}
	}

	void RTUFactory::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent, std::map < unsigned long, TA_Base_Bus::DataPoint *> &dataPointList)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In RTUFactory::updateDataPointConfiguration().");
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = dataPointList.find (updateEventPKey);

		// if found one
		if (dpIter != dataPointList.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;
			RTUManagerMap::iterator pos;

			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
                
                for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        			pos->second->removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
                
                for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        			pos->second->addDataPoint(dp);
                break;
            
            default:
                // do nothing, exit switch statement
                break;
			}

			//
			// check if the data point's update priority has been changed online
			//

			// get the list of changed parameter from the config update
			const std::vector< std::string > * configChanges = 0;
			configChanges = &updateEvent.getChangedParams();

			// for each of the changed parameters
			for (unsigned int i = 0; i < configChanges->size(); i++)
			{
				std::string paramName = (*configChanges)[i];

				// only interested in this parameter
				if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_PRIORITY)
				{
					//
					// update the RTU's with the data point whose update priority
					// has been changed
					//
					RTUManagerMap::iterator pos;
        			for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
					{
        				pos->second->removeDataPoint (dp);
						pos->second->addDataPoint (dp);
					}
				}
			}

			dp = 0;
		}
	}

	void	RTUFactory::updateRTUConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In RTUFactory::updateRTUConfiguration().");
		// if the update event is for RTU1
		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
        {
			// If RTU matches, it is processed, and hence no need to go further
			if( pos->second->updateRTUConfiguration(updateEvent) )
				return;
		}
	}

	CreateDPWorkItem::CreateDPWorkItem(RTUFactory& pfactory, DataPoint* dp, TA_Base_Core::WorkItemExeSync* workSync)
		:m_parent(pfactory), m_dp(dp), m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void CreateDPWorkItem::executeWorkItem()
	{
		m_parent.addDataPoint(m_dp);
		m_sync->itemEnd();
	}

};
