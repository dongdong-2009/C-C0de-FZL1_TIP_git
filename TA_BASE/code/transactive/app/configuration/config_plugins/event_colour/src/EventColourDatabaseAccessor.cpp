/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourDatabaseAccessor.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve EventColour information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

// EventColourDatabaseAccessor.cpp: implementation of the EventColourDatabaseAccessor class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigEventColourData.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include "core/data_access_interface/src/EventColourAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	EventColourDatabaseAccessor::EventColourDatabaseAccessor()
		: m_isLoaded(false)
	{
	}

	EventColourDatabaseAccessor::~EventColourDatabaseAccessor()
	{
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedEventColours::iterator iter = m_eventColours.begin(); iter != m_eventColours.end(); ++iter)
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

	
	void EventColourDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

		// need to always reload the items as it is dependent on AlarmSeverity's name
		// currently there is no framework/design to inform diff plugins abt changes/updates
		// in future, may be improve the config editor framework to allow diff plug-ins to
		// inform each other of data changes
		if (m_isLoaded) m_isLoaded = false;

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the EventColours and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedEventColours::iterator eventColourToDelete = m_eventColours.begin(); eventColourToDelete != m_eventColours.end(); ++eventColourToDelete)
        {
            if( (eventColourToDelete->second) != NULL )
            {
                delete eventColourToDelete->second;
                eventColourToDelete->second = NULL;
            }
        }
        m_eventColours.clear();


        // Now reload all the EventColours
        std::vector<TA_Base_Core::IEventColourData*> eventColours = TA_Base_Core::EventColourAccessFactory::getInstance().getAllEventColours( true);

        // And insert the eventColours into our map. At the same time cast them to IConfigEventColourData*
        // instead of IEventColourData*.
        for (std::vector<TA_Base_Core::IEventColourData*>::iterator iter = eventColours.begin();
             iter != eventColours.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigEventColourData* castEventColour = dynamic_cast<TA_Base_Core::IConfigEventColourData*>(*iter);
             if( castEventColour != NULL )
             {
                m_eventColours.insert( LoadedEventColours::value_type(castEventColour->getUniqueIdentifier(), castEventColour) );
             }
        }

        m_isLoaded = true;
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> EventColourDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of operators times by 2. This is because we want to step for each
        // EventColour while it is loaded and then again while each EventColour is added to the list.
        progress.SetRange( 0, m_eventColours.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedEventColours::iterator iter = m_eventColours.begin();
             iter != m_eventColours.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                std::string name = iter->second->getName();

                names.insert( std::multimap<CString, unsigned long>::value_type( name.c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* EventColourDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the EventColour matching the key passed in.
        LoadedEventColours::iterator matching = m_eventColours.find(key);
        if (matching == m_eventColours.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the EventColour with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void EventColourDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Event Colours cannot be deleted.

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* EventColourDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");
        FUNCTION_EXIT;

        // Event Colours cannot be created.
        return NULL;

    }


    TA_Base_Core::IConfigItem* EventColourDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");
        FUNCTION_EXIT;

        // Event Colours cannot be copied.
        return NULL;
    }


    bool EventColourDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& eventColoursNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(eventColoursNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedEventColours::iterator iter = m_eventColours.begin(); iter != m_eventColours.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    eventColoursNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this EventColour that has not had it's changes applied yet");
                    eventColoursNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this EventColour that has not had it's changes applied yet");
                    eventColoursNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !eventColoursNotApplied.empty();
    }

} //close namespace