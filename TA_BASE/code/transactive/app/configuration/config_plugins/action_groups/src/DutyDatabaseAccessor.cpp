/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/DutyDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Duty information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/action_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/action_groups/src/DutyDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigActionGroup.h"
#include "core/data_access_interface/src/IActionGroup.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DutyDatabaseAccessor::DutyDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    DutyDatabaseAccessor::~DutyDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedDuties::iterator iter = m_duties.begin(); iter != m_duties.end(); ++iter)
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


    void DutyDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Duties and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

        // Now reload all the Duties
        std::vector<TA_Base_Core::IActionGroup*> duties = TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroups( true);

        // And insert the duties into our map. At the same time cast them to IConfigActionGroup*
        // instead of IActionGroupData*.
        for (std::vector<TA_Base_Core::IActionGroup*>::iterator iter = duties.begin();
             iter != duties.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigActionGroup* castDuty = dynamic_cast<TA_Base_Core::IConfigActionGroup*>(*iter);
             if( castDuty != NULL )
             {
				 LoadedDuties::iterator matching = m_duties.find(castDuty->getUniqueIdentifier());
				 if (matching == m_duties.end() )
				 {
					 m_duties.insert( LoadedDuties::value_type(castDuty->getUniqueIdentifier(), castDuty) );
				 }
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> DutyDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of duties times by 2. This is because we want to step for each
        // Duty while it is loaded and then again while each Duty is added to the list.
        progress.SetRange( 0, m_duties.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedDuties::iterator iter = m_duties.begin();
             iter != m_duties.end();
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


    TA_Base_Core::IConfigItem* DutyDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Duty matching the key passed in.
        LoadedDuties::iterator matching = m_duties.find(key);
        if (matching == m_duties.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Duty with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void DutyDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Duty matching the key passed in.
        LoadedDuties::iterator matching = m_duties.find(key);
        if (matching == m_duties.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Duty with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Duty was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisActionGroup();
        }
        delete matching->second;
        matching->second = NULL;
        m_duties.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* DutyDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigActionGroup* newDuty = TA_Base_Core::ActionGroupAccessFactory::getInstance().createActionGroup();

        m_duties.insert( LoadedDuties::value_type( newDuty->getUniqueIdentifier(), newDuty ) );

        return newDuty;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* DutyDatabaseAccessor::copyItem(const unsigned long idOfDutyToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigActionGroup* newDuty = NULL;

        // Attempt to find the Duty matching the key passed in.
        LoadedDuties::iterator matching = m_duties.find(idOfDutyToCopy);

        TA_ASSERT(matching != m_duties.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigDuty stored in the map");

        newDuty = TA_Base_Core::ActionGroupAccessFactory::getInstance().copyActionGroup(matching->second);

        if (newDuty != NULL)
        {
            m_duties.insert( LoadedDuties::value_type( newDuty->getUniqueIdentifier(), newDuty ) );
        }
        
        return newDuty;

        FUNCTION_EXIT;
    }


    bool DutyDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& dutiesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(dutiesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedDuties::iterator iter = m_duties.begin(); iter != m_duties.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    dutiesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Duty that has not had it's changes applied yet");
                    dutiesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Duty that has not had it's changes applied yet");
                    dutiesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !dutiesNotApplied.empty();
    }
}
