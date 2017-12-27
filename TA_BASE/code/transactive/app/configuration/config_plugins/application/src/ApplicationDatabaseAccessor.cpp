/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve application information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/application/src/StdAfx.h"
#include "app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigGui.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"

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
    ApplicationDatabaseAccessor::ApplicationDatabaseAccessor(const std::vector<std::string>& entityTypes)
        : m_isLoaded(false), m_entityTypes(entityTypes)
    {
    }


    ApplicationDatabaseAccessor::~ApplicationDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedApplications::iterator iter = m_applications.begin(); iter != m_applications.end(); ++iter)
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


    void ApplicationDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the applications and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedApplications::iterator applicationToDelete = m_applications.begin(); applicationToDelete != m_applications.end(); ++applicationToDelete)
        {
            if( (applicationToDelete->second) != NULL )
            {
                delete applicationToDelete->second;
                applicationToDelete->second = NULL;
            }
        }
        m_applications.clear();


        // Now reload all the applications
        std::vector<TA_Base_Core::IGui*> applications = TA_Base_Core::GuiAccessFactory::getInstance().getAllGuis( true);

        // And insert the applications into our map. At the same time cast them to IConfigApplication*
        // instead of IApplicationData*.
        for (std::vector<TA_Base_Core::IGui*>::iterator iter = applications.begin();
             iter != applications.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigGui* castApplication = dynamic_cast<TA_Base_Core::IConfigGui*>(*iter);
             if( castApplication != NULL )
             {
                m_applications.insert( LoadedApplications::value_type(castApplication->getUniqueIdentifier(), castApplication) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> ApplicationDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of applications times by 2. This is because we want to step for each
        // Application while it is loaded and then again while each Application is added to the list.
        progress.SetRange( 0, m_applications.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedApplications::iterator iter = m_applications.begin();
             iter != m_applications.end();
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


    TA_Base_Core::IConfigItem* ApplicationDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Application matching the key passed in.
        LoadedApplications::iterator matching = m_applications.find(key);
        if (matching == m_applications.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Application with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ApplicationDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Application matching the key passed in.
        LoadedApplications::iterator matching = m_applications.find(key);
        if (matching == m_applications.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Application with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Application was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisGui();
        }
        delete matching->second;
        matching->second = NULL;
        m_applications.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ApplicationDatabaseAccessor::newItem()
    {
        TA_ASSERT(false,"This method should not be called. newItem(type) should be called instead");
        return NULL;
    }


    TA_Base_Core::IConfigItem* ApplicationDatabaseAccessor::newItem(const std::string& type)
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigGui* newApplication = TA_Base_Core::GuiAccessFactory::getInstance().createGui(type);

        m_applications.insert( LoadedApplications::value_type( newApplication->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IConfigGui*>(newApplication) ) );

        return newApplication;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ApplicationDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigGui* newApplication = NULL;

        // Attempt to find the Application matching the key passed in.
        LoadedApplications::iterator matching = m_applications.find(idOfItemToCopy);

        TA_ASSERT(matching != m_applications.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigGui stored in the map");

        newApplication = TA_Base_Core::GuiAccessFactory::getInstance().copyGui(matching->second);

        if (newApplication != NULL)
        {
            m_applications.insert( LoadedApplications::value_type( newApplication->getUniqueIdentifier(), newApplication ) );
        }
        
        return newApplication;

        FUNCTION_EXIT;
    }


    std::map<unsigned long, std::string> ApplicationDatabaseAccessor::getAllLocations()
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


    std::map<unsigned long, std::string> ApplicationDatabaseAccessor::getAllSubsystems()
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

    
    bool ApplicationDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& applicationsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(applicationsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedApplications::iterator iter = m_applications.begin(); iter != m_applications.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    applicationsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Application that has not had it's changes applied yet");
                    applicationsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Application that has not had it's changes applied yet");
                    applicationsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !applicationsNotApplied.empty();
    }
}
