/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmSortByField.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmSortByField.h
//  Implementation of the Class AlarmSortByField
//  Created on:      24-Jan-2004 04:19:57 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmSortByField_E88C79C4_A0AA_41a3_844A_BC843CA6C277__INCLUDED_)
#define AlarmSortByField_E88C79C4_A0AA_41a3_844A_BC843CA6C277__INCLUDED_

#include "IAlarmSort.h"

#include <sys/timeb.h>
#include <string>
namespace TA_Base_Bus
{
	/**
	 * This class allows sorting by a single field in the alarm details
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:57 PM
	 */
	class AlarmSortByField : public IAlarmSort 
	{
		public:

			typedef enum ESortField { SORT_OPEN_CLOSE,
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
									  //TD #12719++
									};

			AlarmSortByField(ESortField sortField,ESortDirection sortDirection);
			virtual ~AlarmSortByField();

		protected:
	
			/**
			 * Compares sort item.
			 * returns true to indicate match
			 * @param sortSet - Sort item to be compared with this one.
			 */
			virtual bool operator == (const IAlarmSort& sortItem) const;

			/**
			 * Compares alarm1 with alarm2.
			 * returns ECompareResult (SORT_BEFORE,SORT_AFTER,SORT_EQUAL)
			 * @param alarm1
			 * @param alarm2
			 */
			virtual ECompareResult compareAlarms(const TA_Base_Core::AlarmDetailCorbaDef& alarm1,
				                                 const TA_Base_Core::AlarmDetailCorbaDef& alarm2) const;

		private:

			AlarmSortByField();
			AlarmSortByField(const AlarmSortByField& sortItem);
			AlarmSortByField& operator = (const AlarmSortByField& sortItem);

			IAlarmSort::ECompareResult compare(const bool key1,const bool key2) const;
			IAlarmSort::ECompareResult compare(const unsigned long key1,const unsigned long key2) const;
			IAlarmSort::ECompareResult compare(const char* key1,const char* key2) const;
            IAlarmSort::ECompareResult compare(const TA_Base_Core::MillisecondTime& key1,const TA_Base_Core::MillisecondTime& key2) const;
		private:
			/**
			 * Order of sorting for this field
			 */
			ESortDirection m_sortDirection;
			/**
			 * Field to be used for sorting
			 */
			ESortField m_sortField;

			bool m_isUsingSourceTime;
	};
};
#endif // !defined(AlarmFieldSort_E88C79C4_A0AA_41a3_844A_BC843CA6C277__INCLUDED_)
