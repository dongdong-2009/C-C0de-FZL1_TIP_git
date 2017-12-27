/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/location/src/LocationDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Location information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/location/src/StdAfx.h"
#include "app/configuration/config_plugins/location/src/LocationDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigLocation.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    LocationDatabaseAccessor::LocationDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    LocationDatabaseAccessor::~LocationDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedLocations::iterator iter = m_locations.begin(); iter != m_locations.end(); ++iter)
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


    void LocationDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the locations and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedLocations::iterator locationToDelete = m_locations.begin(); locationToDelete != m_locations.end(); ++locationToDelete)
        {
            if( (locationToDelete->second) != NULL )
            {
                delete locationToDelete->second;
                locationToDelete->second = NULL;
            }
        }
        m_locations.clear();


        // Now reload all the locations
        std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations( true);

        // And insert the locations into our map. At the same time cast them to IConfigLocation*
        // instead of ILocationData*.
        for (std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin();
             iter != locations.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigLocation* castLocation = dynamic_cast<TA_Base_Core::IConfigLocation*>(*iter);
             if( castLocation != NULL )
             {
                m_locations.insert( LoadedLocations::value_type(castLocation->getUniqueIdentifier(), castLocation) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> LocationDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_locations.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedLocations::iterator iter = m_locations.begin();
             iter != m_locations.end();
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


    TA_Base_Core::IConfigItem* LocationDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Location matching the key passed in.
        LoadedLocations::iterator matching = m_locations.find(key);
        if (matching == m_locations.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Location with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void LocationDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Location matching the key passed in.
        LoadedLocations::iterator matching = m_locations.find(key);
        if (matching == m_locations.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Location with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Location was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisLocation();
        }
        delete matching->second;
        matching->second = NULL;
        m_locations.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* LocationDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigLocation* newLocation = TA_Base_Core::LocationAccessFactory::getInstance().createLocation();

        m_locations.insert( LoadedLocations::value_type( newLocation->getUniqueIdentifier(), newLocation ) );

        return newLocation;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* LocationDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigLocation* newLocation = NULL;

        // Attempt to find the Location matching the key passed in.
        LoadedLocations::iterator matching = m_locations.find(idOfItemToCopy);

        TA_ASSERT(matching != m_locations.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigLocation stored in the map");

        newLocation = TA_Base_Core::LocationAccessFactory::getInstance().copyLocation(matching->second);

        if (newLocation != NULL)
        {
            m_locations.insert( LoadedLocations::value_type( newLocation->getUniqueIdentifier(), newLocation ) );
        }
        
        return newLocation;

        FUNCTION_EXIT;
    }


    bool LocationDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& locationsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(locationsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedLocations::iterator iter = m_locations.begin(); iter != m_locations.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    locationsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Location that has not had it's changes applied yet");
                    locationsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Location that has not had it's changes applied yet");
                    locationsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !locationsNotApplied.empty();
    }
}
