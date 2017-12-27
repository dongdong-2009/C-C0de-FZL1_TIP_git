/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/IAlarmFilter.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  IAlarmFilter.h
//  Implementation of the Interface IAlarmFilter
//  Created on:      24-Jan-2004 04:19:38 PM
///////////////////////////////////////////////////////////

#if !defined(IAlarmFilter_72A4A09A_44C5_4a40_8D82_670286C0E74A__INCLUDED_)
#define IAlarmFilter_72A4A09A_44C5_4a40_8D82_670286C0E74A__INCLUDED_

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmItem.h"
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
/**
 * This interface provides the pure methods that need to be implementd by an
 * AlarmFilter class for it to be include in an AlarmFilterSet.
 * @version 1.0
 * @created 24-Jan-2004 04:19:38 PM
 */


namespace TA_Base_Bus
{
   class IAlarmFilter
   {
		public:

			enum EComparitor {COMPARE_LESS_THAN,
				              COMPARE_LESS_THAN_EQUAL,
							  COMPARE_EQUAL,
							  COMPARE_GREATER_THAN,
							  COMPARE_GREATER_THAN_EQUAL,
							  COMPARE_NOT_EQUAL,
							  COMPARE_BETWEEN_INCLUSIVE,
							  COMPARE_BETWEEN_EXCLUSIVE,
							  COMPARE_CONTAINS};

			// Constructor
			IAlarmFilter(EComparitor compareOp) : m_compareOp(compareOp) {};

			// Destructor
			virtual ~IAlarmFilter() {};

			/**
			 * Check if the specified alarm meets the filter requirements
			 * @param alarm
			 * 
			 */
			virtual bool includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm) =0;
			// Comparison operator used to check if the filter is the same as 
			// another.

			virtual bool includeAlarm(const TA_Base_Core::AlarmItem* alarm) =0;
			virtual bool includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm) = 0;

			virtual bool operator == (const IAlarmFilter&) const =0;

			// yanrong++: reduce access to rights agent.
			/**
			 * Filter reset.
			 *
			 * If the derived class caches data, it must overwrite this method to clean up caches.
			 */
			virtual void reset() {};
			// ++yanrong

		protected:

			EComparitor m_compareOp;
   };
};
#endif // !defined(IAlarmFilter_72A4A09A_44C5_4a40_8D82_670286C0E74A__INCLUDED_)
