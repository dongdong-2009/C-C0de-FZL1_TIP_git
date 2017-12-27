/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/IAlarmSort.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  IAlarmSort.h
//  Implementation of the Interface IAlarmSort
//  Created on:      24-Jan-2004 04:20:06 PM
///////////////////////////////////////////////////////////

#if !defined(IAlarmSort_9BDB6398_65F1_4e37_A9E2_AC51028A1FFF__INCLUDED_)
#define IAlarmSort_9BDB6398_65F1_4e37_A9E2_AC51028A1FFF__INCLUDED_

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

namespace TA_Base_Bus
{
	/**
	 * Interface used by all sort classes
	 * @version 1.0
	 * @created 24-Jan-2004 04:20:06 PM
	 */
	class IAlarmSort
	{
		public:

			enum ECompareResult {SORT_BEFORE, SORT_AFTER, SORT_EQUAL};
			enum ESortDirection {SORT_ASCENDING, SORT_DESCENDING};

			/**
			 * This is an interface so requires a virtual destructor
			 */
			virtual ~IAlarmSort() {}


			/**
			 * Compares sort item.
			 * returns true to indicate match
			 * @param sortSet - Sort item to be compared with this one.
			 */
			virtual bool operator == (const IAlarmSort& sortItem) const =0;

			/**
			 * Compares alarm1 with alarm2.
			 * returns ECompareResult (SORT_BEFORE,SORT_AFTER,SORT_EQUAL)
			 * @param alarm1
			 * @param alarm2
			 */
		   virtual ECompareResult compareAlarms(const TA_Base_Core::AlarmDetailCorbaDef& alarm1, 
												const TA_Base_Core::AlarmDetailCorbaDef& alarm2) const =0;

	};
};
#endif // !defined(IAlarmSort_9BDB6398_65F1_4e37_A9E2_AC51028A1FFF__INCLUDED_)
