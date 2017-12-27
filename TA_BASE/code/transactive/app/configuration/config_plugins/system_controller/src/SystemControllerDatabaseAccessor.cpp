/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve system controller information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"

#include "core/data_access_interface/system_controller/src/IConfigSystemController.h"
#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/data_access_interface/system_controller/src/IConfigGroupMonitor.h"
#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const std::string SystemControllerDatabaseAccessor::SERVER_ENTITY_TYPE = "Server";
    const std::string SystemControllerDatabaseAccessor::MANAGED_PROCESS_ENTITY_TYPE = "MonitoredProcess";

    SystemControllerDatabaseAccessor::SystemControllerDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    SystemControllerDatabaseAccessor::~SystemControllerDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedSystemControllers::iterator iter = m_systemControllers.begin(); iter != m_systemControllers.end(); ++iter)
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


    void SystemControllerDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the system controllers and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedSystemControllers::iterator itemToDelete = m_systemControllers.begin(); itemToDelete != m_systemControllers.end(); ++itemToDelete)
        {
            if( (itemToDelete->second) != NULL )
            {
                delete itemToDelete->second;
                itemToDelete->second = NULL;
            }
        }
        m_systemControllers.clear();


        // Now reload all the system controllers
        std::vector<TA_Base_Core::ISystemControllerData*> systemControllers = TA_Base_Core::SystemControllerAccessFactory::getInstance().getAllSystemControllers( true);

        // And insert the system controllers into our map. At the same time cast them to IConfigSystemController*
        // instead of ISystemControllerData*.
        for (std::vector<TA_Base_Core::ISystemControllerData*>::iterator iter = systemControllers.begin();
             iter != systemControllers.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigSystemController* castSystemController = dynamic_cast<TA_Base_Core::IConfigSystemController*>(*iter);
             if( castSystemController != NULL )
             {
                m_systemControllers.insert( LoadedSystemControllers::value_type(castSystemController->getUniqueIdentifier(), castSystemController) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> SystemControllerDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of system controllers times by 2. This is because we want to step for each
        // system controller while it is loaded and then again while each system controller is added to the list.
        progress.SetRange( 0, m_systemControllers.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedSystemControllers::iterator iter = m_systemControllers.begin();
             iter != m_systemControllers.end();
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


    TA_Base_Core::IConfigItem* SystemControllerDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the system controller matching the key passed in.
        LoadedSystemControllers::iterator matching = m_systemControllers.find(key);
        if (matching == m_systemControllers.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the system controller with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void SystemControllerDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the system controller matching the key passed in.
        LoadedSystemControllers::iterator matching = m_systemControllers.find(key);
        if (matching == m_systemControllers.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the system controller with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored system controller was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisSystemController();
        }
        delete matching->second;
        matching->second = NULL;
        m_systemControllers.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* SystemControllerDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigSystemController* newSystemController = TA_Base_Core::SystemControllerAccessFactory::getInstance().createSystemController();

        m_systemControllers.insert( LoadedSystemControllers::value_type( newSystemController->getUniqueIdentifier(), newSystemController ) );

        return newSystemController;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* SystemControllerDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigSystemController* newSystemController = NULL;

        // Attempt to find the system controller matching the key passed in.
        LoadedSystemControllers::iterator matching = m_systemControllers.find(idOfItemToCopy);

        TA_ASSERT(matching != m_systemControllers.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigSystemController stored in the map");

        newSystemController = TA_Base_Core::SystemControllerAccessFactory::getInstance().copySystemController(matching->second);

        if (newSystemController != NULL)
        {
            m_systemControllers.insert( LoadedSystemControllers::value_type( newSystemController->getUniqueIdentifier(), newSystemController ) );
        }
        
        return newSystemController;

        FUNCTION_EXIT;
    }


    bool SystemControllerDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedSystemControllers::iterator iter = m_systemControllers.begin(); iter != m_systemControllers.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    itemsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this system controller that has not had it's changes applied yet");
                    itemsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this system controller that has not had it's changes applied yet");
                    itemsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::map<unsigned long, std::string> SystemControllerDatabaseAccessor::getAllServers()
    {
        FUNCTION_ENTRY("getAllServers");

        using TA_Base_Core::IEntityData;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the goups from the database
        std::vector<IEntityData*> servers;
        try
        {
            servers = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(SERVER_ENTITY_TYPE);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No servers will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No servers will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IEntityData object when we're done
        for (std::vector<IEntityData*>::iterator iter = servers.begin(); iter != servers.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This server will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This server will not be added to the map");
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


    std::map<unsigned long, std::string> SystemControllerDatabaseAccessor::getAllGroups()
    {
        FUNCTION_ENTRY("getAllGroups");

        using TA_Base_Core::IConfigGroupMonitor;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the goups from the database
        std::vector<IConfigGroupMonitor*> groups;
        try
        {
            groups = TA_Base_Core::GroupMonitorAccessFactory::getInstance().getAllMonitorGroups();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No system controller groups will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No system controller groups will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IConfigGroupMonitor object when we're done
        for (std::vector<IConfigGroupMonitor*>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This system controller will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This system controller will not be added to the map");
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


    std::map<unsigned long, std::string> SystemControllerDatabaseAccessor::getAllManagedProcesses()
    {
        FUNCTION_ENTRY("getAllManagedProcesses");

        using TA_Base_Core::IEntityData;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the goups from the database
        std::vector<IEntityData*> servers;
        try
        {
            servers = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(MANAGED_PROCESS_ENTITY_TYPE);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No managed processes will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No managed processes will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IEntityData object when we're done
        for (std::vector<IEntityData*>::iterator iter = servers.begin(); iter != servers.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getDescription() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This managed proces will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This managed process will not be added to the map");
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

}
