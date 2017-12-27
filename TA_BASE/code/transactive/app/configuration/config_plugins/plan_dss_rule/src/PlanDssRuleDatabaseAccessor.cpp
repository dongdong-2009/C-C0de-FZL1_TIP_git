/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/06/16 10:34:31 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This class uses the Data Access Interface to retrieve alarm plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#include "app/configuration/config_plugins/plan_dss_rule/src/StdAfx.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"
#include "ace/Date_Time.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDatabaseAccessor.h"


#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/plans/src/IConfigPlanDssDpRule.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"


namespace TA_Base_App
{
    const int MAX_ARRAY_LENGTH = 10;

    PlanDssRuleDatabaseAccessor::PlanDssRuleDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    PlanDssRuleDatabaseAccessor::~PlanDssRuleDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedDssRules::iterator iter = m_PlanDssRules.begin(); iter != m_PlanDssRules.end(); ++iter)
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
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void PlanDssRuleDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "We have already loaded the alarm plan associations and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedDssRules::iterator assocToDelete = m_PlanDssRules.begin(); assocToDelete != m_PlanDssRules.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_PlanDssRules.clear();


        // Now reload all the alarm plan associations
        std::vector<TA_Base_Core::IPlanDssDpRule*> associations = TA_Base_Core::PlanDssDpRuleAccessFactory::getInstance().getAllPlanDssDpRules( true );

        // And insert the alarm plan associations into our map. At the same time cast them to IConfigPlanDssDpRule*
        // instead of IPlanDssDpRule*.
        for (std::vector<TA_Base_Core::IPlanDssDpRule*>::iterator iter = associations.begin(); iter != associations.end(); ++iter)
        {
             TA_Base_Core::IConfigPlanDssDpRule* castItem = dynamic_cast<TA_Base_Core::IConfigPlanDssDpRule*>(*iter);
             if( castItem != NULL )
             {
                m_PlanDssRules.insert( LoadedDssRules::value_type(castItem->getUniqueIdentifier(), castItem) );
             }
        }

        reloadLocations();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> PlanDssRuleDatabaseAccessor::getItemNames( CProgressCtrl& progress )
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> mapNotExist;

        FUNCTION_EXIT;
        return mapNotExist;
    }


    std::map<unsigned long,PlanDssRuleDatabaseAccessor::PlanDssDpRuleText> PlanDssRuleDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,PlanDssDpRuleText> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_PlanDssRules.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedDssRules::iterator iter = m_PlanDssRules.begin(); iter != m_PlanDssRules.end(); ++iter)
        {
            if (iter->second != NULL)
            {
                PlanDssDpRuleText item;
                convertItemToDisplay( *(iter->second), item );

                details.insert( std::map<unsigned long, PlanDssDpRuleText>::value_type( iter->second->getUniqueIdentifier(), item ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PlanDssRuleDatabaseAccessor::PlanDssDpRuleText PlanDssRuleDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedDssRules::iterator matching = m_PlanDssRules.find(identifier);
        if (matching == m_PlanDssRules.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not find the alarm plan association with identifier %lu",identifier);
            FUNCTION_EXIT;
            PlanDssDpRuleText item;
            return item;
        }

        PlanDssDpRuleText item;
        convertItemToDisplay( *(matching->second), item );

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* PlanDssRuleDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedDssRules::iterator matching = m_PlanDssRules.find(key);
        if (matching == m_PlanDssRules.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void PlanDssRuleDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedDssRules::iterator matching = m_PlanDssRules.find(key);
        if (matching == m_PlanDssRules.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored alarm plan association was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }

        delete matching->second;
        matching->second = NULL;
        m_PlanDssRules.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PlanDssRuleDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigPlanDssDpRule* pNewRule = TA_Base_Core::PlanDssDpRuleAccessFactory::getInstance().createPlanDssDpRule();

        m_PlanDssRules.insert( LoadedDssRules::value_type( pNewRule->getUniqueIdentifier(), pNewRule ) );

        FUNCTION_EXIT;
        return pNewRule;
    }


    TA_Base_Core::IConfigItem* PlanDssRuleDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigPlanDssDpRule* pNewRule = NULL;

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedDssRules::iterator matching = m_PlanDssRules.find(idOfItemToCopy);

        TA_ASSERT(matching != m_PlanDssRules.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPlanDssRule stored in the map");

        pNewRule = TA_Base_Core::PlanDssDpRuleAccessFactory::getInstance().copyPlanDssDpRule(matching->second);

        if (pNewRule != NULL)
        {
            m_PlanDssRules.insert( LoadedDssRules::value_type( pNewRule->getUniqueIdentifier(), pNewRule ) );
        }

        FUNCTION_EXIT;
        return pNewRule;
    }


    bool PlanDssRuleDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedDssRules::iterator iter = m_PlanDssRules.begin(); iter != m_PlanDssRules.end();  ++iter)
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

    void PlanDssRuleDatabaseAccessor::invalidateData()
    {
        m_isLoaded = false;
    }

    void PlanDssRuleDatabaseAccessor::convertItemToDisplay( TA_Base_Core::IConfigPlanDssDpRule& refFrom, PlanDssDpRuleText& refTo )
    {
        refTo.strLocation = refFrom.getLocationName();
		refTo.strActivePlan = refFrom.getActivePlanName();
		refTo.strTriggerEntity = refFrom.getTriggerEntityName();
        refTo.strMinimumInterval = TA_Base_Core::DataConversion::toString( refFrom.getMinimumInterval() ) + " 毫秒";
		refTo.strActivedTolerantTime = TA_Base_Core::DataConversion::toString( refFrom.getTolerantTimeForActive()) + " 秒";
		refTo.strDenyStrategy = TA_Base_Core::DenyMultipleStrategyString[refFrom.getDenyMultipleStrategy()];
		refTo.strActivateValue = refFrom.getActivateValue() ? "真" : "假";
		refTo.strNeedDeactivate = refFrom.getNeedDeactivate() ? "是" : "否";
        refTo.strAllowMultiple = refFrom.getAllowMultiple() ? "允许" : "禁止";
		refTo.strEnable = refFrom.getEnable() ? "启用" : "禁用";
		
		time_t currentTime = ACE_OS::gettimeofday().sec();
		time_t zeroTime = currentTime-((currentTime + SECONDS_INTERVAL) % SECONDS_IN_DAY);

		//Get Start Time
		time_t startTime = refFrom.getValidStartTimeInDay();
		startTime = startTime + zeroTime;
		char arrayStartTime[MAX_ARRAY_LENGTH] = {0};
		struct tm* tmStartTime = localtime ( &startTime );
		strftime (arrayStartTime,sizeof(arrayStartTime),"%H:%M:%S",tmStartTime);
		std::string strStartTime(arrayStartTime);
		refTo.strValidTimeInDay = strStartTime;

		//Get End Time
		time_t endTime = refFrom.getValidEndTimeInDay();
		endTime = endTime + zeroTime;
		char arrayEndTime[MAX_ARRAY_LENGTH] = {0};
		struct tm* tmEndTime = localtime ( &endTime );
		strftime (arrayEndTime,sizeof(arrayEndTime),"%H:%M:%S",tmEndTime);
		std::string strEndTime(arrayEndTime);
		refTo.strValidTimeInDay = refTo.strValidTimeInDay + " ~ " + strEndTime;

		convertValidDaysToDisplay(refTo.strValidDaysInWeek,refFrom);
    }

	void PlanDssRuleDatabaseAccessor::convertValidDaysToDisplay( std::string& strValidDaysInWeek, TA_Base_Core::IConfigPlanDssDpRule& refFrom )
	{
		if(refFrom.getValidSunday())
		{
			strValidDaysInWeek = " 星期日";
		}
		if(refFrom.getValidMonday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期一";
		}
		if(refFrom.getValidTuesday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期二";
		}
		if(refFrom.getValidWednesday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期三";
		}
		if(refFrom.getValidThursday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期四";
		}
		if(refFrom.getValidFriday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期五";
		}
		if(refFrom.getValidSaturday())
		{
			strValidDaysInWeek = strValidDaysInWeek + " 星期六";
		} 
	}

    std::map<unsigned long, std::string> PlanDssRuleDatabaseAccessor::getAllLocations()
    {
        return m_mapLocations;
    }

    void PlanDssRuleDatabaseAccessor::reloadLocations()
    {
        std::vector<TA_Base_Core::ILocation*> vecLocations;

        try
        {
            vecLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch (...)
        {
            return;
        }

        m_mapLocations.clear();
        for ( std::vector<TA_Base_Core::ILocation*>::iterator itLoop = vecLocations.begin(); itLoop != vecLocations.end(); ++itLoop )
        {
            m_mapLocations[(*itLoop)->getKey()] = (*itLoop)->getDisplayName();
        }
    }

    std::map<unsigned long, std::string> PlanDssRuleDatabaseAccessor::getAllStrategies(const bool bOnlyValid )
    {
        std::map<unsigned long, std::string> mapStrategies;

        for ( unsigned long ulLoop = TA_Base_Core::StrategyNoDeny; ulLoop < TA_Base_Core::StrategyCount; ++ulLoop )
        {
            if ( bOnlyValid && ( ulLoop == TA_Base_Core::StrategyNoDeny ))
            {
                continue;
            }

            mapStrategies[ulLoop] = TA_Base_Core::DenyMultipleStrategyString[ulLoop];
        }

        return mapStrategies;
    }
}
