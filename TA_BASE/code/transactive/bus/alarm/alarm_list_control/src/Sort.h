/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/Sort.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class allows the column sorting to be set up and modified. It is a holding class for storing
  * the sort settings.
  */

#ifndef SORT_TUE_10_02_2004_H
#define SORT_TUE_10_02_2004_H

#include <vector>
#include <string>


namespace TA_Base_Bus
{
    class Sort
    {
    public:
        /**
         * Constructor
         */
        Sort();


        /**
         * Copy Constructor
         */
        Sort(const Sort&);


        /**
         * operator=
         */
        Sort& operator=(const Sort&);
        

        /**
         * Destructor
         */
        ~Sort();


		enum SortType
        {
            SORT_OPEN_CLOSE,
			SORT_ACK_NOT_ACK,
			SORT_ALARM_TYPE,
			SORT_LOCATION,
			SORT_TIME,
			SORT_DESCRIPTION,
			SORT_ENTITY_NAME, // ASSET
			SORT_SUB_SYSTEM_KEY,
			SORT_ACK_TIME,
			SORT_ACK_BY,
            SORT_SEVERITY,
            SORT_ASSET_NAME,
            SORT_VALUE,
            SORT_MMS,
            SORT_DSS,
            SORT_AVALANCHE,
			//TD #12719++
			SORT_COMMENT
			//++TD #12719
        };


		enum SortDirection
        {
            SORT_ASCENDING,
		    SORT_DESCENDING
        };


        /**
         * addSort
         *
         * This adds this sort to the end of the current sorting setup.
         *
         * Each type can only be added once.
         *
         * @param SortType - The alarm field to sort on
         * @param SortDirection - The direction of the sort
         */
        void addSort(SortType type, SortDirection direction);


        /**
         * removeSort
         *
         * This removes the specified column from the sort order
         *
         * @param SortType - The alarm field to sort on
         * @param SortDirection - The direction of the sort
         */
        void removeSort(SortType type, SortDirection direction);


        /**
         * removeAll
         *
         * This clears the sort ready for a new one to be set up
         */
        void removeAll();


        /** 
         * isSortEmpty
         *
         * This determines if this sort is empty. (therefore no sorting)
         *
         * @return bool - true if there is no sort set up.
         */
        bool isSortEmpty() const
        {
            return m_sort.empty();
        }


        struct SortDetails
        {
            SortType type;
            SortDirection direction;

            bool operator==(const SortDetails& rhs)
            {
                // value is not used when comparing.
                return ( type == rhs.type && direction == rhs.direction );
            }
        };


        /** 
         * getSort
         *
         * Returns all the columns in the sort
         *
         * @return vector<SortDetails> - A vector of sort information
         */
        std::vector<SortDetails> getSort() const
        {
            return m_sort;
        }


    private:

        std::vector<SortDetails> m_sort;
    };
}

#endif // SORT_TUE_10_02_2004_H
