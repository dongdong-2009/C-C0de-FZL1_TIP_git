/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve agent information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */

#pragma warning(disable:4786)

#include <algorithm> // Used for std::find

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"

#include "core/data_access_interface/src/IConfigGui.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/Process.h"

#include "core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::IEntityData;

namespace TA_Base_App
{
    AgentDatabaseAccessor::AgentDatabaseAccessor(const std::vector<std::string>& entityTypes)
        : m_isLoaded(false), m_entityTypes(entityTypes)
    {
    }


    AgentDatabaseAccessor::AgentDatabaseAccessor()
        : m_isLoaded(false)
    {
    }

    
    AgentDatabaseAccessor::~AgentDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAgents::iterator iter = m_agents.begin(); iter != m_agents.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void AgentDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the agents and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAgents::iterator agentToDelete = m_agents.begin(); agentToDelete != m_agents.end(); ++agentToDelete)
        {
            if( (agentToDelete->second) != NULL )
            {
                delete agentToDelete->second;
                agentToDelete->second = NULL;
            }
        }
        m_agents.clear();

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedEntities::iterator entityToDelete = m_entities.begin(); entityToDelete != m_entities.end(); ++entityToDelete)
        {
            if( (entityToDelete->second) != NULL )
            {
                delete entityToDelete->second;
                entityToDelete->second = NULL;
            }
        }
        m_entities.clear();

        // Now reload all the agents and entities

        // Get the entities and then create ConfigAgent objects from them.
        TA_Base_Core::IEntityDataList agents = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::Process::getStaticType(),true);
        
        using TA_Base_Core::IConfigEntity;
        for (TA_Base_Core::IEntityDataList::iterator iter = agents.begin(); iter != agents.end(); ++iter )
        {
            IConfigEntity* castEntity = dynamic_cast<IConfigEntity*>(*iter);
            if (castEntity != NULL)
            {
                ConfigAgent* agent = new ConfigAgent(*castEntity);
                m_agents.insert( LoadedAgents::value_type(agent->getUniqueIdentifier(), agent) );
            }
        }


        for (std::vector<std::string>::iterator type = m_entityTypes.begin(); type != m_entityTypes.end(); ++type)
        {
            TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType( *type, true );

            for (TA_Base_Core::IEntityDataList::iterator entity = entities.begin(); entity != entities.end(); ++entity )
            {
                IConfigEntity* castEntity = dynamic_cast<IConfigEntity*>(*entity);
                if (castEntity != NULL)
                {
                    m_entities.insert( LoadedEntities::value_type(castEntity->getUniqueIdentifier(), castEntity) );
                }
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AgentDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of agents times by 2. This is because we want to step for each
        // Agent while it is loaded and then again while each Agent is added to the list.
        progress.SetRange( 0, m_agents.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAgents::iterator iter = m_agents.begin();
             iter != m_agents.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                names.insert( std::multimap<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* AgentDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Agent matching the key passed in.
        LoadedAgents::iterator matching1 = m_agents.find(key);
        if (matching1 != m_agents.end() )
        {
            FUNCTION_EXIT;
            return matching1->second;
        }

        LoadedEntities::iterator matching2 = m_entities.find(key);
        if (matching2 != m_entities.end())
        {
            FUNCTION_EXIT;
            return matching2->second;
        }


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Agent or entity with unique identifier %lu",key);
        FUNCTION_EXIT;
        return NULL;
    }


    ConfigAgent* AgentDatabaseAccessor::getAgentFromKey(unsigned long key)
    {
        FUNCTION_ENTRY("getAgentFromKey");
        for (LoadedAgents::iterator iter = m_agents.begin(); iter != m_agents.end(); ++iter)
        {
            if( iter->second->doesAgentEntityExist() && iter->second->getAgentEntity()->getKey() == key)
            {
                FUNCTION_EXIT;
                return iter->second;
            }
        }

        FUNCTION_EXIT;
        return NULL;
    }


    std::vector<TA_Base_Core::IConfigItem*> AgentDatabaseAccessor::getItemsAtLocation(unsigned long locationKey)
    {
        FUNCTION_ENTRY("getItemsAtLocation");

        std::vector<TA_Base_Core::IConfigItem*> items;

        // Step through all the objects and retrieve those at the correct location.
        for (LoadedAgents::iterator iter = m_agents.begin();
             iter != m_agents.end();
             ++iter)
        {
            if ( (iter->second != NULL) && (iter->second->getLocation() == locationKey) )
            {
                items.push_back( iter->second );
            }
        }

        FUNCTION_EXIT;
        return items;
    }

        
    bool AgentDatabaseAccessor::locationHasChildren(unsigned long locationKey)
    {
        FUNCTION_ENTRY("locationHasChildren");

        // Step through all the objects and retrieve those at the correct location.
        for (LoadedAgents::iterator iter = m_agents.begin();
             iter != m_agents.end();
             ++iter)
        {
             try
             {
                    if ( (iter->second != NULL) && (iter->second->getLocation() == locationKey) )
                    {
                        // We've found a match. We can exit
                        FUNCTION_EXIT;
                        return true;
                    }
             }
             catch( ... )
             {
                 LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not determine if this item belongs to the specified location");
             }
        }

        FUNCTION_EXIT;
        return false;
    }


    std::vector<TA_Base_Core::IConfigItem*> AgentDatabaseAccessor::getChildItemsOfAgent(unsigned long agentId)
    {
        FUNCTION_ENTRY("getChildItemsOfAgent");

        std::vector<TA_Base_Core::IConfigItem*> items;

        LoadedAgents::iterator matching = m_agents.find(agentId);
        if (matching == m_agents.end())
        {
            // Could not find a matching agent so there must be no children of it
            FUNCTION_EXIT;
            return items;
        }

        if (!matching->second->doesAgentEntityExist())
        {
            // There is no child entity of the managed process so there cannot be any children it
            FUNCTION_EXIT;
            return items;
        }

        unsigned long agentKey = matching->second->getAgentEntity()->getKey();


        // Step through all the objects and retrieve those with the correct parent
        for (LoadedEntities::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter)
        {
            if ( (iter->second != NULL) && (iter->second->getParent() == agentKey) )
            {
                items.push_back( iter->second );
            }
        }

        FUNCTION_EXIT;
        return items;
    }


    bool AgentDatabaseAccessor::agentHasChildren(unsigned long agentId)
    {
        FUNCTION_ENTRY("agentHasChildren");

        LoadedAgents::iterator matching = m_agents.find(agentId);
        if (matching == m_agents.end())
        {
            // Could not find a matching agent so there must be no children of it
            FUNCTION_EXIT;
            return false;
        }

        TA_ASSERT(matching->second != NULL, "A NULL object was stored in the map");

        if (matching->second->isNew())
        {
            // If the agent entity is new then it cannot have any children yet so just return false
            FUNCTION_EXIT;
            return false;
        }

        if (!matching->second->doesAgentEntityExist())
        {
            // There is no child entity of the managed process so there cannot be any children it
            FUNCTION_EXIT;
            return false;
        }

        unsigned long agentKey = matching->second->getAgentEntity()->getKey();

        // Step through all the objects and retrieve those with the correct parent
        for (LoadedEntities::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter)
        {
            try
            {
                if ( (iter->second != NULL) && (iter->second->getParent() == agentKey) )
                {
                    return true;
                }
            }
             catch( ... )
             {
                 LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not determine if this item belongs to the specified agent");
             }
        }

        FUNCTION_EXIT;
        return false;
    }


    void AgentDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Agent matching the key passed in.
        LoadedAgents::iterator matching1 = m_agents.find(key);
        if (matching1 != m_agents.end() )
        {
            TA_ASSERT(matching1->second != NULL, "Stored Agent was equal to NULL");

            if (!matching1->second->isNew())
            {
                matching1->second->deleteThisAgent();
            }
            m_agents.erase(matching1);

            // We will delete what is stored at matching later. The tree still needs it

            FUNCTION_EXIT;
            return;
        }

        LoadedEntities::iterator matching2 = m_entities.find(key);
        if (matching2 != m_entities.end() )
        {
            TA_ASSERT(matching2->second != NULL, "Stored entity was equal to NULL");

            if (!matching2->second->isNew())
            {
                matching2->second->deleteThisEntity();
            }
            m_entities.erase(matching2);

            // We will delete what is stored at matching later. The tree still needs it

            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Agent or Entity with unique identifier %lu",key);
        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AgentDatabaseAccessor::newItem()
    {
        TA_ASSERT(false,"This method should not be called. newItem(type) should be called instead");
        return NULL;
    }


    TA_Base_Core::IConfigItem* AgentDatabaseAccessor::newItem(const std::string& type)
    {
        FUNCTION_ENTRY("newItem");

        if ( m_entityTypes.end() != std::find(m_entityTypes.begin(), m_entityTypes.end(), type) )
        {
            // This is not a new agent. It is a different type. So let's create it
            TA_Base_Core::IConfigEntity* newEntity = TA_Base_Core::EntityAccessFactory::getInstance().createEntity(type);
            m_entities.insert( LoadedEntities::value_type( newEntity->getUniqueIdentifier(), newEntity ) );

            FUNCTION_EXIT;
            return newEntity;
        }

        // This is a new agent (not a general entity)
        ConfigAgent* newAgent = new ConfigAgent(type);

        m_agents.insert( LoadedAgents::value_type( newAgent->getUniqueIdentifier(), newAgent ) );

        FUNCTION_EXIT;
        return newAgent;
    }


    TA_Base_Core::IConfigItem* AgentDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        // Attempt to find the Agent matching the key passed in.
        LoadedAgents::iterator matching1 = m_agents.find(idOfItemToCopy);
        if(matching1 != m_agents.end())
        {
            TA_ASSERT(matching1->second != NULL, "There was a NULL object stored in the map");

            // This is an agent we're copying
            ConfigAgent* newAgent = new ConfigAgent( *(matching1->second) );

            if (newAgent != NULL)
            {
                m_agents.insert( LoadedAgents::value_type( newAgent->getUniqueIdentifier(), newAgent ) );
            }

            FUNCTION_EXIT;
            return newAgent;
        }

        // Attempt to find the entity matching the key passed in.
        LoadedEntities::iterator matching2 = m_entities.find(idOfItemToCopy);
        if(matching2 != m_entities.end())
        {
            TA_ASSERT(matching2->second != NULL, "There was a NULL object stored in the map");


            // This is an agent we're copying
            TA_Base_Core::IConfigEntity* newEntity = TA_Base_Core::EntityAccessFactory::getInstance().copyEntity(matching2->second);
            if (newEntity != NULL)
            {
                m_entities.insert( LoadedEntities::value_type( newEntity->getUniqueIdentifier(), newEntity ) );
            }

            FUNCTION_EXIT;
            return newEntity;
        }

        TA_ASSERT(false, "The key passed could not be found in the map");
		return 0;
    }


    std::vector<std::string> AgentDatabaseAccessor::getAllEntityTypes()
    {
        FUNCTION_ENTRY("getAllEntityTypes");

        std::vector<std::string> typesToReturn = m_entityTypes;

        std::map<unsigned long, std::string> types;
        try
        {
            types = TA_Base_Core::EntityAccessFactory::getInstance().getChildTypesOf( TA_Base_Core::Process::getStaticType() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve child entity types of the MonitoredProcess entity type");
        }

        std::map<unsigned long, std::string>::iterator iter;

        for (iter = types.begin(); iter != types.end(); ++iter)
        {
            if (std::find(typesToReturn.begin(), typesToReturn.end(), iter->second) == typesToReturn.end() )
            {
                // This entity type is not already in the list so add it
                typesToReturn.push_back( iter->second );
            }
        }

        return typesToReturn;

        FUNCTION_EXIT;
    }


    std::map<unsigned long, std::string> AgentDatabaseAccessor::getAllLocations()
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


    std::vector<TA_Base_Core::ILocation*> AgentDatabaseAccessor::getAllLocationObjects()
    {
        FUNCTION_ENTRY("getAllLocationObjects");

        std::vector<TA_Base_Core::ILocation*> locations;
        try
        {
            locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No locations will be added to the vector. An empty vector will be returned");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No locations will be added to the vector. An empty vector will be returned");
        }

        FUNCTION_EXIT;
        return locations;
    }


    void AgentDatabaseAccessor::safeToDeleteItem( TA_Base_Core::IItem* item )
    {
        FUNCTION_ENTRY("safeToDeleteItem");

        // If its an agent make sure its not currently associated with a
        // system controller.
        ConfigAgent* agent = dynamic_cast<ConfigAgent*>(item);
        if (agent == NULL)
        {
            // Otherwise, see if its the child of an agent that shouldn't be deleted.
            TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(item);
            if (entity != NULL)
            {
                // Get the parent of the entity.
                agent = getAgentFromKey( entity->getParent() );
            }
        }

        if (agent != NULL)
        {
            std::vector<std::string> peers = TA_Base_Core::ManagedProcessAccessFactory::getInstance()
                                             .getPeersOfManagedProcess( agent->getMonitoredProcessEntity().getName() );
            if ( 0 < peers.size() )
            {
                // Generate the list of system controllers.
                std::string systemControllers;
                for( unsigned int i=0; i<peers.size(); ++i)
                {
                    systemControllers += peers[i];
					if(i != peers.size()-1 )
						systemControllers += "\n";
                }
                TA_THROW( TA_Base_Core::DataException("This agent cannot be deleted as it associated with one or more System Controllers.",
                          TA_Base_Core::DataException::CANNOT_DELETE,
                          systemControllers ) );
            }
        }

        FUNCTION_EXIT;
    }


    bool AgentDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& agentsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(agentsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAgents::iterator iter = m_agents.begin(); iter != m_agents.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    agentsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Agent that has not had it's changes applied yet");
                    agentsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Agent that has not had it's changes applied yet");
                    agentsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !agentsNotApplied.empty();
    }
}
