/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/Sort.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class allows the column sorting to be set up and modified. It is a holding class for storing
  * the sort settings.
  */

#include <algorithm>

#include "bus/alarm/alarm_list_control/src/Sort.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	Sort::Sort()
	{
        // Empty
	}


	Sort::Sort(const Sort& sortToCopy)
	{
        m_sort = sortToCopy.m_sort;
    }


	Sort& Sort::operator=(const Sort& rhs)
	{
        if (this != &rhs)
        {
            // Clear the old vector
            std::vector<SortDetails>().swap(m_sort);

            m_sort = rhs.m_sort;
        }
        return *this;
	}
        

	Sort::~Sort()
	{
	}


    void Sort::addSort(SortType type, SortDirection direction)
    {
        FUNCTION_ENTRY("addSort");

        SortDetails newSort;
        newSort.type = type;
        newSort.direction = direction;

        // Check that the sort isn't already in the list. If it is remove it and add this new one to the end
        // We must do this so the sort order is as the user requests
        std::vector<SortDetails>::iterator matching = std::find(m_sort.begin(), m_sort.end(), newSort);
        if (matching != m_sort.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Sort already existed so it is being replaced with the new one");
            m_sort.erase( std::remove(m_sort.begin(), m_sort.end(), newSort) );
        }

        m_sort.push_back(newSort);

        FUNCTION_EXIT;
    }

    
    void Sort::removeSort(SortType type, SortDirection direction)
    {
        FUNCTION_ENTRY("removeSort");

        SortDetails sortToSearchFor;
        sortToSearchFor.type = type;
        sortToSearchFor.direction = direction;

        m_sort.erase( std::remove(m_sort.begin(), m_sort.end(), sortToSearchFor) );

        FUNCTION_EXIT;
    }


    void Sort::removeAll()
    {
        FUNCTION_ENTRY("removeAll");

        // Clear the vector
        std::vector<SortDetails>().swap(m_sort);

        FUNCTION_EXIT;
    }
}
