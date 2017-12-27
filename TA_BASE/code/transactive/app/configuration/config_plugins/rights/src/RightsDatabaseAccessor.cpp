/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve rights information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/rights/src/StdAfx.h"
#include "app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h"

#include <algorithm>
#include <cctype>

#include "core/data_access_interface/src/IConfigRights.h"
#include "core/data_access_interface/src/RightsAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/ISubsystemState.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/SubsystemStateAccessFactory.h"
#include "core/data_access_interface/src/IActionGroup.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    static std::string UNALLOCATED = "<unallocated>";

    RightsDatabaseAccessor::RightsDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    RightsDatabaseAccessor::~RightsDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedRights::iterator iter = m_applicationRights.begin(); iter != m_applicationRights.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }

            for (LoadedRights::iterator  iter = m_subsystemRights.begin(); iter != m_subsystemRights.end(); ++iter)
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


    void RightsDatabaseAccessor::loadItems(unsigned long subsystemStateKey, const RightsType& type)
    {
        FUNCTION_ENTRY("loadItems");

        //if (m_isLoaded)
        //{
            // This means the data is already loaded so we just ignore it.
        //    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the rights and this is not a refresh so we exit");
        //    FUNCTION_EXIT;
        //    return;
        //}

       
        // If we get to here this means we are loading the data fresh

        if(type == APPLICATION)
        {
            // We must clean up all the database objects we previously loaded in case we are doing a refresh
            for (LoadedRights::iterator rightsToDelete = m_applicationRights.begin(); rightsToDelete != m_applicationRights.end(); ++rightsToDelete)
            {
                if( (rightsToDelete->second) != NULL )
                {
                    delete rightsToDelete->second;
                    rightsToDelete->second = NULL;
                }
            }

            m_applicationRights.clear();

             // Now reload all the rights for Application Grid
            std::vector<TA_Base_Core::IRights*> rights = TA_Base_Core::RightsAccessFactory::getInstance().getAllRights( true);

            // And insert the rights into our map. At the same time cast them to IConfigRights*
            // instead of IRightsData*.
            std::vector<TA_Base_Core::IRights*>::iterator iter;
            for (iter = rights.begin(); iter != rights.end(); ++iter )
            {
                 TA_Base_Core::IConfigRights* castRights = dynamic_cast<TA_Base_Core::IConfigRights*>(*iter);
                 if( castRights != NULL )
                 {
                     //if (castRights->isPhysicalSubsystem())
                     //{
                     //    m_subsystemRights.insert( LoadedRights::value_type(castRights->getUniqueIdentifier(), castRights) );
                     //}
                     if (!castRights->isPhysicalSubsystem())
                     {
                         m_applicationRights.insert( LoadedRights::value_type(castRights->getUniqueIdentifier(), castRights) );
                     }
                 }
            }
        }
        else if(type == SUBSYSTEM)
        {

            for (LoadedRights::iterator rightsToDelete = m_subsystemRights.begin(); rightsToDelete != m_subsystemRights.end(); ++rightsToDelete)
            {
                if( (rightsToDelete->second) != NULL )
                {
                    delete rightsToDelete->second;
                    rightsToDelete->second = NULL;
                }
            }

            m_subsystemRights.clear();

             // Now reload all the rights for Subsystem Grid
            std::vector<TA_Base_Core::IRights*> rights = TA_Base_Core::RightsAccessFactory::getInstance().getRights( subsystemStateKey, true);

            // And insert the rights into our map. At the same time cast them to IConfigRights*
            // instead of IRightsData*.
            std::vector<TA_Base_Core::IRights*>::iterator iter = rights.begin();
            for (; iter != rights.end(); ++iter )
            {
                 TA_Base_Core::IConfigRights* castRights = dynamic_cast<TA_Base_Core::IConfigRights*>(*iter);
                 if( castRights != NULL )
                 {
                     if (castRights->isPhysicalSubsystem())
                     {
                         m_subsystemRights.insert( LoadedRights::value_type(castRights->getUniqueIdentifier(), castRights) );
                     }
                 }
            }
        }
        else
        {
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    RightsDatabaseAccessor::LoadedRights RightsDatabaseAccessor::getAllRights()
    {
        FUNCTION_ENTRY("getAllRights");

        LoadedRights rights;

        std::copy(m_applicationRights.begin(), m_applicationRights.end(), std::inserter(rights, rights.begin()));
        std::copy(m_subsystemRights.begin(), m_subsystemRights.end(), std::inserter(rights, rights.begin()));

        FUNCTION_EXIT;
        return rights;
    }


    TA_Base_Core::IConfigRights* RightsDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the rights matching the key passed in.
        LoadedRights::iterator matching = m_applicationRights.find(key);
        if (matching != m_applicationRights.end() )
        {
            FUNCTION_EXIT;
            return matching->second;
        }

        matching = m_subsystemRights.find(key);
        if (matching == m_subsystemRights.end())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the rights with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    TA_Base_Core::IConfigRights* RightsDatabaseAccessor::newItem(unsigned long profileKey, unsigned long subsystemKey, 
		unsigned long subsystemStateKey, RightsType type)
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigRights* newRights = TA_Base_Core::RightsAccessFactory::getInstance().createRights(profileKey, subsystemKey, subsystemStateKey);

        switch(type)
        {
            case(APPLICATION):
                m_applicationRights.insert( LoadedRights::value_type( newRights->getUniqueIdentifier(), newRights) );
                break;
            case(SUBSYSTEM):
                m_subsystemRights.insert( LoadedRights::value_type( newRights->getUniqueIdentifier(), newRights) );
                break;
            default:
                TA_ASSERT(false, "Wrong rights type passed in. Must be APPLICATION or SUBSYSTEM");
        }

        return newRights;

        FUNCTION_EXIT;
    }


    void RightsDatabaseAccessor::removeItem(unsigned long uniqueIdentifer)
    {
        FUNCTION_ENTRY("removeItem");

        LoadedRights::iterator iter = m_subsystemRights.find(uniqueIdentifer);
        if(iter != m_subsystemRights.end())
        {
            m_subsystemRights.erase(iter);
            FUNCTION_EXIT;
            return;
        }
        
        iter = m_applicationRights.find(uniqueIdentifer);
        if(iter != m_applicationRights.end())
        {
            m_applicationRights.erase(iter);
            FUNCTION_EXIT;
            return;
        }
    }


    bool RightsDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& rightsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(rightsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        bool haveChangesBeenMade = false;

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedRights::iterator iter = m_applicationRights.begin(); iter != m_applicationRights.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                haveChangesBeenMade = true;
                break;
            }
        }

        // If we still haven't found any changes then go through the other list of rights
        if (!haveChangesBeenMade)
        {
            for (LoadedRights::iterator iter = m_subsystemRights.begin(); iter != m_subsystemRights.end();  ++iter)
            {
                if( (iter->second != NULL) && iter->second->hasChanged() )
                {
                    haveChangesBeenMade = true;
                    break;
                }
            }
        }


        if (haveChangesBeenMade)
        {
            // We push back a blank entry because it is this component overall that has changed not an individual
            // part of it.
            rightsNotApplied.push_back( "" );
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;

        return haveChangesBeenMade;
    }


    std::map<unsigned long, RightsDatabaseAccessor::ProfileDetails> RightsDatabaseAccessor::getAllProfiles()
    {
        FUNCTION_ENTRY("getAllProfiles");

        using TA_Base_Core::IProfile;

        std::map<unsigned long, ProfileDetails> namesAndKeys;

        // First retrieve all the locations from the database
        std::vector<IProfile*> profiles;
        try
        {
            profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles();
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
        // 2) Delete the IProfile object when we're done
        for (std::vector<IProfile*>::iterator iter = profiles.begin(); iter != profiles.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    ProfileDetails details;
                    details.name = (*iter)->getName();
                    details.typeKey = (*iter)->getType();
                    namesAndKeys.insert( std::map<unsigned long, ProfileDetails>::value_type( (*iter)->getKey(), details ) );
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


    std::map<std::string, unsigned long> RightsDatabaseAccessor::getAllSubsystems(RightsType type)
    {
        FUNCTION_ENTRY("getAllSubsystems");
        std::map<std::string, unsigned long> namesAndKeys;

        using TA_Base_Core::ISubsystem;

        // First retrieve all the subsystems from the database
        std::vector<ISubsystem*> subsystems;
        try
        {
            subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems();
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
                    std::string name((*iter)->getName());
                    std::string lowname(name);
                    std::transform(lowname.begin(), lowname.end(), lowname.begin(), ::tolower);
                    switch (type)
                    {
                        case(APPLICATION):
                            // We want to insert the subsystem if it is not a physical one
                            if ( !(*iter)->isPhysical() && lowname != UNALLOCATED)
                            {
                                namesAndKeys.insert( std::map<std::string, unsigned long>::value_type( name,(*iter)->getKey() ) );
                            }
                            break;

                        case(SUBSYSTEM):
                            // We want to insert the subsystem if it is a physical one
                            if ( (*iter)->isPhysical() && lowname != UNALLOCATED)
                            {
                                namesAndKeys.insert( std::map<std::string, unsigned long>::value_type( name, (*iter)->getKey() ) );
                            }
                            break;

                        case(ALL):
                            namesAndKeys.insert( std::map<std::string, unsigned long>::value_type( name, (*iter)->getKey() ) );
                            break;
                    }
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


    std::map<unsigned long, std::string> RightsDatabaseAccessor::getAllDuties()
    {
        FUNCTION_ENTRY("getAllDuties");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the duties from the database
        std::vector<TA_Base_Core::IActionGroup*> duties;
        try
        {
             duties = TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroups();
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the duty types. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the duty types. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }


        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the IActionGroup object when we're done
        for (std::vector<TA_Base_Core::IActionGroup*>::iterator iter = duties.begin(); iter != duties.end(); ++iter)
        {
            try
            {
                unsigned int key = (*iter)->getKey();
                std::string name = (*iter)->getName();

                namesAndKeys.insert( std::map<unsigned long, std::string>::value_type(key,name) );
            }
            catch(const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the duty type name. It will not be displayed to the user");
            }
            catch(const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the duty type name. It will not be displayed to the user");
            }

            // We're done with this duty type so clean it up
            delete *iter;
            *iter = NULL;
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> RightsDatabaseAccessor::getAllSubsystemStates()
    {
        FUNCTION_ENTRY("getAllSubsystemStates");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the duties from the database
        std::vector<TA_Base_Core::ISubsystemState*> subsystemStates;
        try
        {
             subsystemStates = TA_Base_Core::SubsystemStateAccessFactory::getInstance().getAllSubsystemStates();
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the duty types. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the duty types. None will be displayed to the user");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the ISubsystemState object when we're done
        for (std::vector<TA_Base_Core::ISubsystemState*>::iterator iter = subsystemStates.begin(); iter != subsystemStates.end(); ++iter)
        {
            try
            {
                unsigned int key = (*iter)->getKey();
                std::string name = (*iter)->getName();

                namesAndKeys.insert( std::map<unsigned long, std::string>::value_type(key,name) );
            }
            catch(const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the duty type name. It will not be displayed to the user");
            }
            catch(const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the duty type name. It will not be displayed to the user");
            }

            // We're done with this duty type so clean it up
            delete *iter;
            *iter = NULL;
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }

}
