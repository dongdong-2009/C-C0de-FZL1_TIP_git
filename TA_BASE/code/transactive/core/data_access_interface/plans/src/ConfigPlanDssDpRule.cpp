/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigPlanDssDpRule.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPlanDssDpRule is a concrete implementation of IConfigPlanDssDpRule, 
  * which is in turn an implementation
  * of IPlanDssDpRule. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PlanDssDpRules.
  *
  */

#include <algorithm>
#include <sstream>

#include "core/data_access_interface/plans/src/ConfigPlanDssDpRule.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleHelper.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPlanDssDpRule::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    const char* const LOCATION_KEY = "LocationKey";
    const char* const TRIGGER_ENTITY_KEY = "TriggerEntityKey";
    const char* const ACTIVE_PLAN_KEY = "ActivePlanKey";
    const char* const MINIMUM_INTERVAL = "MinimumInterval";
    const char* const TOLERANT_TIME_FOR_ACTIVE = "TolerantTimeForActive";
    const char* const VALID_TIME_IN_DAY = "ValidTimeInDay";
    const char* const VALID_DAYS_IN_WEEK = "ValidDaysInWeek";
    const char* const ACTIVATE_VALUE = "ActivateValue";
    const char* const NEED_DEACTIVATE = "NeedDeactivate";
    const char* const ALLOW_MULTIPLE = "AllowMultiple";
    const char* const RULE_ENABLE = "Enable";
    const char* const DENY_MULTIPLE_STRATEGY = "DenyMultipleStrategy";

    ConfigPlanDssDpRule::ConfigPlanDssDpRule() : 
    m_PlanDssDpRuleHelper( new PlanDssDpRuleHelper() ),
    m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPlanDssDpRule::ConfigPlanDssDpRule(unsigned long row, TA_Base_Core::IData& data) : 
    m_PlanDssDpRuleHelper(new PlanDssDpRuleHelper(row, data)),
    m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPlanDssDpRule::ConfigPlanDssDpRule(const unsigned long key) : 
    m_PlanDssDpRuleHelper( new PlanDssDpRuleHelper(key) ),
    m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPlanDssDpRule::ConfigPlanDssDpRule( const ConfigPlanDssDpRule& theConfigPlanDssDpRule) : 
    m_PlanDssDpRuleHelper( new PlanDssDpRuleHelper( *(theConfigPlanDssDpRule.m_PlanDssDpRuleHelper)) ),
    m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }

    ConfigPlanDssDpRule::~ConfigPlanDssDpRule()
    {
        try
        {
            delete m_PlanDssDpRuleHelper;
            m_PlanDssDpRuleHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }

    bool ConfigPlanDssDpRule::isNew()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->isNew();
    }

    void ConfigPlanDssDpRule::setLocationKey(unsigned long key)
    {
        FUNCTION_ENTRY("setLocationKey");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }
    
        updatePlanDssDpRuleChanges(LOCATION_KEY, isNew() ? 0 : m_PlanDssDpRuleHelper->getLocationKey(), key);

        m_PlanDssDpRuleHelper->setLocationKey(key);

        FUNCTION_EXIT;
    }

    unsigned long ConfigPlanDssDpRule::getKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getKey();
    }

    unsigned long ConfigPlanDssDpRule::getLocationKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getLocationKey();
    }    

    unsigned long ConfigPlanDssDpRule::getDateCreated()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

		return m_PlanDssDpRuleHelper->getDateCreated();
    }

    unsigned long ConfigPlanDssDpRule::getDateModified()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

		return m_PlanDssDpRuleHelper->getDateModified();
    }
 	
    void ConfigPlanDssDpRule::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            FUNCTION_EXIT;
            TA_THROW( expWhat );
        }

        m_PlanDssDpRuleHelper->invalidate();
        m_PlanDssDpRuleChanges.clear();

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            FUNCTION_EXIT;
            TA_THROW( expWhat );
        }

        // Delete the ConfigPlanDssDpRule from the database
        m_PlanDssDpRuleHelper->deletePlanDssDpRule();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PlanDssDpRuleHelper pointer m_PlanDssDpRuleHelper");

        delete m_PlanDssDpRuleHelper;
        m_PlanDssDpRuleHelper = NULL;

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            FUNCTION_EXIT;
            TA_THROW( expWhat );
        }

        m_PlanDssDpRuleHelper->writePlanDssDpRuleData();

        m_PlanDssDpRuleChanges.clear();

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::updatePlanDssDpRuleChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePlanDssDpRuleChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_PlanDssDpRuleChanges.find(name);

        if (matching != m_PlanDssDpRuleChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_PlanDssDpRuleChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_PlanDssDpRuleChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::updatePlanDssDpRuleChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePlanDssDpRuleChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePlanDssDpRuleChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPlanDssDpRule::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPlanDssDpRule::getName()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        std::ostringstream name;
		name << "Plan DSS rule " << m_PlanDssDpRuleHelper->getKey();

        return name.str();
    }

    unsigned long ConfigPlanDssDpRule::getTriggerEntityKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getTriggerEntityKey();
    }

    unsigned long ConfigPlanDssDpRule::getActivePlanKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getActivePlanKey();
    }

    unsigned long ConfigPlanDssDpRule::getMinimumInterval()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getMinimumInterval();
    }

    unsigned long ConfigPlanDssDpRule::getTolerantTimeForActive()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getTolerantTimeForActive();
    }

    unsigned long ConfigPlanDssDpRule::getValidStartTimeInDay()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidStartTimeInDay();
    }

    unsigned long ConfigPlanDssDpRule::getValidEndTimeInDay()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidEndTimeInDay();
    }

    bool ConfigPlanDssDpRule::getValidSunday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidSunday();
    }

    bool ConfigPlanDssDpRule::getValidMonday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidMonday();
    }

    bool ConfigPlanDssDpRule::getValidTuesday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidTuesday();
    }

    bool ConfigPlanDssDpRule::getValidWednesday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidWednesday();
    }

    bool ConfigPlanDssDpRule::getValidThursday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidThursday();
    }

    bool ConfigPlanDssDpRule::getValidFriday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidFriday();
    }

    bool ConfigPlanDssDpRule::getValidSaturday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidSaturday();
    }

    bool ConfigPlanDssDpRule::getActivateValue()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getActivateValue();
    }

    bool ConfigPlanDssDpRule::getNeedDeactivate()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getNeedDeactivate();
    }

    bool ConfigPlanDssDpRule::getAllowMultiple()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getAllowMultiple();
    }

    bool ConfigPlanDssDpRule::getEnable()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getEnable();
    }

    int ConfigPlanDssDpRule::getDenyMultipleStrategy()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getDenyMultipleStrategy();
    }

	std::string ConfigPlanDssDpRule::getLocationName()
	{
		if ( m_PlanDssDpRuleHelper == NULL )
		{
			DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
			TA_THROW( expWhat );
		}

		return m_PlanDssDpRuleHelper->getLocationName();
	}

	std::string ConfigPlanDssDpRule::getTriggerEntityName()
	{
		if ( m_PlanDssDpRuleHelper == NULL )
		{
			DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
			TA_THROW( expWhat );
		}

		return m_PlanDssDpRuleHelper->getTriggerEntityName();
	}

	std::string ConfigPlanDssDpRule::getActivePlanName()
	{
		if ( m_PlanDssDpRuleHelper == NULL )
		{
			DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
			TA_THROW( expWhat );
		}

		return m_PlanDssDpRuleHelper->getActivePlanName();
	}

    void ConfigPlanDssDpRule::setTriggerEntityKey( const unsigned long ulTriggerEntityKey )
    {
        FUNCTION_ENTRY("setTriggerEntityKey");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(TRIGGER_ENTITY_KEY, isNew() ? 0 : m_PlanDssDpRuleHelper->getTriggerEntityKey(), ulTriggerEntityKey);

        m_PlanDssDpRuleHelper->setTriggerEntityKey( ulTriggerEntityKey );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setActivePlanKey( const unsigned long ulActivePlanKey )
    {
        FUNCTION_ENTRY("setActivePlanKey");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(ACTIVE_PLAN_KEY, isNew() ? 0 : m_PlanDssDpRuleHelper->getActivePlanKey(), ulActivePlanKey);

        m_PlanDssDpRuleHelper->setActivePlanKey( ulActivePlanKey );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setMinimumInterval( const unsigned long ulMinimumInterval )
    {
        FUNCTION_ENTRY("setMinimumInterval");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(MINIMUM_INTERVAL, isNew() ? 0 : m_PlanDssDpRuleHelper->getMinimumInterval(), ulMinimumInterval);

        m_PlanDssDpRuleHelper->setMinimumInterval( ulMinimumInterval );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setTolerantTimeForActive( const unsigned long ulTolerantTimeForActive )
    {
        FUNCTION_ENTRY("setTolerantTimeForActive");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(TOLERANT_TIME_FOR_ACTIVE, isNew() ? 0 : m_PlanDssDpRuleHelper->getTolerantTimeForActive(), ulTolerantTimeForActive);

        m_PlanDssDpRuleHelper->setTolerantTimeForActive( ulTolerantTimeForActive );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidStartTimeInDay( const unsigned long ulValidStartTimeInDay )
    {
        FUNCTION_ENTRY("setValidStartTimeInDay");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_TIME_IN_DAY, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidStartTimeInDay(), ulValidStartTimeInDay);

        m_PlanDssDpRuleHelper->setValidStartTimeInDay( ulValidStartTimeInDay );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidEndTimeInDay( const unsigned long ulValidEndTimeInDay )
    {
        FUNCTION_ENTRY("setValidEndTimeInDay");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_TIME_IN_DAY, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidEndTimeInDay(), ulValidEndTimeInDay);

        m_PlanDssDpRuleHelper->setValidEndTimeInDay( ulValidEndTimeInDay );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidSunday( const bool bValidSunday )
    {
        FUNCTION_ENTRY("setValidSunday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidSunday(), bValidSunday);

        m_PlanDssDpRuleHelper->setValidSunday( bValidSunday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidMonday( const bool bValidMonday )
    {
        FUNCTION_ENTRY("setValidMonday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidMonday(), bValidMonday);

        m_PlanDssDpRuleHelper->setValidMonday( bValidMonday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidTuesday( const bool bValidTuesday )
    {
        FUNCTION_ENTRY("setValidTuesday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidTuesday(), bValidTuesday);

        m_PlanDssDpRuleHelper->setValidTuesday( bValidTuesday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidWednesday( const bool bValidWednesday )
    {
        FUNCTION_ENTRY("setValidWednesday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidWednesday(), bValidWednesday);

        m_PlanDssDpRuleHelper->setValidWednesday( bValidWednesday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidThursday( const bool bValidThursday )
    {
        FUNCTION_ENTRY("setValidThursday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidThursday(), bValidThursday);

        m_PlanDssDpRuleHelper->setValidThursday( bValidThursday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidFriday( const bool bValidFriday )
    {
        FUNCTION_ENTRY("setValidFriday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidFriday(), bValidFriday);

        m_PlanDssDpRuleHelper->setValidFriday( bValidFriday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setValidSaturday( const bool bValidSaturday )
    {
        FUNCTION_ENTRY("setValidSaturday");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(VALID_DAYS_IN_WEEK, isNew() ? 0 : m_PlanDssDpRuleHelper->getValidSaturday(), bValidSaturday);

        m_PlanDssDpRuleHelper->setValidSaturday( bValidSaturday );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setActivateValue( const bool bActivateValue )
    {
        FUNCTION_ENTRY("setActivateValue");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(ACTIVATE_VALUE, isNew() ? 0 : m_PlanDssDpRuleHelper->getActivateValue(), bActivateValue);

        m_PlanDssDpRuleHelper->setActivateValue( bActivateValue );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setNeedDeactivate( const bool bNeedDeactivate )
    {
        FUNCTION_ENTRY("setNeedDeactivate");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(NEED_DEACTIVATE, isNew() ? 0 : m_PlanDssDpRuleHelper->getNeedDeactivate(), bNeedDeactivate);

        m_PlanDssDpRuleHelper->setNeedDeactivate( bNeedDeactivate );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setAllowMultiple( const bool bAllowMultiple )
    {
        FUNCTION_ENTRY("setAllowMultiple");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(ALLOW_MULTIPLE, isNew() ? 0 : m_PlanDssDpRuleHelper->getAllowMultiple(), bAllowMultiple);

        m_PlanDssDpRuleHelper->setAllowMultiple( bAllowMultiple );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setEnable( const bool bEnable )
    {
        FUNCTION_ENTRY("setAllowMultiple");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(RULE_ENABLE, isNew() ? 0 : m_PlanDssDpRuleHelper->getEnable(), bEnable);

        m_PlanDssDpRuleHelper->setEnable( bEnable );

        FUNCTION_EXIT;
    }

    void ConfigPlanDssDpRule::setDenyMultipleStrategy( const int nDenyMultipleStrategy )
    {
        FUNCTION_ENTRY("setDenyMultipleStrategy");

        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        updatePlanDssDpRuleChanges(DENY_MULTIPLE_STRATEGY, isNew() ? 0 : m_PlanDssDpRuleHelper->getDenyMultipleStrategy(), nDenyMultipleStrategy);

        m_PlanDssDpRuleHelper->setDenyMultipleStrategy( nDenyMultipleStrategy );

        FUNCTION_EXIT;
    }
} // closes TA_Base_Core

