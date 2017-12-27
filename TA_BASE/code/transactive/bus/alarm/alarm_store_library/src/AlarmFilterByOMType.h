/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByOMType.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByLocation.h
//  Implementation of the Class AlarmFilterByLocation
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterByOMType_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
#define AlarmFilterByOMType_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on state of the alarm
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterByOMType : public IAlarmFilter 
	{
		public:

			enum EAlarmOMType
			{
				OMTYPE_OPERATION   = 0,
				OMTYPE_MAINTENANCE = 1,
				OMTYPE_BOTH        = 2,
				OMTYPE_ALL         = 3
			};
			
			AlarmFilterByOMType(EComparitor compareOp,EAlarmOMType compareOMType);
			virtual ~AlarmFilterByOMType();

		protected:
			/**
			 * Check if the specified alarm meets the filter requirements
			 * @param alarm
			 * 
			 */
			virtual bool includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			virtual bool includeAlarm(const TA_Base_Core::AlarmItem* alarm);
			virtual bool includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm);
			// Comparison operator used to check if the filter is the same as 
			// another.
			virtual bool operator == (const IAlarmFilter&) const;

		private:
			AlarmFilterByOMType();
			AlarmFilterByOMType(const AlarmFilterByOMType& filter);
			AlarmFilterByOMType& operator = (const AlarmFilterByOMType& filter);

		    bool alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Core::AlarmDetailCorbaDef& p_alarm ) const;
			bool alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Core::AlarmItem* p_alarm ) const;
			bool alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Bus::AlarmDataItem* p_alarm ) const;
		private:
			EAlarmOMType         m_compareOMType;
	};
};

#endif // !defined(AlarmFilterByOMType_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
