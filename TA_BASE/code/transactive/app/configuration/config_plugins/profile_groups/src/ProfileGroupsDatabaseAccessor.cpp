/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile_groups/src/ProfileGroupsDatabaseAccessor.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve profile group information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigProfileGroup.h"
#include "core/data_access_interface/src/IProfileGroup.h"
#include "core/data_access_interface/src/ProfileGroupAccessFactory.h"
//#include "core/data_access_interface/radio/src/IITSIGroup.h"
//#include "core/data_access_interface/radio/src/ITSIGroupAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ProfileGroupsDatabaseAccessor::ProfileGroupsDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    ProfileGroupsDatabaseAccessor::~ProfileGroupsDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedProfileGroups::iterator iter = m_profileGroups.begin(); iter != m_profileGroups.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
			m_profileGroups.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void ProfileGroupsDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the profiles and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedProfileGroups::iterator profileToDelete = m_profileGroups.begin(); profileToDelete != m_profileGroups.end(); ++profileToDelete)
        {
            if( (profileToDelete->second) != NULL )
            {
                delete profileToDelete->second;
                profileToDelete->second = NULL;
            }
        }
        if (m_profileGroups.size() > 0) m_profileGroups.clear();//if there is something then clear it


        // Now reload all the profile groups
        std::vector<TA_Base_Core::IProfileGroup*> profileGroups = TA_Base_Core::ProfileGroupAccessFactory::getInstance().getAllProfileGroups(true);

        // And insert the profiles into our map. At the same time cast them to IConfigProfile*
        // instead of IProfileData*.
        for (std::vector<TA_Base_Core::IProfileGroup*>::iterator iter = profileGroups.begin();
             iter != profileGroups.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigProfileGroup* castProfileGp = dynamic_cast<TA_Base_Core::IConfigProfileGroup*>(*iter);
             if( castProfileGp != NULL )
             {
                m_profileGroups.insert( LoadedProfileGroups::value_type(castProfileGp->getUniqueIdentifier(), castProfileGp) );
             }
        }

        if (m_profileGroups.size() > 0) m_isLoaded = true; // may need to try again as there is no profile groups in the database which is impossible
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> ProfileGroupsDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of profiles times by 2. This is because we want to step for each
        // profile while it is loaded and then again while each profile is added to the list.
        progress.SetRange( 0, m_profileGroups.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedProfileGroups::iterator iter = m_profileGroups.begin();
             iter != m_profileGroups.end();
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


    TA_Base_Core::IConfigItem* ProfileGroupsDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the profile matching the key passed in.
        LoadedProfileGroups::iterator matching = m_profileGroups.find(key);
        if (matching == m_profileGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the profile group with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ProfileGroupsDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Profile matching the key passed in.
        LoadedProfileGroups::iterator matching = m_profileGroups.find(key);
        if (matching == m_profileGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the profile group with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored profile group was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_profileGroups.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ProfileGroupsDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigProfileGroup* newProfileGp = TA_Base_Core::ProfileGroupAccessFactory::getInstance().createProfileGroup();

        m_profileGroups.insert( LoadedProfileGroups::value_type( newProfileGp->getUniqueIdentifier(), newProfileGp ) );

        return newProfileGp;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ProfileGroupsDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigProfileGroup* newProfileGp = NULL;

        // Attempt to find the Profile matching the key passed in.
        LoadedProfileGroups::iterator matching = m_profileGroups.find(idOfItemToCopy);

        TA_ASSERT(matching != m_profileGroups.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigProfileGroup stored in the map");

        newProfileGp = TA_Base_Core::ProfileGroupAccessFactory::getInstance().copyProfileGroup(matching->second);

        if (newProfileGp != NULL)
        {
            m_profileGroups.insert( LoadedProfileGroups::value_type( newProfileGp->getUniqueIdentifier(), newProfileGp ) );
        }
        
        return newProfileGp;

        FUNCTION_EXIT;
    }


    bool ProfileGroupsDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& profileGroupsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(profileGroupsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedProfileGroups::iterator iter = m_profileGroups.begin(); iter != m_profileGroups.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    profileGroupsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this profile group that has not had it's changes applied yet");
                    profileGroupsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this profile group that has not had it's changes applied yet");
                    profileGroupsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;

        return !profileGroupsNotApplied.empty();
    }

}
