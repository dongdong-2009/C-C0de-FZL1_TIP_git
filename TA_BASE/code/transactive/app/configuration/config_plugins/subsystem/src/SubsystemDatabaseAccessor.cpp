/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/subsystem/src/SubsystemDatabaseAccessor.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Subsystem information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/subsystem/src/StdAfx.h"
#include "app/configuration/config_plugins/subsystem/src/SubsystemDatabaseAccessor.h"

#include <algorithm>
#include <cctype>

#include "core/data_access_interface/src/IConfigSubsystem.h"
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
    static std::string UNALLOCATED = "<unallocated>";

    SubsystemDatabaseAccessor::SubsystemDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    SubsystemDatabaseAccessor::~SubsystemDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedSubsystems::iterator iter = m_subsystems.begin(); iter != m_subsystems.end(); ++iter)
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


    void SubsystemDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the subsystems and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedSubsystems::iterator subsystemToDelete = m_subsystems.begin(); subsystemToDelete != m_subsystems.end(); ++subsystemToDelete)
        {
            if( (subsystemToDelete->second) != NULL )
            {
                delete subsystemToDelete->second;
                subsystemToDelete->second = NULL;
            }
        }
        m_subsystems.clear();


        // Now reload all the subsystems
        std::vector<TA_Base_Core::ISubsystem*> subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems( true);

        // And insert the subsystems into our map. At the same time cast them to IConfigSubsystem*
        // instead of ISubsystemData*.
        for (std::vector<TA_Base_Core::ISubsystem*>::iterator iter = subsystems.begin();
             iter != subsystems.end();
             ++iter
             )
        {
            TA_Base_Core::IConfigSubsystem* castSubsystem = dynamic_cast<TA_Base_Core::IConfigSubsystem*>(*iter);
            if( castSubsystem != NULL )
            {
                std::string name = castSubsystem->getName();
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                if (name == UNALLOCATED)
                {
                    delete castSubsystem;
                    castSubsystem = NULL;
                }
                else
                {
                    m_subsystems.insert( LoadedSubsystems::value_type(castSubsystem->getUniqueIdentifier(), castSubsystem) );
                }
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> SubsystemDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of subsystems times by 2. This is because we want to step for each
        // Subsystem while it is loaded and then again while each Subsystem is added to the list.
        progress.SetRange( 0, m_subsystems.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedSubsystems::iterator iter = m_subsystems.begin();
             iter != m_subsystems.end();
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


    TA_Base_Core::IConfigItem* SubsystemDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Subsystem matching the key passed in.
        LoadedSubsystems::iterator matching = m_subsystems.find(key);
        if (matching == m_subsystems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Subsystem with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void SubsystemDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Subsystem matching the key passed in.
        LoadedSubsystems::iterator matching = m_subsystems.find(key);
        if (matching == m_subsystems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Subsystem with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Subsystem was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisSubsystem();
        }
        delete matching->second;
        matching->second = NULL;
        m_subsystems.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* SubsystemDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigSubsystem* newSubsystem = TA_Base_Core::SubsystemAccessFactory::getInstance().createSubsystem();

        m_subsystems.insert( LoadedSubsystems::value_type( newSubsystem->getUniqueIdentifier(), newSubsystem ) );

        FUNCTION_EXIT;
        return newSubsystem;

        return NULL;
    }


    TA_Base_Core::IConfigItem* SubsystemDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigSubsystem* newSubsystem = NULL;

        // Attempt to find the Subsystem matching the key passed in.
        LoadedSubsystems::iterator matching = m_subsystems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_subsystems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigSubsystem stored in the map");

        newSubsystem = TA_Base_Core::SubsystemAccessFactory::getInstance().copySubsystem(matching->second);

        if (newSubsystem != NULL)
        {
            m_subsystems.insert( LoadedSubsystems::value_type( newSubsystem->getUniqueIdentifier(), newSubsystem ) );
        }

        return newSubsystem;

        FUNCTION_EXIT;
    }


    bool SubsystemDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& subsystemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(subsystemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedSubsystems::iterator iter = m_subsystems.begin(); iter != m_subsystems.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    subsystemsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Subsystem that has not had it's changes applied yet");
                    subsystemsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Subsystem that has not had it's changes applied yet");
                    subsystemsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !subsystemsNotApplied.empty();
    }
}
