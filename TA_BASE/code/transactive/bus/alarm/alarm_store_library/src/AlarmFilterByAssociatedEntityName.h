/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByAssociatedEntityName.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByAssociatedEntityName.h
//  Implementation of the Class AlarmFilterByAssociatedEntityName
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterByAssociatedEntityName_WA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
#define AlarmFilterByAssociatedEntityName_WA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on location
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterByAssociatedEntityName : public IAlarmFilter 
	{

		public:
			AlarmFilterByAssociatedEntityName(EComparitor compareOp, const std::string &compareEntityName);
			virtual ~AlarmFilterByAssociatedEntityName();

		protected:
			/**
			 * Check if the specified alarm meets the filter requirements
			 * @param alarm
			 * 
			 */
			virtual bool includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			virtual bool includeAlarm(const TA_Base_Core::AlarmItem* alarm);
			virtual bool includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm); //++Noel

			// Comparison operator used to check if the filter is the same as 
			// another.
			virtual bool operator == (const IAlarmFilter&) const;

		private:
			AlarmFilterByAssociatedEntityName();

			AlarmFilterByAssociatedEntityName(const AlarmFilterByAssociatedEntityName& filter);
			AlarmFilterByAssociatedEntityName& operator = (const AlarmFilterByAssociatedEntityName& filter);
		private:
			std::string      m_compareEntityName;

	};
};

#endif // !defined(AlarmFilterByAssociatedEntityName_WA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
