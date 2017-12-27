/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller_groups/src/SCGroupDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve SC group information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  * SC = System Controller
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller_groups/src/SCGroupDatabaseAccessor.h"

#include "core/data_access_interface/system_controller/src/IConfigGroupMonitor.h"
#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    SCGroupDatabaseAccessor::SCGroupDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    SCGroupDatabaseAccessor::~SCGroupDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedSCGroups::iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter)
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


    void SCGroupDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the SC groups and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedSCGroups::iterator groupToDelete = m_groups.begin(); groupToDelete != m_groups.end(); ++groupToDelete)
        {
            if( (groupToDelete->second) != NULL )
            {
                delete groupToDelete->second;
                groupToDelete->second = NULL;
            }
        }
        m_groups.clear();


        // Now reload all the SC groups
        std::vector<TA_Base_Core::IConfigGroupMonitor*> groups = TA_Base_Core::GroupMonitorAccessFactory::getInstance().getAllMonitorGroups();

        // And insert the SC groups into our map. At the same time cast them to IConfigGroupMonitor*
        // instead of ISCGroupData*.
        for (std::vector<TA_Base_Core::IConfigGroupMonitor*>::iterator iter = groups.begin();
             iter != groups.end();
             ++iter
             )
        {
            m_groups.insert( LoadedSCGroups::value_type((*iter)->getUniqueIdentifier(), (*iter)) );
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> SCGroupDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of SC groups times by 2. This is because we want to step for each
        // SC group while it is loaded and then again while each SC group is added to the list.
        progress.SetRange( 0, m_groups.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedSCGroups::iterator iter = m_groups.begin();
             iter != m_groups.end();
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


    TA_Base_Core::IConfigItem* SCGroupDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the SC group matching the key passed in.
        LoadedSCGroups::iterator matching = m_groups.find(key);
        if (matching == m_groups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the SC group with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void SCGroupDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the SC group matching the key passed in.
        LoadedSCGroups::iterator matching = m_groups.find(key);
        if (matching == m_groups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the SC group with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored SC group was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisGroup();
        }
        delete matching->second;
        matching->second = NULL;
        m_groups.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* SCGroupDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigGroupMonitor* newSCGroup = TA_Base_Core::GroupMonitorAccessFactory::getInstance().createGroup();

        m_groups.insert( LoadedSCGroups::value_type( newSCGroup->getUniqueIdentifier(), newSCGroup ) );

        return newSCGroup;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* SCGroupDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigGroupMonitor* newSCGroup = NULL;

        // Attempt to find the SC group matching the key passed in.
        LoadedSCGroups::iterator matching = m_groups.find(idOfItemToCopy);

        TA_ASSERT(matching != m_groups.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigSCGroup stored in the map");

        newSCGroup = TA_Base_Core::GroupMonitorAccessFactory::getInstance().copyGroup(matching->second);

        if (newSCGroup != NULL)
        {
            m_groups.insert( LoadedSCGroups::value_type( newSCGroup->getUniqueIdentifier(), newSCGroup ) );
        }
        
        return newSCGroup;

        FUNCTION_EXIT;
    }


    bool SCGroupDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& groupsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(groupsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedSCGroups::iterator iter = m_groups.begin(); iter != m_groups.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    groupsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this SC group that has not had it's changes applied yet");
                    groupsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this SC group that has not had it's changes applied yet");
                    groupsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !groupsNotApplied.empty();
    }
}
