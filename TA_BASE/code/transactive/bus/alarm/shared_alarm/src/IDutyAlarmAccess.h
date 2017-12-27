// Author: Noel R. Tajanlangit


#ifndef __IDUTY_ALARM_ACCESS_H__
#define __IDUTY_ALARM_ACCESS_H__

#include "Bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include <list>

namespace TA_Base_Bus
{
	class IDutyAlarmAccess
	{
	public:
		virtual ~IDutyAlarmAccess(){};

		virtual bool insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist) = 0;

		virtual long removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource =ISCS_SUBSYSTEM_ID) = 0;

	};
}


#endif //__IDUTY_ALARM_ACCESS_H__