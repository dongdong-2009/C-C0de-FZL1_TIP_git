/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByAssociatedEntityKey.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByLocation.h
//  Implementation of the Class AlarmFilterByLocation
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterByAssociatedEntityKey_UA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
#define AlarmFilterByAssociatedEntityKey_UA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_

#include "IAlarmFilter.h"


namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on associated entity key
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterByAssociatedEntityKey : public IAlarmFilter 
	{

		public:
			AlarmFilterByAssociatedEntityKey(EComparitor compareOp,unsigned long compareEntityKey);
			virtual ~AlarmFilterByAssociatedEntityKey();

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
			AlarmFilterByAssociatedEntityKey();
			
			AlarmFilterByAssociatedEntityKey(const AlarmFilterByAssociatedEntityKey& filter);
			AlarmFilterByAssociatedEntityKey& operator = (const AlarmFilterByAssociatedEntityKey& filter);
		private:
			unsigned long         m_compareEntityKey;
	};
};

#endif // !defined(AlarmFilterByAssociatedEntityKey_UA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
