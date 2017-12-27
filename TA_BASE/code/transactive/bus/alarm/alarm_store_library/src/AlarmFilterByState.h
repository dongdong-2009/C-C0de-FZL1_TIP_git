/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByState.h $
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

#if !defined(AlarmFilterByState_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
#define AlarmFilterByState_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on state of the alarm
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterByState : public IAlarmFilter 
	{
		public:

			enum EAlarmState
			{
				ALL_OPEN     = 0,
				OPEN_UNACKED = 1,
				OPEN_ACKED   = 2,
				CLOSED       = 3,
				ALL_UNACKED  = 4,
				ANY          = 5
			};
			
			AlarmFilterByState(EComparitor compareOp,EAlarmState compareState);
			virtual ~AlarmFilterByState();

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
			AlarmFilterByState();
			AlarmFilterByState(const AlarmFilterByState& filter);
			AlarmFilterByState& operator = (const AlarmFilterByState& filter);

		    bool alarmStatesMatch( EAlarmState p_state, const TA_Base_Core::AlarmDetailCorbaDef& p_alarm ) const;
			bool alarmStatesMatch( EAlarmState p_state, const TA_Base_Core::AlarmItem* p_alarm ) const;
			bool alarmStatesMatch( EAlarmState p_state, const TA_Base_Bus::AlarmDataItem* p_alarm ) const; //++Noel

		private:
			EAlarmState         m_compareState;
	};
};

#endif // !defined(AlarmFilterByState_RA4E5BDB_B8F2_4fdd_ABB4_C417C947D248__INCLUDED_)
