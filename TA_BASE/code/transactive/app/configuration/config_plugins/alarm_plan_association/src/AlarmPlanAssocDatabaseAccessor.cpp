/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve alarm plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDatabaseAccessor.h"


#include "core/data_access_interface/plans/src/IConfigAlarmPlanAssoc.h"
#include "core/data_access_interface/plans/src/IAlarmPlanAssocData.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    AlarmPlanAssocDatabaseAccessor::AlarmPlanAssocDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    AlarmPlanAssocDatabaseAccessor::~AlarmPlanAssocDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAssociations::iterator iter = m_alarmPlanAssocs.begin(); iter != m_alarmPlanAssocs.end(); ++iter)
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


    void AlarmPlanAssocDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the alarm plan associations and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAssociations::iterator assocToDelete = m_alarmPlanAssocs.begin(); assocToDelete != m_alarmPlanAssocs.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_alarmPlanAssocs.clear();


        // Now reload all the alarm plan associations
        std::vector<TA_Base_Core::IAlarmPlanAssocData*> associations = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().getAllAlarmPlanAssociations( true);

        // And insert the alarm plan associations into our map. At the same time cast them to IConfigAlarmPlanAssoc*
        // instead of IAlarmPlanAssocData*.
        for (std::vector<TA_Base_Core::IAlarmPlanAssocData*>::iterator iter = associations.begin();
             iter != associations.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigAlarmPlanAssoc* castItem = dynamic_cast<TA_Base_Core::IConfigAlarmPlanAssoc*>(*iter);
             if( castItem != NULL )
             {
                m_alarmPlanAssocs.insert( LoadedAssociations::value_type(castItem->getUniqueIdentifier(), castItem) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AlarmPlanAssocDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for alarm plan associations");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation> AlarmPlanAssocDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,AlarmPlanAssociation> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_alarmPlanAssocs.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAssociations::iterator iter = m_alarmPlanAssocs.begin();
             iter != m_alarmPlanAssocs.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                AlarmPlanAssociation item;
                item.alarmTypeDisplay = iter->second->getAlarmTypeDisplayName().c_str();
                item.entity = iter->second->getEntityName().c_str();
                item.entityType = iter->second->getEntityTypeName().c_str();
                item.plan = iter->second->getPlanPath().c_str();

                details.insert( std::map<unsigned long, AlarmPlanAssociation>::value_type( iter->second->getUniqueIdentifier(), item ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation AlarmPlanAssocDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAssociations::iterator matching = m_alarmPlanAssocs.find(identifier);
        if (matching == m_alarmPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with identifier %lu",identifier);
            FUNCTION_EXIT;
            AlarmPlanAssociation item;
            return item;
        }

        AlarmPlanAssociation item;
        item.alarmTypeDisplay =  matching->second->getAlarmTypeDisplayName().c_str();
        item.entity =     matching->second->getEntityName().c_str();
        item.entityType = matching->second->getEntityTypeName().c_str();
        item.plan =       matching->second->getPlanPath().c_str();

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* AlarmPlanAssocDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAssociations::iterator matching = m_alarmPlanAssocs.find(key);
        if (matching == m_alarmPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AlarmPlanAssocDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAssociations::iterator matching = m_alarmPlanAssocs.find(key);
        if (matching == m_alarmPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored alarm plan association was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisAssociation();
        }
        delete matching->second;
        matching->second = NULL;
        m_alarmPlanAssocs.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AlarmPlanAssocDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigAlarmPlanAssoc* newAssoc = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().createAlarmPlanAssociation();

        m_alarmPlanAssocs.insert( LoadedAssociations::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AlarmPlanAssocDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigAlarmPlanAssoc* newAssoc = NULL;

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAssociations::iterator matching = m_alarmPlanAssocs.find(idOfItemToCopy);

        TA_ASSERT(matching != m_alarmPlanAssocs.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAlarmPlanAssoc stored in the map");

        newAssoc = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().copyAlarmPlanAssociation(matching->second);

        if (newAssoc != NULL)
        {
            m_alarmPlanAssocs.insert( LoadedAssociations::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool AlarmPlanAssocDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAssociations::iterator iter = m_alarmPlanAssocs.begin(); iter != m_alarmPlanAssocs.end();  ++iter)
        {
            if( (iter->second != NULL) && (iter->second->hasChanged() || iter->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for alarm plan associations
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::map<unsigned long, std::string> AlarmPlanAssocDatabaseAccessor::getAllAlarmTypes()
    {
        FUNCTION_ENTRY("getAllAlarmTypes");
        std::map<unsigned long, std::string> namesAndKeys;

        using TA_Base_Core::IAlarmTypeData;

        // First retrieve all the subsystems from the database
        std::vector<IAlarmTypeData*> alarmTypes;
        try
        {
            alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IAlarmTypeData object when we're done
        for (std::vector<IAlarmTypeData*>::iterator iter = alarmTypes.begin(); iter != alarmTypes.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This alarm type will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This alarm type will not be added to the map");
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

	std::map<unsigned long, std::string> AlarmPlanAssocDatabaseAccessor::getAllAlarmTypeDisplays()
	{
		FUNCTION_ENTRY("getAllAlarmTypeDisplays");
		std::map<unsigned long, std::string> namesAndKeys;

		using TA_Base_Core::IAlarmTypeData;

		// First retrieve all the subsystems from the database
		std::vector<IAlarmTypeData*> alarmTypes;
		try
		{
			alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
		}
		catch( const TA_Base_Core::DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
			FUNCTION_EXIT;
			return namesAndKeys;
		}
		catch( const TA_Base_Core::DataException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
			FUNCTION_EXIT;
			return namesAndKeys;
		}

		// Now step through each one and:
		// 1) Add it's name and key to the map we want ot return
		// 2) Delete the IAlarmTypeData object when we're done
		for (std::vector<IAlarmTypeData*>::iterator iter = alarmTypes.begin(); iter != alarmTypes.end(); ++iter)
		{
			if (*iter != NULL)
			{
				try
				{
					namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getDisplayName() ) );
				}
				catch ( const TA_Base_Core::DataException& )
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This alarm type will not be added to the map");
				}
				catch ( const TA_Base_Core::DatabaseException& )
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This alarm type will not be added to the map");
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


    std::map<unsigned long, std::string> AlarmPlanAssocDatabaseAccessor::getAllEntityTypes()
    {
        FUNCTION_ENTRY("getAllEntityTypes");

        // First retrieve all the subsystems from the database
        std::map<unsigned long, std::string> entityTypes;
        try
        {
            entityTypes = TA_Base_Core::EntityAccessFactory::getInstance().getEntityTypes();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
        }

        FUNCTION_EXIT;
        return entityTypes;
    }

}
