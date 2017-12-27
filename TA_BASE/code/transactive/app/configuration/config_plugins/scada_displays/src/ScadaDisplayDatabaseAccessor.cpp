/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ScadaDisplay information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_displays/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigScadaDisplay.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ScadaDisplayDatabaseAccessor::ScadaDisplayDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    ScadaDisplayDatabaseAccessor::~ScadaDisplayDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedScadaDisplays::iterator iter = m_scadaDisplays.begin(); iter != m_scadaDisplays.end(); ++iter)
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


    void ScadaDisplayDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Scada displays and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedScadaDisplays::iterator scadaDisplayToDelete = m_scadaDisplays.begin(); scadaDisplayToDelete != m_scadaDisplays.end(); ++scadaDisplayToDelete)
        {
            if( (scadaDisplayToDelete->second) != NULL )
            {
                delete scadaDisplayToDelete->second;
                scadaDisplayToDelete->second = NULL;
            }
        }
        m_scadaDisplays.clear();


        // Now reload all the scadaDisplays
        std::vector<TA_Base_Core::IScadaDisplay*> scadaDisplays = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getAllScadaDisplays( true);

        // And insert the Scada displays into our map. At the same time cast them to IConfigScadaDisplay*
        // instead of IScadaDisplayData*.
        for (std::vector<TA_Base_Core::IScadaDisplay*>::iterator iter = scadaDisplays.begin();
             iter != scadaDisplays.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigScadaDisplay* castScadaDisplay = dynamic_cast<TA_Base_Core::IConfigScadaDisplay*>(*iter);
             if( castScadaDisplay != NULL )
             {
                m_scadaDisplays.insert( LoadedScadaDisplays::value_type(castScadaDisplay->getUniqueIdentifier(), castScadaDisplay) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> ScadaDisplayDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of Scada displays times by 2. This is because we want to step for each
        // ScadaDisplay while it is loaded and then again while each ScadaDisplay is added to the list.
        progress.SetRange( 0, m_scadaDisplays.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedScadaDisplays::iterator iter = m_scadaDisplays.begin();
             iter != m_scadaDisplays.end();
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


    TA_Base_Core::IConfigItem* ScadaDisplayDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ScadaDisplay matching the key passed in.
        LoadedScadaDisplays::iterator matching = m_scadaDisplays.find(key);
        if (matching == m_scadaDisplays.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ScadaDisplay with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ScadaDisplayDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ScadaDisplay matching the key passed in.
        LoadedScadaDisplays::iterator matching = m_scadaDisplays.find(key);
        if (matching == m_scadaDisplays.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ScadaDisplay with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ScadaDisplay was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_scadaDisplays.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ScadaDisplayDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigScadaDisplay* newScadaDisplay = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().createScadaDisplay();

        m_scadaDisplays.insert( LoadedScadaDisplays::value_type( newScadaDisplay->getUniqueIdentifier(), newScadaDisplay ) );

        return newScadaDisplay;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ScadaDisplayDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigScadaDisplay* newScadaDisplay = NULL;

        // Attempt to find the ScadaDisplay matching the key passed in.
        LoadedScadaDisplays::iterator matching = m_scadaDisplays.find(idOfItemToCopy);

        TA_ASSERT(matching != m_scadaDisplays.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigScadaDisplay stored in the map");

        newScadaDisplay = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().copyScadaDisplay(matching->second);

        if (newScadaDisplay != NULL)
        {
            m_scadaDisplays.insert( LoadedScadaDisplays::value_type( newScadaDisplay->getUniqueIdentifier(), newScadaDisplay ) );
        }
        
        return newScadaDisplay;

        FUNCTION_EXIT;
    }


    bool ScadaDisplayDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& scadaDisplaysNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(scadaDisplaysNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedScadaDisplays::iterator iter = m_scadaDisplays.begin(); iter != m_scadaDisplays.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    scadaDisplaysNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this ScadaDisplay that has not had it's changes applied yet");
                    scadaDisplaysNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this ScadaDisplay that has not had it's changes applied yet");
                    scadaDisplaysNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !scadaDisplaysNotApplied.empty();
    }


    std::map<unsigned long, std::string> ScadaDisplayDatabaseAccessor::getAllLocations()
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
}
