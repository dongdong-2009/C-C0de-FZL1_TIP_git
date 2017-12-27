/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/Filter.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class allows filters to be set up and modified. It is a holding class for storing
  * filters.
  */

#include <algorithm>

#include "bus/alarm/alarm_list_control/src/Filter.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	Filter::Filter()
	{
        // Empty
	}


	Filter::Filter(const Filter& filterToCopy)
	{
        m_filters = filterToCopy.m_filters;

        for(std::vector<FilterDetails>::iterator iter = m_filters.begin(); iter != m_filters.end(); ++iter)
        {
            // Now update any string pointers so they don't point to the same objects
            if( (iter->type == FILTER_PROFILE) ||
                (iter->type == FILTER_ASSOCIATED_ENTITY_NAME) ||
                (iter->type == FILTER_ASSET_NAME) ||
				(iter->type == FILTER_DESCRIPTION) )
            {
                std::string savedString = *(iter->value.stringValue);
                iter->value.stringValue = new std::string(savedString);
            }
        }

		m_filterName = filterToCopy.m_filterName;
    }


	Filter& Filter::operator=(const Filter& rhs)
	{
        if (this != &rhs)
        {
            // Delete old data and clear the vector
			std::vector<FilterDetails>::iterator iter = m_filters.end();
            for( iter = m_filters.begin(); iter != m_filters.end(); ++iter)
            {
                if( (iter->type == FILTER_PROFILE) ||
                    (iter->type == FILTER_ASSOCIATED_ENTITY_NAME) ||
                    (iter->type == FILTER_ASSET_NAME) ||
					(iter->type == FILTER_DESCRIPTION) )
                {
                    std::string savedString = *(iter->value.stringValue);
                    delete iter->value.stringValue;
                    iter->value.stringValue = NULL;
                }
            }
            std::vector<FilterDetails>().swap(m_filters);


            m_filters = rhs.m_filters;

            // Now update any string pointers so they don't point to the same objects
            for(iter = m_filters.begin(); iter != m_filters.end(); ++iter)
            {
                if( (iter->type == FILTER_PROFILE) ||
                    (iter->type == FILTER_ASSOCIATED_ENTITY_NAME) ||
                    (iter->type == FILTER_ASSET_NAME) ||
					(iter->type == FILTER_DESCRIPTION) )
                {
                    std::string savedString = *(iter->value.stringValue);
                    iter->value.stringValue = new std::string(savedString);
                }
            }

			m_filterName = rhs.m_filterName;
        }
        return *this;
	}
        

	Filter::~Filter()
	{
        try
        {
            for(std::vector<FilterDetails>::iterator iter = m_filters.begin(); iter != m_filters.end(); ++iter)
            {
                if( (iter->type == FILTER_PROFILE) ||
                    (iter->type == FILTER_ASSOCIATED_ENTITY_NAME) ||
                    (iter->type == FILTER_ASSET_NAME) ||
					(iter->type == FILTER_DESCRIPTION) )
                {
                    std::string savedString = *(iter->value.stringValue);
                    delete iter->value.stringValue;
                    iter->value.stringValue = NULL;
                }
            }
        }
        catch( ... )
        {
        }
	}


    void Filter::addFilter(FilterType type, FilterComparison comparison, std::string value)
    {
        FUNCTION_ENTRY("addFilter");

        TA_ASSERT(type == FILTER_PROFILE || type == FILTER_DESCRIPTION || type == FILTER_OPERATOR ||
                  type == FILTER_ASSOCIATED_ENTITY_NAME  || type == FILTER_ASSET_NAME || type == FILTER_SUBSYSTEMTYPE || type == FILTER_SYSTEMTYPE,
                  "Filter type must be FILTER_OPERATOR, FILTER_PROFILE, FILTER_ASSET_NAME, FILTER_DESCRIPTION or FILTER_ASSOCIATED_ENTITY_NAME or FILTER_SUBSYSTEMTYPE to call the string version of this function");

        FilterDetails newFilter;
        newFilter.type = type;
        newFilter.comparison = comparison;
        newFilter.value.stringValue = new std::string(value);

        m_filters.push_back(newFilter);

        FUNCTION_EXIT;
    }

    
    void Filter::addFilter(FilterType type, FilterComparison comparison, unsigned long value)
    {
        FUNCTION_ENTRY("addFilter");

        TA_ASSERT(type == FILTER_ALARM_TIME || type == FILTER_LOCATION ||
                  type == FILTER_ASSOCIATED_ENTITY_KEY ||
                  type == FILTER_SUBSYSTEM  || type == FILTER_SEVERITY || type == FILTER_ALARM_TYPE,
                  "Filter type must be FILTER_ALARM_TIME, FILTER_LOCATION, FILTER_ASSOCIATED_ENTITY_KEY, FILTER_SUBSYSTEM or FILTER_SEVERITY or FILTER_ALARM_TYPE to call the unsigned long version of this function");

        FilterDetails newFilter;
        newFilter.type = type;
        newFilter.comparison = comparison;
        newFilter.value.ulValue = value;

        m_filters.push_back(newFilter);

        FUNCTION_EXIT;
    }
    

    void Filter::addFilter(FilterType type, FilterComparison comparison, EAlarmState state)
    {
        FUNCTION_ENTRY("addFilter");

        TA_ASSERT(type == FILTER_STATE, "Filter type must be FILTER_STATE to call the EAlarmState version of this function");

        FilterDetails newFilter;
        newFilter.type = type;
        newFilter.comparison = comparison;
        newFilter.value.alarmStateValue = state;

        m_filters.push_back(newFilter);

        FUNCTION_EXIT;
    }

    void Filter::addFilter(FilterType type, FilterComparison comparison, EAlarmOMType OMType)
    {
        FUNCTION_ENTRY("addFilter");

        TA_ASSERT(type == FILTER_OMTYPE, "Filter type must be FILTER_OMTYPE to call the EAlarmState version of this function");

        FilterDetails newFilter;
        newFilter.type = type;
        newFilter.comparison = comparison;
        newFilter.value.alarmOMTypeValue = OMType;

        m_filters.push_back(newFilter);

        FUNCTION_EXIT;
    }

    void Filter::addFilter(FilterType type, FilterComparison comparison, const SDateRange& dateRange)
    {
        FUNCTION_ENTRY("addFilter");

        TA_ASSERT(type == FILTER_ALARM_TIME_RANGE, "Filter type must be FILTER_ALARM_TIME_RANGE to call the EAlarmState version of this function");

        FilterDetails newFilter;
        newFilter.type = type;
        newFilter.comparison = comparison;
        newFilter.value.dateRange = dateRange;

        m_filters.push_back(newFilter);

        FUNCTION_EXIT;
    }


    void Filter::removeAll()
    {
        FUNCTION_ENTRY("removeAll");

        // Delete old data and clear the vector
        for(std::vector<FilterDetails>::iterator iter = m_filters.begin(); iter != m_filters.end(); ++iter)
        {
            if( (iter->type == FILTER_PROFILE) ||
                (iter->type == FILTER_ASSOCIATED_ENTITY_NAME) )
            {
                std::string savedString = *(iter->value.stringValue);
                delete iter->value.stringValue;
                iter->value.stringValue = NULL;
            }
        }
        std::vector<FilterDetails>().swap(m_filters); // clear

        FUNCTION_EXIT;
    }
}
