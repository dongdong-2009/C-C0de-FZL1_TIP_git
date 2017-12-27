/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/ProfileDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve profile information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile/src/StdAfx.h"
#include "app/configuration/config_plugins/profile/src/ProfileDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigProfile.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IProfileGroup.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ProfileGroupAccessFactory.h"
#include "core/data_access_interface/radio/src/IITSIGroup.h"
#include "core/data_access_interface/radio/src/ITSIGroupAccessFactory.h"
#include "core/data_access_interface/pa/src/PaPriorityGroupAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ProfileDatabaseAccessor::ProfileDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    ProfileDatabaseAccessor::~ProfileDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedProfiles::iterator iter = m_profiles.begin(); iter != m_profiles.end(); ++iter)
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


    void ProfileDatabaseAccessor::loadItems()
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
        for (LoadedProfiles::iterator profileToDelete = m_profiles.begin(); profileToDelete != m_profiles.end(); ++profileToDelete)
        {
            if( (profileToDelete->second) != NULL )
            {
                delete profileToDelete->second;
                profileToDelete->second = NULL;
            }
        }
        m_profiles.clear();


        // Now reload all the profiles
        std::vector<TA_Base_Core::IProfile*> profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles( true);

        // And insert the profiles into our map. At the same time cast them to IConfigProfile*
        // instead of IProfileData*.
        for (std::vector<TA_Base_Core::IProfile*>::iterator iter = profiles.begin();
             iter != profiles.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigProfile* castProfile = dynamic_cast<TA_Base_Core::IConfigProfile*>(*iter);
             if( castProfile != NULL )
             {
                m_profiles.insert( LoadedProfiles::value_type(castProfile->getUniqueIdentifier(), castProfile) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> ProfileDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of profiles times by 2. This is because we want to step for each
        // profile while it is loaded and then again while each profile is added to the list.
        progress.SetRange( 0, m_profiles.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedProfiles::iterator iter = m_profiles.begin();
             iter != m_profiles.end();
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


    TA_Base_Core::IConfigItem* ProfileDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the profile matching the key passed in.
        LoadedProfiles::iterator matching = m_profiles.find(key);
        if (matching == m_profiles.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the profile with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ProfileDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Profile matching the key passed in.
        LoadedProfiles::iterator matching = m_profiles.find(key);
        if (matching == m_profiles.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the profile with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored profile was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_profiles.erase(matching);

        FUNCTION_EXIT;
    }


    std::map<unsigned long, std::string> ProfileDatabaseAccessor::getAllAccessControlGroups()
    {
        FUNCTION_ENTRY("getAllAccessControlGroups");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the duties from the database
        std::vector<TA_Base_Core::IProfileGroup*> profileGroups;
        try
        {
             profileGroups = TA_Base_Core::ProfileGroupAccessFactory::getInstance().getAllProfileGroups();
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the profile groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the profile groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the IProfileGroup object when we're done
        for (std::vector<TA_Base_Core::IProfileGroup*>::iterator iter = profileGroups.begin(); iter != profileGroups.end(); ++iter)
        {
            try
            {
                unsigned int key = (*iter)->getKey();
                std::string name = (*iter)->getName();

                namesAndKeys.insert( std::map<unsigned long, std::string>::value_type(key,name) );
            }
            catch(const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the profile group name. It will not be displayed to the user");
            }
            catch(const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the profile group name. It will not be displayed to the user");
            }

            // We're done with this duty type so clean it up
            delete *iter;
            *iter = NULL;
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> ProfileDatabaseAccessor::getAllRadioLoginGroups()
    {
        FUNCTION_ENTRY("getAllRadioLoginGroups");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the duties from the database
        std::vector<TA_Base_Core::IITSIGroup*> radioGroups;
        try
        {
            radioGroups = TA_Base_Core::ITSIGroupAccessFactory::getInstance().getAllITSIGroups();
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the radio login groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the radio login groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the IITSIGroup object when we're done
        for (std::vector<TA_Base_Core::IITSIGroup*>::iterator iter = radioGroups.begin(); iter != radioGroups.end(); ++iter)
        {
            try
            {
                unsigned int key = (*iter)->getKey();
                std::string name = (*iter)->getName();

                namesAndKeys.insert( std::map<unsigned long, std::string>::value_type(key,name) );
            }
            catch(const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the radio login group name. It will not be displayed to the user");
            }
            catch(const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the radio login group name. It will not be displayed to the user");
            }

            // We're done with this duty type so clean it up
            delete *iter;
            *iter = NULL;
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> ProfileDatabaseAccessor::getAllPaPriorityGroups()
    {
        FUNCTION_ENTRY("getAllPaPriorityGroups");

        std::map<unsigned long, std::string> namesAndKeys;

        try
        {
            TA_Base_Core::PaPriorityGroupAccessFactory::getInstance().getAllPaPriorityGroupNames( namesAndKeys );
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the radio login groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the radio login groups. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    TA_Base_Core::IConfigItem* ProfileDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigProfile* newProfile = TA_Base_Core::ProfileAccessFactory::getInstance().createProfile();

        m_profiles.insert( LoadedProfiles::value_type( newProfile->getUniqueIdentifier(), newProfile ) );

        return newProfile;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ProfileDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigProfile* newProfile = NULL;

        // Attempt to find the Profile matching the key passed in.
        LoadedProfiles::iterator matching = m_profiles.find(idOfItemToCopy);

        TA_ASSERT(matching != m_profiles.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigProfile stored in the map");

        newProfile = TA_Base_Core::ProfileAccessFactory::getInstance().copyProfile(matching->second);

        if (newProfile != NULL)
        {
            m_profiles.insert( LoadedProfiles::value_type( newProfile->getUniqueIdentifier(), newProfile ) );
        }
        
        return newProfile;

        FUNCTION_EXIT;
    }


    bool ProfileDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& profilesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(profilesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedProfiles::iterator iter = m_profiles.begin(); iter != m_profiles.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    profilesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this profile that has not had it's changes applied yet");
                    profilesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this profile that has not had it's changes applied yet");
                    profilesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !profilesNotApplied.empty();
    }


    std::vector<std::string> ProfileDatabaseAccessor::getAllDistinctParameterNames()
    {
        FUNCTION_ENTRY("getAllDistinctParameterNames");
        FUNCTION_EXIT;
        return TA_Base_Core::ProfileAccessFactory::getInstance().getAllDistinctParameterNames();
    }
}
