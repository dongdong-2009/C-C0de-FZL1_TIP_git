/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterSet.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class manages a collection of alarm filters to be applied to an alarm to
  * determine if it should be included.
  *
  * Note: The filter set will hold separate filter lists for each type of filter.  This is 
  * required to apply the rule that filters of the same type are OR'd in the comparison,
  * whilst across filter types we must AND the results.  Note that for dates we must now 
  * have 
  *
  *
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterSet.h
//  Implementation of the Class AlarmFilterSet
//  Created on:      24-Jan-2004 04:12:48 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterSet_EA079B54_F04B_4704_A0F4_1060F40DC5B3__INCLUDED_)
#define AlarmFilterSet_EA079B54_F04B_4704_A0F4_1060F40DC5B3__INCLUDED_

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "IAlarmFilter.h"
#include <vector>
#include <map>
#include <string>
	
//class TA_Base_Core::AlarmItem;
namespace TA_Base_Bus
{
	class AlarmFilterSet
	{
		public:

			AlarmFilterSet();
			virtual ~AlarmFilterSet();

			/*
			* AddFilter
			* 
			* This method will add a filter to the collection.  Ownership
			* of the passed filter will pass from calling function to this class.
			*  This class will delete the filter when it is finished with it.
			*/
			void addFilter(IAlarmFilter* filter);

			bool passesFilters(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			bool passesFilters(const TA_Base_Core::AlarmItem* alarm);
			bool passesFilters(const TA_Base_Bus::AlarmDataItem* alarm);

			bool operator == (const AlarmFilterSet& filterSet) const;

			bool isFilterPresent(const IAlarmFilter& filter) const;

			// yanrong++: reduce access to rights agent.
			void resetFilters();
			// ++yanrong.
		private:
			/*
			* Check if the passed alarm is in this filter set
			*/

			AlarmFilterSet(const AlarmFilterSet& theAlarmFilterSet);
			AlarmFilterSet& operator = (const AlarmFilterSet& theAlarmFilterSet);

			typedef std::vector<IAlarmFilter*>          FilterList;
			typedef std::map<std::string,FilterList* >  FilterCollection;
			
			FilterCollection                   m_filters;
			long                               m_filterCount;
	};
};
#endif // !defined(AlarmFilterSet_EA079B54_F04B_4704_A0F4_1060F40DC5B3__INCLUDED_)
