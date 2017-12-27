/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/AlarmTypeDatabaseAccessor.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve AlarmType information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "app/configuration/config_plugins/alarm_type/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigAlarmType.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    AlarmTypeDatabaseAccessor::AlarmTypeDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    AlarmTypeDatabaseAccessor::~AlarmTypeDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAlarmTypes::iterator iter = m_alarmTypes.begin(); iter != m_alarmTypes.end(); ++iter)
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


    void AlarmTypeDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the alarm types and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAlarmTypes::iterator alarmTypeToDelete = m_alarmTypes.begin(); alarmTypeToDelete != m_alarmTypes.end(); ++alarmTypeToDelete)
        {
            if( (alarmTypeToDelete->second) != NULL )
            {
                delete alarmTypeToDelete->second;
                alarmTypeToDelete->second = NULL;
            }
        }
        m_alarmTypes.clear();


        // Now reload all the alarm types
        std::vector<TA_Base_Core::IAlarmTypeData*> alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes( true);

        // And insert the alarm types into our map. At the same time cast them to IConfigAlarmType*
        // instead of IAlarmTypeData*.
        for (std::vector<TA_Base_Core::IAlarmTypeData*>::iterator iter = alarmTypes.begin();
             iter != alarmTypes.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigAlarmType* castAlarmType = dynamic_cast<TA_Base_Core::IConfigAlarmType*>(*iter);
             if( castAlarmType != NULL && castAlarmType->isVisible() )
             {
                m_alarmTypes.insert( LoadedAlarmTypes::value_type(castAlarmType->getUniqueIdentifier(), castAlarmType) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AlarmTypeDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of alarm types times by 2. This is because we want to step for each
        // AlarmType while it is loaded and then again while each AlarmType is added to the list.
        progress.SetRange( 0, m_alarmTypes.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAlarmTypes::iterator iter = m_alarmTypes.begin();
             iter != m_alarmTypes.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                names.insert( std::multimap<CString, unsigned long>::value_type( iter->second->getDisplayName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* AlarmTypeDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the AlarmType matching the key passed in.
        LoadedAlarmTypes::iterator matching = m_alarmTypes.find(key);
        if (matching == m_alarmTypes.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Alarm Type with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AlarmTypeDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the AlarmType matching the key passed in.
        LoadedAlarmTypes::iterator matching = m_alarmTypes.find(key);
        if (matching == m_alarmTypes.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Alarm Type with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Alarm Type was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_alarmTypes.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AlarmTypeDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        //
        // Create a new alarm type with default values.
        //
        const std::string name        = "";
        const std::string description = "";
        unsigned long severityKey     = 1;
	    bool toBePrinted              = false;
		bool toBePersisted            = false;
		bool isSystemAlarm            = true;

        TA_Base_Core::IConfigAlarmType* newAlarmType =
            TA_Base_Core::AlarmTypeAccessFactory::getInstance().createAlarmType(
                name, description, severityKey, toBePrinted, toBePersisted, isSystemAlarm);


        m_alarmTypes.insert( LoadedAlarmTypes::value_type( newAlarmType->getUniqueIdentifier(), newAlarmType ) );

        FUNCTION_EXIT;
        return newAlarmType;
    }


    TA_Base_Core::IConfigItem* AlarmTypeDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigAlarmType* newAlarmType = NULL;

        // Attempt to find the AlarmType matching the key passed in.
        LoadedAlarmTypes::iterator matching = m_alarmTypes.find(idOfItemToCopy);

        TA_ASSERT(matching != m_alarmTypes.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAlarmType stored in the map");

        newAlarmType = TA_Base_Core::AlarmTypeAccessFactory::getInstance().copyAlarmType(matching->second);

        if (newAlarmType != NULL)
        {
            m_alarmTypes.insert( LoadedAlarmTypes::value_type( newAlarmType->getUniqueIdentifier(), newAlarmType ) );
        }
        
        return newAlarmType;

        FUNCTION_EXIT;
    }


    bool AlarmTypeDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& alarmTypesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(alarmTypesNotApplied.empty(), "The vector of strings passed was empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAlarmTypes::iterator iter = m_alarmTypes.begin(); iter != m_alarmTypes.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    alarmTypesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Alarm Type that has not had it's changes applied yet");
                    alarmTypesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Alarm Type that has not had it's changes applied yet");
                    alarmTypesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !alarmTypesNotApplied.empty();
    }


    std::map<unsigned long, std::string> AlarmTypeDatabaseAccessor::getAllSeverities()
    {
        FUNCTION_ENTRY("getAllSeverities");

        using TA_Base_Core::IAlarmSeverityData;

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the Severities from the database
        std::vector<IAlarmSeverityData*> severities;
        try
        {
            severities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No severities will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No severities will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ISeverity object when we're done
        for (std::vector<IAlarmSeverityData*>::iterator iter = severities.begin();
             iter != severities.end();
             ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This severity will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This severity will not be added to the map");
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

	std::map<IAlarmData::EMmsState, std::string> AlarmTypeDatabaseAccessor::getMmsAlarmTypes()
	{
		std::map<IAlarmData::EMmsState, std::string> result;
		result.insert(std::map<IAlarmData::EMmsState, std::string>::value_type(IAlarmData::TYPE_NONE, "None"));
        
        result.insert(std::map<IAlarmData::EMmsState, std::string>::value_type(IAlarmData::TYPE_SEMI, "Semi-Automatic"));        
		result.insert(std::map<IAlarmData::EMmsState, std::string>::value_type(IAlarmData::TYPE_AUTO, "Automatic"));
        return result;
	}
}
