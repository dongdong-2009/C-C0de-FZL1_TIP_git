/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Location information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAgentEntityData.h"

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"

#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{


    ScadaDatabaseAccessor::ScadaDatabaseAccessor()
        : m_isLoaded(false)
    {
    }

    
    ScadaDatabaseAccessor::ScadaDatabaseAccessor(const std::vector<std::string>& entityTypes)
        : m_isLoaded(false),
          m_entityTypes(entityTypes)
    {
    }


    ScadaDatabaseAccessor::~ScadaDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // Delete anything currently in the lists
            for(LoadedScadaItems::iterator iter1 = m_items.begin(); iter1 != m_items.end(); ++iter1)
            {
                delete iter1->second;
                iter1->second = NULL;
            }
            m_items.clear();

            for(LoadedDerivedDataPoints::iterator iter2 = m_derivedDataPoints.begin(); iter2 != m_derivedDataPoints.end(); ++iter2)
            {
                delete iter2->second;
                iter2->second = NULL;
            }
            m_derivedDataPoints.clear();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in Destructor");
        }

        FUNCTION_EXIT;
    }


    void ScadaDatabaseAccessor::loadItems()
    {
        // Doesn't do anything as our tree holds the items. The method getEntitiesOfType() will load
        // the entities.
    }


    void ScadaDatabaseAccessor::invalidateData()
    {
        FUNCTION_ENTRY("invalidateData");

        m_isLoaded = false;

        for(LoadedDerivedDataPoints::iterator iter2 = m_derivedDataPoints.begin(); iter2 != m_derivedDataPoints.end(); ++iter2)
        {
            delete iter2->second;
            iter2->second = NULL;
        }
        m_derivedDataPoints.clear();

        FUNCTION_EXIT;
     }

     
    std::multimap<CString,unsigned long> ScadaDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        // Doesn't do anything as our tree holds the items
        std::multimap<CString,unsigned long> empty;
        return empty;
    }


    TA_Base_Core::IConfigItem* ScadaDatabaseAccessor::getItem(unsigned long key)
    {
        // Doesn't do anything as our tree holds the items and we do not need to call this method
        return NULL;
    }


    TA_Base_Core::IConfigDerivedDataPoint& ScadaDatabaseAccessor::getDerivedDataPointItem(unsigned long identifier)
    {
        FUNCTION_ENTRY("getDerivedDataPointItem");

        LoadedDerivedDataPoints::iterator matchingDP = m_derivedDataPoints.find(identifier);
        if (matchingDP != m_derivedDataPoints.end())
        {
            FUNCTION_EXIT;
            return *(matchingDP->second);
        }

        LoadedScadaItems::iterator matchingScadaItem = m_items.find(identifier);
        TA_ASSERT(matchingScadaItem != m_items.end(), "The identifier passed in was invalid and could not be found");

        // Try to retrieve an existing derived datapoint from the database
        TA_Base_Core::IConfigDerivedDataPoint* dataPoint = NULL;
        if ( !matchingScadaItem->second->isNew() )
        {
            dataPoint = TA_Base_Core::DerivedDataPointAccessFactory::getInstance().
                                               getDerivedDataPoint( matchingScadaItem->second->getKey(), true );
            if (dataPoint != NULL)
            {
                m_derivedDataPoints.insert( LoadedDerivedDataPoints::value_type(identifier,dataPoint) );

                FUNCTION_EXIT;
                return *dataPoint;
            }
        }

        // Else create a new derived data point object for this datapoint

        dataPoint = TA_Base_Core::DerivedDataPointAccessFactory::getInstance().newDerivedDataPoint();

        m_derivedDataPoints.insert( LoadedDerivedDataPoints::value_type(identifier, dataPoint) );

        FUNCTION_EXIT;
        return *dataPoint;
    }


    void ScadaDatabaseAccessor::deleteItem(unsigned long identifier)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the item matching the identifier passed in.
        LoadedScadaItems::iterator matching = m_items.find(identifier);
        if (matching == m_items.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the entity with unique identifier %lu",identifier);
            FUNCTION_EXIT;
        }

        deleteDerivedDataPointItem( identifier);

        TA_ASSERT(matching->second != NULL, "Stored entity was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisEntity();
        }

        // The actual entity object must be deleted by the caller of this method as the object is needed
        // after this. So we will just erase it from the map.
        m_items.erase(matching);

        FUNCTION_EXIT;
    }


    void ScadaDatabaseAccessor::deleteDerivedDataPointItem( unsigned long id )
    {
        FUNCTION_ENTRY("deleteDerivedDataPointItem");

        LoadedDerivedDataPoints::iterator matchingDataPoint = m_derivedDataPoints.find( id );
        if( matchingDataPoint != m_derivedDataPoints.end() )
        {
            if (!matchingDataPoint->second->isNew())
            {
                matchingDataPoint->second->deleteThisDataPoint();
            }

            delete matchingDataPoint->second;
            matchingDataPoint->second = NULL;

            m_derivedDataPoints.erase( matchingDataPoint );
        }

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ScadaDatabaseAccessor::newItem() 
    {
        TA_ASSERT(false,"This method should not be called. newItem(type) should be called instead");

		throw;		
    }

    TA_Base_Core::IConfigItem* ScadaDatabaseAccessor::newItem(const std::string& type)
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigItem* newEntity = TA_Base_Core::EntityAccessFactory::getInstance().createEntity(type);

        m_items.insert( LoadedScadaItems::value_type( newEntity->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IConfigEntity*>(newEntity) ) );

        return newEntity;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ScadaDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigEntity* newEntity = NULL;

        // Attempt to find the Location matching the key passed in.
        LoadedScadaItems::iterator matching = m_items.find(idOfItemToCopy);

        TA_ASSERT(matching != m_items.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigLocation stored in the map");

        newEntity = TA_Base_Core::EntityAccessFactory::getInstance().copyEntity(matching->second);

        if (newEntity != NULL)
        {
            m_items.insert( LoadedScadaItems::value_type( newEntity->getUniqueIdentifier(), newEntity ) );
        }

        // If we just copied a derived or output data point then we need to also copy
        // the derived data point details
        if(matching->second->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
        {
            std::string dataPointType = matching->second->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

            if (isDerivedDataPointType(dataPointType))
            {
                TA_Base_Core::IConfigDerivedDataPoint& dataPointToCopy = getDerivedDataPointItem( idOfItemToCopy ); 

                TA_Base_Core::IConfigDerivedDataPoint* dataPoint = TA_Base_Core::DerivedDataPointAccessFactory::getInstance().
                                                              copyDerivedDataPoint( &dataPointToCopy );

                m_derivedDataPoints.insert( LoadedDerivedDataPoints::value_type(newEntity->getUniqueIdentifier(), dataPoint) );
            }
        }
        
        return newEntity;

        FUNCTION_EXIT;
    }


    bool ScadaDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end();  ++iter)
        {
            if( iter->second->hasChanged() )
            {
                try
                {
                    itemsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this SCADA item that has not had it's changes applied yet");
                    itemsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this SCADA item that has not had it's changes applied yet");
                    itemsNotApplied.push_back("Unknown");
                }
            }
            else if(iter->second->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
            {
                std::string dataPointType = iter->second->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

                if (isDerivedDataPointType(dataPointType))
                {
                    // The entity itself has not changed but we must check if it has a derived data point
                    // and see if that has changed
                    LoadedDerivedDataPoints::iterator matching = m_derivedDataPoints.find( iter->second->getUniqueIdentifier() );
                    if( (matching != m_derivedDataPoints.end()) && (matching->second->hasChanged()) )
                    {
                        itemsNotApplied.push_back(iter->second->getName());
                    }
                }
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::vector<TA_Base_Core::IEntityData*> ScadaDatabaseAccessor::getChildEntitiesOf(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getChildEntitiesOf");

        using TA_Base_Core::IEntityData;

        typedef std::vector<IEntityData*> EntityVector;
        typedef std::map<unsigned long, IEntityData*> EntityMap;

        EntityVector entitiesToReturn;

        // Retrieve all child entities from the database and put them in a map for easy searching
        EntityMap databaseEntityMap;
        try
        {
            EntityVector databaseEntities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey, true);
            for (EntityVector::iterator iter = databaseEntities.begin(); iter != databaseEntities.end(); ++iter)
            {
                databaseEntityMap.insert( EntityMap::value_type( (*iter)->getKey(), *iter ) );
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve a list of child entities.");
        }

        // Now step through each of our local already-loaded entities and see if they match the critera.
        for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (iter->second->getParent() == entityKey)
            {
                entitiesToReturn.push_back( iter->second );

                // If this is in the database list we must remove it so we don't add it again at the end
                // Of course it will only be in the database list if it is a new item so we check that first
                if (!iter->second->isNew())
                {
                    EntityMap::iterator matching = databaseEntityMap.find( iter->second->getKey() );
                    if (matching != databaseEntityMap.end())
                    {
                        delete matching->second;
                        matching->second = NULL;
                        databaseEntityMap.erase( matching );
                    }
                }
            }
        }

        // Now any remaining entities in the database map must be checked to see if they are either:
        // a) Entities we have not loaded yet in which case we must store them
        // b) Entities that have had their parent changed locally but not in the database. IN this case we
        //    can ignore them.
        for(EntityMap::iterator dbEntity = databaseEntityMap.begin(); dbEntity != databaseEntityMap.end(); ++dbEntity)
        {
            bool haveFoundMatching = false;
            for(LoadedScadaItems::iterator storedEntity = m_items.begin(); storedEntity != m_items.end(); ++storedEntity)
            {
                // Ignore new items
                if (storedEntity->second->isNew())
                {
                    continue;
                }

                // Have we got a match?
                if (storedEntity->second->getKey() == dbEntity->second->getKey() )
                {
                    // This item will have already been added to the list or has had it's parent changed
                    // and no longer needs to be added. So we can ignore it
                    delete dbEntity->second;
                    dbEntity->second =  NULL;
                    haveFoundMatching = true;
                    break;
                }
            }

            if (!haveFoundMatching)
            {
                entitiesToReturn.push_back( dbEntity->second );

                // Also store this for later use in our map
                TA_Base_Core::IConfigEntity* configEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(dbEntity->second);
                m_items.insert( LoadedScadaItems::value_type(configEntity->getUniqueIdentifier(),configEntity) );
            }
        }

        FUNCTION_EXIT;
        return entitiesToReturn;
    }

    
    bool ScadaDatabaseAccessor::hasChildren(unsigned long entityKey, bool hasChildren)
    {
        // Step through each of our local already-loaded entities and see if they match the critera.
        // We only need to check these as these will include any changes to entities that the caller to
        // this class won't have looked at.
        for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (iter->second->getParent() == entityKey)
            {
                return true;
            }
        }
        
        return hasChildren;
    }


    std::vector<TA_Base_Core::IEntityData*> ScadaDatabaseAccessor::getEntitiesOfType(const std::string& entityTypeName)
    {
        FUNCTION_ENTRY("getEntitiesOfType");

        using TA_Base_Core::IEntityData;
        using TA_Base_Core::IConfigEntity;

        // We only reload the entities if a reload has been called or this is the first time we are loading
        // them. Any other time we return what we already have stored.
        std::vector<IEntityData*> entities;
        if (!m_isLoaded)
        {
            // Delete anything currently in the list so we can start again
            for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
            {
                delete iter->second;
                iter->second = NULL;
            }
            m_items.clear();

            // Retrieve the new items and store them in our list. This can throw an exception
            entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(entityTypeName, true);

            for(std::vector<IEntityData*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
            {
                IConfigEntity* configEntity = dynamic_cast<IConfigEntity*>(*entity);
                m_items.insert( LoadedScadaItems::value_type(configEntity->getUniqueIdentifier(),configEntity) );
            }

            m_isLoaded = true;
            FUNCTION_EXIT;
            return entities;
        }

        // Else we need to step through what we have and find all entities of this entity type
        for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (0 == iter->second->getType().compare(entityTypeName) )
            {
                entities.push_back(iter->second);
            }
        }

        FUNCTION_EXIT;
        return entities;
    }
    

    TA_Base_Core::IEntityData* ScadaDatabaseAccessor::getEntity(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getEntity");

        // First see if we already have this entity in our list
        for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if( (!iter->second->isNew()) && (iter->second->getKey() == entityKey) )
            {
                FUNCTION_EXIT;
                return iter->second;
            }
        }

        // If we don't we'll get it fresh from the access factory
        TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey, true);

        FUNCTION_EXIT;
        return entity;
    }


    TA_Base_Core::IEntityData* ScadaDatabaseAccessor::getEntity(const std::string& entityName)
    {
        FUNCTION_ENTRY("getEntity");

        // First see if we already have this entity in our list
        for(LoadedScadaItems::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if( iter->second->getName() == entityName )
            {
                FUNCTION_EXIT;
                return iter->second;
            }
        }

        // If we don't we'll get it fresh from the access factory
        TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName, true);

        FUNCTION_EXIT;
        return entity;
    }


    std::map<unsigned long, std::string> ScadaDatabaseAccessor::getAllSeverities()
    {
        FUNCTION_ENTRY("getAllSeverities");

        using TA_Base_Core::IAlarmSeverityData;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the Severities from the database
        std::vector<IAlarmSeverityData*> severities;
        try
        {
            severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No severities will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No severities will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ISeverity object when we're done
        for (std::vector<IAlarmSeverityData*>::iterator iter = severities.begin();
             iter != severities.end();
             ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This severity will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This severity will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::string ScadaDatabaseAccessor::getAlarmSeverityName(unsigned long key)
    {
        FUNCTION_ENTRY("getAlarmSeverityName");
        
        TA_Base_Core::IAlarmSeverityData* severity = NULL;
        std::string name = "";
        try
        {
            severity = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getSeverityByKey(key);
            name = severity->getName();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve the matching severity to determine the severity name");
        }

        if (severity != NULL)
        {
            delete severity;
            severity = NULL;
        }

        FUNCTION_EXIT;
        return name;
    }


    std::map<unsigned long, std::string> ScadaDatabaseAccessor::getAllLocations()
    {
        FUNCTION_ENTRY("getAllLocations");

        using TA_Base_Core::ILocation;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the locations from the database
        std::vector<ILocation*> locations;
        try
        {
            locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ILocation object when we're done
        for (std::vector<ILocation*>::iterator iter = locations.begin(); iter != locations.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This location will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This location will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> ScadaDatabaseAccessor::getAllSubsystems()
    {
        FUNCTION_ENTRY("getAllSubsystems");
        std::map<unsigned long, std::string> namesAndKeys;

        using TA_Base_Core::ISubsystem;

        // First retrieve all the subsystems from the database
        std::vector<ISubsystem*> subsystems;
        try
        {
            subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllPhysicalSubsystems();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No subsystems will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No subsystems will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ISubsystem object when we're done
        for (std::vector<ISubsystem*>::iterator iter = subsystems.begin(); iter != subsystems.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This subsystem will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This subsystem will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> ScadaDatabaseAccessor::getAllAgents(const std::string& type)
    {
        FUNCTION_ENTRY("getAllAgents");
        std::map<unsigned long, std::string> namesAndKeys;

        using TA_Base_Core::IEntityData;

        // Generate a vector of all the entity types we must retrieve
        std::vector<std::string> entityTypesToRetrieve;

        if (type == TA_Base_Core::DataPointEntityData::getStaticType())
        {
            entityTypesToRetrieve.push_back(TA_Base_Core::VirtualDataPointAgentEntityData::getStaticType());
            entityTypesToRetrieve.push_back(TA_Base_Core::PMSAgentEntityData::getStaticType());
            entityTypesToRetrieve.push_back(TA_Base_Core::SystemStatusAgentEntityData::getStaticType());
        }
        else if (type == TA_Base_Core::DataNodeEntityData::getStaticType() )
        {
            entityTypesToRetrieve.push_back(TA_Base_Core::DataNodeAgentEntityData::getStaticType());
        }


        // Now loop through each type and get matching entities then add them to the list of names and keys
        for(std::vector<std::string>::iterator entityType = entityTypesToRetrieve.begin(); 
                                               entityType != entityTypesToRetrieve.end();
                                               ++entityType)
        {
            // First retrieve all the matching entities from the database
            std::vector<IEntityData*> entities;
            try
            {
                entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(*entityType);
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Some agents will be missing from the map.");
                continue;
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Some agents will be missing from the map.");
                continue;
            }

            // Now step through each one and:
            // 1) Add it's name and key to the map we want ot return
            // 2) Delete the ISubsystem object when we're done
            for (std::vector<IEntityData*>::iterator iter = entities.begin(); iter != entities.end(); ++iter)
            {
                if (*iter != NULL)
                {
                    try
                    {
                        namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                    }
                    catch ( const TA_Base_Core::DataException& )
                    {
                        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This agent will not be added to the map");
                    }
                    catch ( const TA_Base_Core::DatabaseException& )
                    {
                        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This agent will not be added to the map");
                    }

                    // Now we're finished with this iterator so we might as well delete what it contains so we
                    // don't have to do it later.
                    delete *iter;
                    *iter = NULL;
                }
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }

    
    bool ScadaDatabaseAccessor::isDerivedDataPointType(const std::string& dataPointType)
    {
        // Not simply a test against the DERIVED_TYPE - other categories of datapoints can
        // additionally have the properties of derived datapoints
        return (dataPointType == ScadaDetailView::DERIVED_TYPE) ||
                (dataPointType == ScadaDetailView::SAFETY_OUTPUT_TYPE) ||
                (dataPointType == ScadaDetailView::COMBINED_OUTPUT_TYPE) ||
                (dataPointType == ScadaDetailView::ENUM_TYPE);
    }


    std::map<std::string, std::string> ScadaDatabaseAccessor::getMmsAlarmTypes()
    {
        std::map<std::string, std::string> result;
        result.insert(std::map<std::string, std::string>::value_type("A", "Automatic"));
        result.insert(std::map<std::string, std::string>::value_type("S", "Semi-Automatic"));
        result.insert(std::map<std::string, std::string>::value_type("", "None"));
        return result;
    }

}
