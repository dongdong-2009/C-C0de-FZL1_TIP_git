/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySystemDisplay.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  */

#ifndef __ALARM_FILTER_BY_SYSTEM_DISPLAY_H_INCLUDED__
#define __ALARM_FILTER_BY_SYSTEM_DISPLAY_H_INCLUDED__

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	typedef enum 
	{
		EXCLUDE = 0,
		INCLUDE
	} displayCond;

	struct displayFilter 
	{
		unsigned long filterKey;

		unsigned long alarmType;

		unsigned long subSystemKey;

		unsigned long alarmLocationKey;

		displayCond displayCondition;
	};

	typedef std::vector<displayFilter*> Vec_displayFilter_T;

	class AlarmFilterBySystemDisplay
		: public IAlarmFilter 
	{
	public: // Methods
		AlarmFilterBySystemDisplay(Vec_displayFilter_T pDisplayFilter);
		virtual ~AlarmFilterBySystemDisplay();

	//protected:
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
	private: // Methods
		AlarmFilterBySystemDisplay();
		AlarmFilterBySystemDisplay(const AlarmFilterBySystemDisplay& filter);
		AlarmFilterBySystemDisplay& operator = (const AlarmFilterBySystemDisplay& filter);

	private: // Methods
		Vec_displayFilter_T m_pDisplayFilter;

	};


}//close namespace TA_Base_Core

#endif // __ALARM_FILTER_BY_SYSTEM_DISPLAY_H_INCLUDED__
