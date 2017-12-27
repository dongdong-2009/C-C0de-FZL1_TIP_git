// ISCSAlarmQueueItem.h: interface for the ISCSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUEITEM_H__807DE6D0_A109_4CAF_A1A1_31F80DE24F30__INCLUDED_)
#define AFX_QUEUEITEM_H__807DE6D0_A109_4CAF_A1A1_31F80DE24F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataStructInfo.h"

class ISCSAlarmQueueItem  
{
public:
	ISCSAlarmQueueItem();
	ISCSAlarmQueueItem(const MillisecondTime &_transactiveTime,
			  const char* _alarmID, // An unique alarm id to differentiate each alarm generated, max 38 characters.
		      unsigned long _ackTime, // the acknowledge time if any
		      unsigned long _closeTime, // the close time if any
		      const char* _assetName, // the asset name, max 20 characters.
		      unsigned long _alarmSeverity, // the severity of the alarm
		      const char* _alarmDescription, // the description of the alarm, max 200 characters.
		      const char* _alarmAcknowledgeBy, // the operator who acknowledge this alarm, if any
		      AlarmState _state, // the alarm state
		      const char* _locationId, // the location (Sect D.5 Station Code Name) that alarm is generated, max 4 characters.
		      const char* _parentAlarmID, // the parent alarm ID
		      const char* _avalancheHeadID, // related to the avalanche head if applicable
		      bool _isHeadOfAvalanche, // whether the alarm is the head of avalanche
		      bool _isChildOfAvalanche, // whether the alarm is the child of avalanche
		      MmsStateType _mmsState, // the MMS state
		      const char* _alarmValue, // the value of the data point related to the alarm, max 16 characters.
		      char _omAlarm, // O¡¯for Operation Alarm, ¡®M¡¯for maintenance 
		
		      time_t _updateTime, // the timestamp when ISCS trigger login/logout/change profile actions 
		      const char* _operatorId, // login operatorId max 10 characters.
		      const char* _profileId, // login operatorId max 10 characters.
	          const char* _loginLocation // Refer to Section D.5 Station Name Code, max 4 characters););
		      );

	~ISCSAlarmQueueItem();
	ISCSAlarmQueueItem(const ISCSAlarmQueueItem& item);
	ISCSAlarmQueueItem& operator=(const ISCSAlarmQueueItem& item);
	
public:
	MillisecondTime m_sourceTime; // The alarm generated time
	char m_alarmId[ALARM_ID_SIZE]; // An unique alarm id to differentiate each alarm generated, max 38 characters.
	unsigned long m_ackTime; // the acknowledge time if any
	unsigned long m_closeTime; // the close time if any
	char m_assetName[ASSERT_NAME_SIZE]; // the asset name, max 20 characters.
	unsigned long m_alarmSeverity; // the severity of the alarm
	char m_alarmDescription[ALARM_DESCRIPTION_SIZE]; // the description of the alarm, max 200 characters.
	char m_alarmAcknowledgeBy[ALARM_ACKNOWLEDGE_BY_SIZE]; // the operator who acknowledge this alarm, if any
	AlarmState m_state; // the alarm state
	char m_locationId[LOCATION_ID_SIZE]; // the location (Sect D.5 Station Code Name) that alarm is generated, max 4 characters.
	char m_parentAlarmId[ALARM_ID_SIZE]; // the parent alarm ID
	char m_avalancheHeadId[ALARM_ID_SIZE]; // related to the avalanche head if applicable
	bool m_isHeadOfAvalanche; // whether the alarm is the head of avalanche
	bool m_isChildOfAvalanche; // whether the alarm is the child of avalanche
	MmsStateType m_mmsState; // the MMS state
	char m_alarmValue[ALARM_VALUE_SIZE]; // the value of the data point related to the alarm, max 16 characters.
	char m_omAlarm; // O¡¯for Operation Alarm, ¡®M¡¯for maintenance 
	
	time_t m_updateTime; // the timestamp when ISCS trigger login/logout/change profile actions 
	char m_operatorId[OPERATOR_ID_SIZE]; // login operatorId max 10 characters.
	char m_profileId[PROFILE_ID_SIZE]; // login operatorId max 10 characters.
	char m_loginLocation[LOGIN_LOCATION_SIZE]; // Refer to Section D.5 Station Name Code, max 4 characters);

};

#endif // !defined(AFX_QUEUEITEM_H__807DE6D0_A109_4CAF_A1A1_31F80DE24F30__INCLUDED_)
