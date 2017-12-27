/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/MMSDatabaseCache.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This class holds cached database information to save returning to the database for each retrieval.
  */

#pragma warning(disable:4786)

//#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "MMSDatabaseCache.h"

#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"

#include "core/data_access_interface/src/IAlarmSeverityData.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	//init static instance member
	MMSDatabaseCache* MMSDatabaseCache::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable MMSDatabaseCache::m_singletonlock;

	MMSDatabaseCache* MMSDatabaseCache::getInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_singletonlock );

		FUNCTION_ENTRY("getInstance");

		if(0 == m_instance)
		{
			m_instance = new MMSDatabaseCache();
		}

		FUNCTION_EXIT;
		return m_instance;
	}

	MMSDatabaseCache::MMSDatabaseCache( ) 
	{
        FUNCTION_ENTRY("Constructor"); 

//         loadLocations();
//         loadOperators();
//         loadAlarmSeverities();
//         loadSubsystems();

        FUNCTION_EXIT;
	}


	MMSDatabaseCache::~MMSDatabaseCache()
	{
        FUNCTION_ENTRY("Destructor");
        try
        {
			 
			TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ENTITY, *this);
			TA_Base_Core::OnlineUpdateListener::cleanUp();
			 

            for(std::map<unsigned long, TA_Base_Core::ILocation*>::iterator iter = m_locations.begin(); iter != m_locations.end(); ++iter)
            {
                if (iter->second != NULL)
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }

            for(std::map<unsigned long, TA_Base_Core::IOperator*>::iterator opToDelete = m_operators.begin(); opToDelete != m_operators.end(); ++opToDelete)
            {
                if (opToDelete->second != NULL)
                {
                    delete opToDelete->second;
                    opToDelete->second = NULL;
                }
            }

            for(std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*>::iterator asToDelete = m_alarmSeverities.begin(); asToDelete != m_alarmSeverities.end(); ++asToDelete)
            {
                if (asToDelete->second != NULL)
                {
                    delete asToDelete->second;
                    asToDelete->second = NULL;
                }
            }

            for(std::map<unsigned long, TA_Base_Core::ISubsystem*>::iterator subsystemToDelete = m_subsystems.begin(); subsystemToDelete != m_subsystems.end(); ++subsystemToDelete)
            {
                if (subsystemToDelete->second != NULL)
                {
                    delete subsystemToDelete->second;
                    subsystemToDelete->second = NULL;
                }
            }

            for(std::map<unsigned long, TA_Base_Core::IEntityData*>::iterator entityToDelete = m_entities.begin(); entityToDelete != m_entities.end(); ++entityToDelete)
            {
                if (entityToDelete->second != NULL)
                {
                    delete entityToDelete->second;
                    entityToDelete->second = NULL;
                }
            }
        }
        catch( ... )
        {
        }

        FUNCTION_EXIT;
	}


    void MMSDatabaseCache::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        if (updateEvent.getType() != TA_Base_Core::ENTITY)
        {
            // Only interested in entity changes
            return;
        }

        if (updateEvent.getModifications() != TA_Base_Core::Update)
        {
            // Only interested in updates
            return;
        }

        TA_Base_Core::ThreadGuard guard(m_entityMapLock);
        std::map<unsigned long, TA_Base_Core::IEntityData*>::iterator matching = m_entities.find(updateEvent.getKey());
        if (matching != m_entities.end())
        {
            matching->second->invalidate();
        }


        FUNCTION_EXIT;
    }


    std::string MMSDatabaseCache::getLocationName(unsigned long key)
    {
        if (m_locations.empty())
        {
            loadLocations();
        }

        std::map<unsigned long, TA_Base_Core::ILocation*>::iterator matching = m_locations.find(key);
        if (matching == m_locations.end())
        {
            // Not found
            return "";
        }

        try
        {
            return matching->second->getName();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve location name");
        }

        return "";
    }


    std::string MMSDatabaseCache::getOperatorName(unsigned long key)
    {
        if (m_operators.empty())
        {
            loadOperators();
        }

        std::map<unsigned long, TA_Base_Core::IOperator*>::iterator matching = m_operators.find(key);
        if (matching == m_operators.end())
        {
            // Not found
            return "";
        }

        try
        {
            return matching->second->getDescription();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve operator name");
        }

        return "";
    }


    std::string MMSDatabaseCache::getSubsystemName(unsigned long key)
    {
        if (m_subsystems.empty())
        {
            loadSubsystems();
        }

        std::map<unsigned long, TA_Base_Core::ISubsystem*>::iterator matching = m_subsystems.find(key);
        if (matching == m_subsystems.end())
        {
            // Not found
            return "";
        }

        try
        {
            return matching->second->getName();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve subsystem name");
        }

        return "";
    }


    std::string MMSDatabaseCache::getAlarmSeverityName(unsigned long key)
    {
        if (m_alarmSeverities.empty())
        {
            loadAlarmSeverities();
        }

        std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*>::iterator matching = m_alarmSeverities.find(key);
        if (matching == m_alarmSeverities.end())
        {
            // Not found
            return "";
        }

        try
        {
            return matching->second->getName();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not retrieve alarm severity name");
        }

        return "";
    }




    TA_Base_Core::IEntityData& MMSDatabaseCache::getEntity(unsigned long key)
    {
        FUNCTION_ENTRY("getEntity");

        TA_Base_Core::ThreadGuard guard(m_entityMapLock);

        std::map<unsigned long, TA_Base_Core::IEntityData*>::iterator matching = m_entities.find(key);
        if (matching != m_entities.end())
        {
            FUNCTION_EXIT;
            return *(matching->second);
        }

        // This means we have to load the entity as it is not already cached
        TA_Base_Core::IEntityData* entity = NULL;
        try
        {
            entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(key);
            m_entities.insert(std::map<unsigned long, TA_Base_Core::IEntityData*>::value_type( key, entity ));

            // Register for any online updates of this entity so we always have the latest information
            // NOTE: Registering for ENTITY updates may not be enough. We may need to register for
			// ENTITY_OWNER instead.
		 
			TA_Base_Core::OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY, *this, key);
			 
        }
        catch( ... )
        {
            // Clean up before this exception escapes
            if( entity != NULL)
            {
                delete entity;
                entity = NULL;
            }
            throw;
        }

		FUNCTION_EXIT;
        return *entity;
    }


    std::string MMSDatabaseCache::getAlarmTypeKey(std::string alarmTypeName)
    {
        TA_Base_Core::ThreadGuard guard(m_alarmTypeMapLock);

        std::map<std::string,std::string>::iterator matching = m_alarmTypes.find(alarmTypeName);
        if (matching != m_alarmTypes.end())
        {
            return matching->second;
        }

        // Not found so we must load the alarm type from the database
        std::ostringstream keyToReturn("");
        TA_Base_Core::IAlarmTypeData* alarmType = NULL;
        try
        {
            alarmType = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType(alarmTypeName,false);

            keyToReturn << alarmType->getKey();

            m_alarmTypes.insert(std::map<std::string,std::string>::value_type(alarmTypeName, keyToReturn.str()) );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve alarm type");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Alarm type key for %s could not be determined", alarmTypeName.c_str());
        }

        if (alarmType != NULL)
        {
            delete alarmType;
            alarmType = NULL;
        }

        return keyToReturn.str();
    }


    void MMSDatabaseCache::loadLocations()
    {
        FUNCTION_ENTRY("loadLocations");

        // Delete any existing locations
        for(std::map<unsigned long, TA_Base_Core::ILocation*>::iterator iter = m_locations.begin(); iter != m_locations.end(); ++iter)
        {
            if (iter->second != NULL)
            {
                delete iter->second;
                iter->second = NULL;
            }
        }
        m_locations.clear();

        // Now load new locations
        std::vector<TA_Base_Core::ILocation*> locations;
        try
        {
            locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
            for(std::vector<TA_Base_Core::ILocation*>::iterator location = locations.begin(); location != locations.end(); ++location)
            {
                m_locations[(*location)->getKey()] = *location;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load locations from the database");
            
            // Clean up and delete any locations that may have been retrieved
            for(std::vector<TA_Base_Core::ILocation*>::iterator location = locations.begin(); location != locations.end(); ++location)
            {
                if (*location != NULL)
                {
                    delete *location;
                    *location = NULL;
                }
            }
        }

        FUNCTION_EXIT;
    }

	


    void MMSDatabaseCache::loadOperators()
    {
        FUNCTION_ENTRY("loadOperators");

        // Delete any existing operators
        for(std::map<unsigned long, TA_Base_Core::IOperator*>::iterator opToDelete = m_operators.begin(); opToDelete != m_operators.end(); ++opToDelete)
        {
            if (opToDelete->second != NULL)
            {
                delete opToDelete->second;
                opToDelete->second = NULL;
            }
        }
        m_operators.clear();

        // NOw load operators fresh from db
        std::vector<TA_Base_Core::IOperator*> operators;
        try
        {
            operators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperators();
            for(std::vector<TA_Base_Core::IOperator*>::iterator iter = operators.begin(); iter != operators.end(); ++iter)
            {
                m_operators[(*iter)->getKey()] = *iter;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load operators from the database");
            
            // Clean up and delete any locations that may have been retrieved
            for(std::vector<TA_Base_Core::IOperator*>::iterator iter = operators.begin(); iter != operators.end(); ++iter)
            {
                if (*iter != NULL)
                {
                    delete *iter;
                    *iter = NULL;
                }
            }
        }

        FUNCTION_EXIT;
    }


    void MMSDatabaseCache::loadAlarmSeverities()
    {
        FUNCTION_ENTRY("loadAlarmSeverities");

        // Delete any existing alarm severities
        for(std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*>::iterator asToDelete = m_alarmSeverities.begin(); asToDelete != m_alarmSeverities.end(); ++asToDelete)
        {
            if (asToDelete->second != NULL)
            {
                delete asToDelete->second;
                asToDelete->second = NULL;
            }
        }
        m_alarmSeverities.clear();

        // Now load alarm severities fresh from db
        std::vector<TA_Base_Core::IAlarmSeverityData*> severities;
        try
        {
            severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
            for(std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = severities.begin(); iter != severities.end(); ++iter)
            {
                m_alarmSeverities[(*iter)->getKey()] = *iter;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load alarm severities from the database");
            
            // Clean up and delete any locations that may have been retrieved
            for(std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = severities.begin(); iter != severities.end(); ++iter)
            {
                if (*iter != NULL)
                {
                    delete *iter;
                    *iter = NULL;
                }
            }
        }

        FUNCTION_EXIT;
    }


    void MMSDatabaseCache::loadSubsystems()
    {
        FUNCTION_ENTRY("loadSubsystems");

        // Delete any existing subsystems
        for(std::map<unsigned long, TA_Base_Core::ISubsystem*>::iterator subsystemToDelete = m_subsystems.begin(); subsystemToDelete != m_subsystems.end(); ++subsystemToDelete)
        {
            if (subsystemToDelete->second != NULL)
            {
                delete subsystemToDelete->second;
                subsystemToDelete->second = NULL;
            }
        }
        m_subsystems.clear();

        // Now load subsystems fresh from db
        std::vector<TA_Base_Core::ISubsystem*> subsystems;
        try
        {
            subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems();
            for(std::vector<TA_Base_Core::ISubsystem*>::iterator iter = subsystems.begin(); iter != subsystems.end(); ++iter)
            {
                m_subsystems[(*iter)->getKey()] = *iter;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not load subsystems from the database");
            
            // Clean up and delete any locations that may have been retrieved
            for(std::vector<TA_Base_Core::ISubsystem*>::iterator iter = subsystems.begin(); iter != subsystems.end(); ++iter)
            {
                if (*iter != NULL)
                {
                    delete *iter;
                    *iter = NULL;
                }
            }
        }

        FUNCTION_EXIT;
    }

}
