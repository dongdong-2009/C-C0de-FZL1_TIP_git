/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/Filter.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class allows filters to be set up and modified. It is a holding class for storing
  * filters.
  */

#ifndef FILTER_TUE_10_02_2004_H
#define FILTER_TUE_10_02_2004_H

#include <vector>
#include <string>

namespace TA_Base_Bus
{
    class Filter
    {
    public:
        /**
         * Constructor
         */
        Filter();


        /**
         * Copy Constructor
         */
        Filter(const Filter&);


        /**
         * operator=
         */
        Filter& operator=(const Filter&);
        

        /**
         * Destructor
         */
        ~Filter();


        enum FilterType
        {
            FILTER_PROFILE,
            FILTER_ALARM_TYPE,
            FILTER_ALARM_TIME,
            FILTER_ALARM_TIME_RANGE,
            FILTER_ASSOCIATED_ENTITY_KEY,
            FILTER_ASSOCIATED_ENTITY_NAME,
            FILTER_LOCATION,
            FILTER_OPERATOR,
            FILTER_SEVERITY,
            FILTER_STATE,
            FILTER_SUBSYSTEM,
            FILTER_ASSET_NAME,
			FILTER_DESCRIPTION,
			FILTER_OMTYPE,
			FILTER_SUBSYSTEMTYPE,
			FILTER_SYSTEMTYPE
        };


        enum FilterComparison
        {
            COMPARE_LESS_THAN,
            COMPARE_LESS_THAN_EQUAL,
            COMPARE_EQUAL,
            COMPARE_GREATER_THAN,
            COMPARE_GREATER_THAN_EQUAL,
            COMPARE_NOT_EQUAL,
            COMPARE_CONTAINS,
            COMPARE_RANGE_INCLUSIVE,
            COMPARE_RANGE_EXCLUSIVE
        };

        
        enum EAlarmState
        {
            STATE_ALL_OPEN,
            STATE_OPEN_UNACKED,
            STATE_OPEN_ACKED,
            STATE_CLOSED,
            STATE_ALL_UNACKED,
            STATE_ANY
        };

        enum EAlarmOMType
        {
            OMTYPE_OPERATION,
            OMTYPE_MAINTENANCE,
            OMTYPE_BOTH,
            OMTYPE_ALL
        };

        struct SDateRange
        {
            unsigned long startTime;
            unsigned long endTime;
        };

        /**
         * setFilterName
         *
         * This will set the name of this Filter for saving and retreiving
         *
         * @param string - The string name.
         */
		void setFilterName(std::string name)
		{
			m_filterName = name;
		}

        /**
         * getFilterName
         *
         * This will set the name of this Filter for saving and retreiving
         *
         * @return string - The filter string name.
         */
		std::string getFilterName() const
		{
			return m_filterName;
		}


        /**
         * addFilter
         *
         * This will add a new filter to the current selection of filters. This method should be used
         * for the types:
         * FILTER_PROFILE, FILTER_ASSOCIATED_ENTITY_NAME, FILTER_ASSET_NAME, FILTER_DESCRIPTION,
		 * FILTER_OPERATOR, FILTER_ALARM_TYPE
         * Any others will cause an exception.
         *
         * @param FilterType - The alarm field to filter on
         * @param FilterComparison - The type of comparison to perform
         * @param string - The string value to compare against.
         */
        void addFilter(FilterType type, FilterComparison comparison, std::string value);

        
        /**
         * addFilter
         *
         * This will add a new filter to the current selection of filters. This method should be used
         * for the types:
         * FILTER_SUBSYSTEM, FILTER_ALARM_TIME, FILTER_LOCATION,
         * FILTER_SEVERITY, FILTER_ASSOCIATED_ENTITY_KEY
         * Any others will cause an exception
         *
         * @param FilterType - The alarm field to filter on
         * @param FilterComparison - The type of comparison to perform
         * @param unsigned long - The unsigned long value to compare against.
         */
        void addFilter(FilterType type, FilterComparison comparison, unsigned long value);
        

        /**
         * addFilter
         *
         * This will add a new filter to the current selection of filters. This method should be used
         * for the types:
         * FILTER_STATE
         * Any others will cause an exception
         *
         * @param FilterType - The alarm field to filter on
         * @param FilterComparison - The type of comparison to perform
         * @param EAlarmState - The alarm state to compare against
         */
        void addFilter(FilterType type, FilterComparison comparison, EAlarmState state);


        /**
         * addFilter
         *
         * This will add a new filter to the current selection of filters. This method should be used
         * for the types:
         * FILTER_OMTYPE
         * Any others will cause an exception
         *
         * @param FilterType - The alarm field to filter on
         * @param FilterComparison - The type of comparison to perform
         * @param EAlarmOMType - The alarm omtype to compare against
         */
     void addFilter(FilterType type, FilterComparison comparison, EAlarmOMType OMType);

        /**
         * addFilter
         *
         * This will add a new filter to the current selection of filters. This method should be used
         * for the types:
         * FILTER_ALARM_TIME_RANGE
         * Any others will cause an exception
         *
         * @param FilterType - The alarm field to filter on
         * @param FilterComparison - The type of comparison to perform
         * @param DateRange - The dates to compare against
         */
        void addFilter(FilterType type, FilterComparison comparison, const SDateRange& dateRange);


        /**
         * removeAll
         *
         * This will remove all current filters ready for a new one to be created
         */
        void removeAll();


        /** 
         * isFilterEmpty
         *
         * This determines if this filter is empty. (therefore no filtering)
         *
         * @return bool - true if there is no filter set up.
         */
        bool isFilterEmpty() const
        {
            return m_filters.empty();
        }

        
        // The value used in this union will be based on the FilterType as outlined below
        // The string variable has to be a pointer for the union.
        union FilterValue
        {
            std::string* stringValue;    // FILTER_PROFILE, FILTER_ASSOCIATED_ENTITY_NAME,FILTER_SUBSYSTEMTYPE,FILTER_SYSTEMTYPE
                                         // FILTER_ASSET_NAME, FILTER_DESCRIPTION
            unsigned long ulValue;       // FILTER_ALARM_TIME, FILTER_LOCATION, FILTER_OPERATOR,
                                         // FILTER_ASSOCIATED_ENTITY_KEY, FILTER_SUBSYSTEM, FILTER_ALARM_TYPE
            EAlarmState alarmStateValue; // FILTER_STATE
            EAlarmOMType alarmOMTypeValue; //FILTER_OMTPYE
            SDateRange dateRange;        // FILTER_ALARM_TIME_RANGE
        };


        struct FilterDetails
        {
            FilterType type;
            FilterComparison comparison;
            FilterValue value;
        };


        /** 
         * getFilters
         *
         * Returns all the current filter settings.
         *
         * @return vector<FilterDetails> - A vector of filter information
         */
        std::vector<FilterDetails> getFilters() const
        {
            return m_filters;
        }


    private:

        std::vector<FilterDetails> m_filters;

		std::string m_filterName;
    };
}

#endif // FILTER_TUE_10_02_2004_H
