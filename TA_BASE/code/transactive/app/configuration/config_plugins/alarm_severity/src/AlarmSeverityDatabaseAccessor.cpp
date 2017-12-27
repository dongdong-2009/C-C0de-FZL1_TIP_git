/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_severity/src/AlarmSeverityDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve AlarmSeverity information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_severity/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_severity/src/AlarmSeverityDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigAlarmSeverityData.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    AlarmSeverityDatabaseAccessor::AlarmSeverityDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    AlarmSeverityDatabaseAccessor::~AlarmSeverityDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAlarmSeverities::iterator iter = m_alarmSeverities.begin(); iter != m_alarmSeverities.end(); ++iter)
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


    void AlarmSeverityDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the AlarmSeverities and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAlarmSeverities::iterator alarmSeverityToDelete = m_alarmSeverities.begin(); alarmSeverityToDelete != m_alarmSeverities.end(); ++alarmSeverityToDelete)
        {
            if( (alarmSeverityToDelete->second) != NULL )
            {
                delete alarmSeverityToDelete->second;
                alarmSeverityToDelete->second = NULL;
            }
        }
        m_alarmSeverities.clear();


        // Now reload all the AlarmSeverities
        std::vector<TA_Base_Core::IAlarmSeverityData*> alarmSeverities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities( true);

        // And insert the alarmSeverities into our map. At the same time cast them to IConfigAlarmSeverityData*
        // instead of IAlarmSeverityData*.
        for (std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = alarmSeverities.begin();
             iter != alarmSeverities.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigAlarmSeverityData* castAlarmSeverity = dynamic_cast<TA_Base_Core::IConfigAlarmSeverityData*>(*iter);
             if( castAlarmSeverity != NULL )
             {
                m_alarmSeverities.insert( LoadedAlarmSeverities::value_type(castAlarmSeverity->getUniqueIdentifier(), castAlarmSeverity) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AlarmSeverityDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of operators times by 2. This is because we want to step for each
        // AlarmSeverity while it is loaded and then again while each AlarmSeverity is added to the list.
        progress.SetRange( 0, m_alarmSeverities.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAlarmSeverities::iterator iter = m_alarmSeverities.begin();
             iter != m_alarmSeverities.end();
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


    TA_Base_Core::IConfigItem* AlarmSeverityDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the AlarmSeverity matching the key passed in.
        LoadedAlarmSeverities::iterator matching = m_alarmSeverities.find(key);
        if (matching == m_alarmSeverities.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the AlarmSeverity with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AlarmSeverityDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Alarm Severities cannot be deleted.

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AlarmSeverityDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");
        FUNCTION_EXIT;

        // Alarm Severities cannot be created.
        return NULL;

    }


    TA_Base_Core::IConfigItem* AlarmSeverityDatabaseAccessor::copyItem(const unsigned long idOfAlarmSeverityToCopy)
    {
        FUNCTION_ENTRY("copyItem");
        FUNCTION_EXIT;

        // Alarm Severities cannot be copied.
        return NULL;
    }


    bool AlarmSeverityDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& alarmSeveritiesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(alarmSeveritiesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAlarmSeverities::iterator iter = m_alarmSeverities.begin(); iter != m_alarmSeverities.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    alarmSeveritiesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this AlarmSeverity that has not had it's changes applied yet");
                    alarmSeveritiesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this AlarmSeverity that has not had it's changes applied yet");
                    alarmSeveritiesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !alarmSeveritiesNotApplied.empty();
    }
}
