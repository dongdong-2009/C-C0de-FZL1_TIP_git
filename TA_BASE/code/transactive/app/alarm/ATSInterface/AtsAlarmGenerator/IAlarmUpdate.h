#pragma once
#ifndef __IALARM_UPDATE__
#define __IALARM_UPDATE__

class IAlarmUpdate
{
public:
	virtual bool insertAlarm(time_t t_submit, I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail) = 0;
	virtual bool updateAlarm(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail) = 0;
	virtual bool removeAlarm(std::string alarmID) = 0;
	virtual bool removeAllAlarms() = 0;
};

#endif //__IALARM_UPDATE__