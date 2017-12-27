/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File$
 * @author Dirk McCormick and Karen Graham
 * @version $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by: $Author$
 * 
 * This provides the configuration for a monitored process entity and it's child agent entity
 */

#pragma warning(disable:4786)

#include <algorithm>

#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/Process.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"


using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IConfigEntity;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // Start large and count down so we don't clash with the other entity unique identifiers
    unsigned long ConfigAgent::s_nextIdentifier = ULONG_MAX;

    const std::string ConfigAgent::MONITORED_PROCESS_NAME_APPEND = "MonitoredProcess";
    const std::string ConfigAgent::AGENT_NAME_APPEND = "Agent";

    const std::string ConfigAgent::NAME_FIELD_NAME = "Name";
    const std::string ConfigAgent::ADDRESS_FIELD_NAME = "Address";
    const std::string ConfigAgent::EXECUTABLE_FIELD_NAME = "Executable";

    const std::string ConfigAgent::IS_MANAGED = "ManagedProcess";

    const std::string ConfigAgent::TA_TRUE  = "Yes";
    const std::string ConfigAgent::TA_FALSE = "No";


    ConfigAgent::~ConfigAgent()
    {
        FUNCTION_ENTRY("Destructor");
        try
        {
            if (m_agentEntity != NULL)
            {
                delete m_agentEntity;
                m_agentEntity = NULL;
            }
            if (m_monitoredProcessEntity != NULL)
            {
                delete m_monitoredProcessEntity;
                m_monitoredProcessEntity = NULL;
            }
        }
        catch ( ... )
        {
        }
        FUNCTION_EXIT;
    }


    ConfigAgent::ConfigAgent(TA_Base_Core::IConfigEntity& monitoredProcessEntity)
        : m_monitoredProcessEntity(&monitoredProcessEntity),
          m_agentEntity(NULL),
          m_uniqueIdentifier(--s_nextIdentifier)
    {
        FUNCTION_ENTRY("Constructor (existing agent)");

        m_agentEntity = getAgentEntityFromDB();
 
        FUNCTION_EXIT;
    }


    ConfigAgent::ConfigAgent(const std::string& type)
        : m_monitoredProcessEntity(NULL),
          m_agentEntity(NULL),
          m_uniqueIdentifier(--s_nextIdentifier)
    {
        FUNCTION_ENTRY("Constructor (creating new agent)");

        m_monitoredProcessEntity= TA_Base_Core::EntityAccessFactory::getInstance().createEntity(TA_Base_Core::Process::getStaticType());
        if (!type.empty())
        {
            m_agentEntity = TA_Base_Core::EntityAccessFactory::getInstance().createEntity(type);
        }

        // Now if this agent has an entity associated with it then it means it is probably a
        // managed process so set that to ticked.
        if( m_agentEntity != NULL)
        {
            m_monitoredProcessEntity->setParameterValue(TA_TRUE,IS_MANAGED);
        }
        else
        {
            // This cannot be a managed process so set the value to false.
            m_monitoredProcessEntity->setParameterValue(TA_FALSE,IS_MANAGED);
        }

        FUNCTION_EXIT;
    }

        
    ConfigAgent::ConfigAgent( const ConfigAgent& agent)
        : m_uniqueIdentifier(--s_nextIdentifier)
    {
        FUNCTION_ENTRY("Copy Constructor");

        m_monitoredProcessEntity = TA_Base_Core::EntityAccessFactory::getInstance().copyEntity(agent.m_monitoredProcessEntity);
        
        if (agent.m_agentEntity != NULL)
        {
            m_agentEntity = TA_Base_Core::EntityAccessFactory::getInstance().copyEntity(agent.m_agentEntity);
        }

        FUNCTION_EXIT;
    }


    TA_Base_Core::ItemChanges ConfigAgent::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        using TA_Base_Core::ItemChanges;

        ItemChanges changes = m_monitoredProcessEntity->getAllItemChanges();

        ItemChanges::iterator matching = changes.find(NAME_FIELD_NAME);
        if (matching != changes.end())
        {
            // Fix up the names so that we don't have MonitoredProcess appended to the end of it
            std::string name = matching->second.oldValue;
            std::string::size_type pos = name.find_last_of(MONITORED_PROCESS_NAME_APPEND);
			if (pos != std::string::npos)
            {
                int stringLength = pos + 1 - MONITORED_PROCESS_NAME_APPEND.length();
                name = name.substr(0,stringLength);

                matching->second.oldValue = name;
            }

            name = matching->second.newValue;
            pos = name.find_last_of(MONITORED_PROCESS_NAME_APPEND);
            if (pos != -1)
            {
                int stringLength = pos + 1 - MONITORED_PROCESS_NAME_APPEND.length();
                name = name.substr(0,stringLength);

                matching->second.newValue = name;
            }
        }

        matching = changes.find(ADDRESS_FIELD_NAME);
        if (matching != changes.end())
        {
            // Change the entry to be 'Executable' instead of address.
            changes.insert( ItemChanges::value_type(EXECUTABLE_FIELD_NAME,matching->second) );
            changes.erase(matching);
        }

        
        if (doesAgentEntityExist())
        {
            ItemChanges agentChanges = m_agentEntity->getAllItemChanges();

            // Now add all of the changes here to the map of changes if they are not duplicates
            for( ItemChanges::iterator iter = agentChanges.begin(); iter != agentChanges.end(); ++iter)
            {
                if( changes.find(iter->first) == changes.end() )
                {
                    // This is a new change so it must be added
                    changes.insert( ItemChanges::value_type(iter->first, iter->second) );
                }
            }
        }

        FUNCTION_EXIT;
        return changes;
    }


    bool ConfigAgent::hasChanged()
    {
        FUNCTION_ENTRY("hasChanged");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        if (m_monitoredProcessEntity->hasChanged())
        {
            FUNCTION_EXIT;
            return true;
        }
        else if( doesAgentEntityExist() && m_agentEntity->hasChanged() )
        {
            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


    bool ConfigAgent::isNew()
    {
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->isNew();
    }


    unsigned long ConfigAgent::getKey()
    {
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");
        return m_monitoredProcessEntity->getKey();
    }


    std::string ConfigAgent::getAgentName()
    {
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        std::string name = m_monitoredProcessEntity->getName();
        
        std::string::size_type pos = name.find_last_of(MONITORED_PROCESS_NAME_APPEND);
		if (pos != std::string::npos)
        {
            int stringLength = pos + 1 - MONITORED_PROCESS_NAME_APPEND.length();
            name = name.substr(0,stringLength);
        }
        return name;
    }


    std::string ConfigAgent::getName()
    {
        return getDescription();
    }


    std::string ConfigAgent::getDescription()
    {
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getDescription();
    }


    std::string ConfigAgent::getAddress()
    {
        TA_ASSERT(doesAgentEntityExist(), "Related entity must exist to get this parameter");

        return m_agentEntity->getAddress();
    }


    unsigned long ConfigAgent::getLocation()
    {
        if (doesAgentEntityExist())
        {
            return m_agentEntity->getLocation();
        }

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getLocation();
    }


    std::string ConfigAgent::getLocationName()
    {

        if (doesAgentEntityExist())
        {
            return m_agentEntity->getLocationName();
        }

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getLocationName();
    }


    unsigned long ConfigAgent::getSubsystem()
    {
        if (doesAgentEntityExist())
        {
            return m_agentEntity->getSubsystem();
        }

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getSubsystem();
    }


	std::string ConfigAgent::getSubsystemName()
    {
        if (doesAgentEntityExist())
        {
            return m_agentEntity->getSubsystemName();
        }
        
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getSubsystemName();
    }


    std::string ConfigAgent::getExecutable()
    {
        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        return m_monitoredProcessEntity->getAddress();
    }


    void ConfigAgent::setDescription(const std::string& description)
    {
        FUNCTION_ENTRY("setDescription");

        if(doesAgentEntityExist())
        {
            m_agentEntity->setDescription(description);
        }

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        m_monitoredProcessEntity->setDescription(description);

        FUNCTION_EXIT;
    }


    void ConfigAgent::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        setDescription(name);
        
        FUNCTION_EXIT;
    }


    void ConfigAgent::setAgentName(const std::string& name)
    {
        FUNCTION_ENTRY("setAgentName");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        std::string mpName;
        std::string agentName;
        if (name.empty())
        {
            mpName = "";
            agentName = "";
        }
        else
        {
            mpName = name + MONITORED_PROCESS_NAME_APPEND;
            agentName = name + AGENT_NAME_APPEND;
        }

        m_monitoredProcessEntity->setName(mpName);

        if (doesAgentEntityExist())
        {
            m_agentEntity->setName(agentName);
        }
        FUNCTION_EXIT;
    }

    
    void ConfigAgent::setLocation(const unsigned long location)
    {
        FUNCTION_ENTRY("setLocation");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        if(doesAgentEntityExist())
        {
            m_agentEntity->setLocation(location);
        }
        m_monitoredProcessEntity->setLocation(location);
        FUNCTION_EXIT;
    }


    void ConfigAgent::setExecutable(const std::string& executable)
    {
        FUNCTION_ENTRY("setExecutable");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        m_monitoredProcessEntity->setAddress(executable);

        FUNCTION_EXIT;
    }


    void ConfigAgent::setAddress(const std::string& address)
    {
        FUNCTION_ENTRY("setAddress");

        TA_ASSERT(doesAgentEntityExist(), "No agent entity exists so cannot set address");

        m_agentEntity->setAddress(address);

        FUNCTION_EXIT;
    }

    
    void ConfigAgent::deleteThisAgent()
    {
        FUNCTION_ENTRY("deleteThisAgent");

        if(doesAgentEntityExist())
        {
            m_agentEntity->deleteThisEntity();
        }

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        m_monitoredProcessEntity->deleteThisEntity();

        FUNCTION_EXIT;
    }


    void ConfigAgent::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        if (m_monitoredProcessEntity->isNew())
        {
            m_monitoredProcessEntity->setParent(0);
        }
      
        try
        {
            m_monitoredProcessEntity->applyChanges();
        }
        catch ( const TA_Base_Core::DataConfigurationException& exception )
        {
            // If one of the fields is 'Address' we must change that to be 'Executable'
            std::vector<std::string> missing = exception.getMissingFields();
            std::vector<std::string>::iterator matching = std::find(missing.begin(), missing.end(), ADDRESS_FIELD_NAME);
            if (matching != missing.end())
            {
                *matching = EXECUTABLE_FIELD_NAME;
                TA_THROW( TA_Base_Core::DataConfigurationException("Caught, fixed field names and now rethrowing",missing) );
            }

            throw;
        }
        catch( const TA_Base_Core::DataException& exception)
        {
            // If one of the fields is 'Address' we must change that to be 'Executable'
            if ( (exception.getFailType() == TA_Base_Core::DataException::MISSING_MANDATORY) ||
                 (exception.getFailType() == TA_Base_Core::DataException::WRONG_TYPE) )
            {
                std::string invalidFields = exception.getWhichData();
                std::string::size_type pos = invalidFields.find(ADDRESS_FIELD_NAME);
				if (pos != std::string::npos)
                {
                    invalidFields.replace(pos,ADDRESS_FIELD_NAME.length(),EXECUTABLE_FIELD_NAME);

                    TA_THROW( TA_Base_Core::DataException("Caught, fixed field names and now rethrowing",exception.getFailType(),invalidFields) );                    
                }
            }

            throw;
        }


        if( m_agentEntity != NULL)
        {
            bool isNew = m_agentEntity->isNew();

            // Create a try-catch block around this code, because if there is a problem writing the agent
            // entity need to "undo" the write of the monitored process entity.
            try
            {
                // setParent can throw an exception, so put it in here...
                if (isNew)
                {
                    unsigned long key = m_monitoredProcessEntity->getKey();
                    m_agentEntity->setParent( key );
                }
                m_agentEntity->applyChanges();
            }
            catch(...)
            {
                if (isNew)
                {
                    m_monitoredProcessEntity->deleteAndResetEntity();
                    m_agentEntity->resetEntity();
                }

                throw;
            }
            
            if (isNew)
            {
                // Set agent key to the same as the pkey. This has to be done after the first apply
                unsigned long agentKey = m_agentEntity->getKey();
                m_agentEntity->setAgent( agentKey );
                m_agentEntity->applyChanges();
            }
        }
        FUNCTION_EXIT;
    }


    void ConfigAgent::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        if(doesAgentEntityExist())
        {
            m_agentEntity->invalidate();
        }
        m_monitoredProcessEntity->invalidate();
        
        FUNCTION_EXIT;
    }


    IConfigEntity* ConfigAgent::getAgentEntityFromDB()
    {
        FUNCTION_ENTRY("getAgentEntityFromDB");

        TA_ASSERT(m_monitoredProcessEntity != NULL, "The monitored process entity is NULL");

        IConfigEntity* relatedEntity = NULL;

        // Get the related entity by retrieving the children of the monitored process entity
        TA_Base_Core::IEntityDataList childEntities = EntityAccessFactory::getInstance().getChildEntitiesOf( m_monitoredProcessEntity->getKey(),true );

        if (childEntities.size() == 0)
        {
            // There is no agent entity
            FUNCTION_EXIT;
            return relatedEntity;
        }
        else if (childEntities.size() > 1)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Entity with key %lu has more than one child. The first child will be used.", m_monitoredProcessEntity->getKey());
            
            // We will use the first entity and delete the rest
            for ( int i = 1; i < childEntities.size(); ++i)
            {
                delete childEntities[i];
                childEntities[i] = NULL;
            }
        }

        FUNCTION_EXIT;
        return dynamic_cast<IConfigEntity*>(childEntities[0]);
    }



    bool ConfigAgent::doesAgentEntityExist()
    {
        return (m_agentEntity != NULL);
    }


} //close namespace TA_Base_Core

