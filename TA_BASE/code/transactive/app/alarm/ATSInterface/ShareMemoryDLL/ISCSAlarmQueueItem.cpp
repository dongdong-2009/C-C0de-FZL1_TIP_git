// ISCSAlarmQueueItem.cpp: implementation of the ISCSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISCSAlarmQueueItem.h"
  

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ISCSAlarmQueueItem

ISCSAlarmQueueItem::ISCSAlarmQueueItem(const MillisecondTime &_sourceTime, 
					 const char* _alarmId, // An unique alarm id to differentiate each alarm generated, max 38 characters.
					 unsigned long _ackTime, // the acknowledge time if any
					 unsigned long _closeTime, // the close time if any
					 const char* _assetName, // the asset name, max 20 characters.
					 unsigned long _alarmSeverity, // the severity of the alarm
					 const char* _alarmDescription, // the description of the alarm, max 200 characters.
					 const char* _alarmAcknowledgeBy, // the operator who acknowledge this alarm, if any
					 AlarmState _state, // the alarm state
					 const char* _locationId, // the location (Sect D.5 Station Code Name) that alarm is generated, max 4 characters.
					 const char* _parentAlarmId, // the parent alarm ID
					 const char* _avalancheHeadId, // related to the avalanche head if applicable
					 bool _isHeadOfAvalanche, // whether the alarm is the head of avalanche
					 bool _isChildOfAvalanche, // whether the alarm is the child of avalanche
					 MmsStateType _mmsState, // the MMS state
					 const char* _alarmValue, // the value of the data point related to the alarm, max 16 characters.
					 char _omAlarm, // O¡¯for Operation Alarm, ¡®M¡¯for maintenance 

 					 time_t _updateTime, // the timestamp when ISCS trigger login/logout/change profile actions 
 					 const char* _operatorId, // login operatorId max 10 characters.
 					 const char* _profileId, // login operatorId max 10 characters.
 					 const char* _loginLocation // Refer to Section D.5 Station Name Code, max 4 characters);
					) : 
m_sourceTime(_sourceTime),
m_ackTime(_ackTime),
m_closeTime(_closeTime),
m_alarmSeverity(_alarmSeverity),
m_state(_state),
m_isHeadOfAvalanche(_isHeadOfAvalanche), 
m_isChildOfAvalanche(_isChildOfAvalanche),
m_mmsState(_mmsState), 
m_omAlarm(_omAlarm),
m_updateTime(_updateTime )
{
	 
	 memset(m_alarmId, 0, ALARM_ID_SIZE);
	 // m_alarmID = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_alarmID) + 1));
	 strcpy(m_alarmId, _alarmId);

	 memset(m_assetName, 0, ASSERT_NAME_SIZE);
	 // m_assetName = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_assetName) + 1));
	 strcpy(m_assetName, _assetName);

	 memset(m_alarmDescription, 0, ALARM_DESCRIPTION_SIZE);
	 // m_alarmDescription = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_alarmDescription) + 1));
	 strcpy(m_alarmDescription, _alarmDescription);
	 
	 memset(m_alarmAcknowledgeBy, 0, ALARM_ACKNOWLEDGE_BY_SIZE);
	 // m_alarmAcknowledgeBy = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_alarmAcknowledgeBy) + 1));
	 strcpy(m_alarmAcknowledgeBy, _alarmAcknowledgeBy);

	 memset(m_locationId, 0, LOCATION_ID_SIZE);
	 // m_locationId = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_locationId) + 1));
	 strcpy(m_locationId, _locationId);
	 
	 //m_parentAlarmId = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_parentAlarmId) + 1));
	 memset(m_parentAlarmId, 0, ALARM_ID_SIZE);
	 strcpy(m_parentAlarmId, _parentAlarmId);
	
	
	 //m_avalancheHeadId = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_avalancheHeadId) + 1));
	 memset(m_avalancheHeadId, 0, ALARM_ID_SIZE);
	 strcpy(m_avalancheHeadId, _avalancheHeadId);

	 memset(m_alarmValue, 0, ALARM_VALUE_SIZE);
	 // m_alarmValue = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_alarmValue) + 1));
	 strcpy(m_alarmValue, _alarmValue);
	 
	 memset(m_operatorId, 0, OPERATOR_ID_SIZE);
 	 // m_operatorId = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_operatorId) + 1));
 	 strcpy(m_operatorId, _operatorId);

	 memset(m_profileId, 0, PROFILE_ID_SIZE);
	 // m_profileId = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_profileId) + 1));
	 strcpy(m_profileId, _profileId);
 

	 memset(m_loginLocation, 0, LOGIN_LOCATION_SIZE);
	 // m_loginLocation = reinterpret_cast<char*>(TA_Base_Core::Shmem::alloc(strlen(_loginLocation) + 1));
	 strcpy(m_loginLocation, _loginLocation);
}

ISCSAlarmQueueItem::ISCSAlarmQueueItem() 
{
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	memset(m_assetName, 0, ASSERT_NAME_SIZE);
	memset(m_alarmDescription, 0, ALARM_DESCRIPTION_SIZE);
	memset(m_alarmAcknowledgeBy, 0, ALARM_ACKNOWLEDGE_BY_SIZE);
	memset(m_locationId, 0, LOCATION_ID_SIZE);
	memset(m_alarmValue, 0, ALARM_VALUE_SIZE);
	memset(m_operatorId, 0, OPERATOR_ID_SIZE);
	memset(m_profileId, 0, PROFILE_ID_SIZE);
	memset(m_loginLocation, 0, LOGIN_LOCATION_SIZE);

	memset(m_parentAlarmId, 0, ALARM_ID_SIZE);
	memset(m_avalancheHeadId, 0, ALARM_ID_SIZE);

	 

	m_sourceTime.millitm = 0;
	m_sourceTime.time = 0;
	m_ackTime = 0;
	m_closeTime = 0;
	m_alarmSeverity = 0;
	m_state = AlarmOpen;
	m_isHeadOfAvalanche = false;
	m_isChildOfAvalanche = false;
	m_mmsState = MMS_NONE;
	m_omAlarm = 'O';
	m_updateTime = 0;
}

ISCSAlarmQueueItem::~ISCSAlarmQueueItem()
{
	//ReleaseMemory();
}

ISCSAlarmQueueItem::ISCSAlarmQueueItem(const ISCSAlarmQueueItem& item)
{
	m_sourceTime = item.m_sourceTime;
    memcpy(m_alarmId, item.m_alarmId, ALARM_ID_SIZE);
	m_ackTime = item.m_ackTime;
	m_closeTime = item.m_ackTime;
	memcpy(m_assetName, item.m_assetName, ASSERT_NAME_SIZE);
	m_alarmSeverity = item.m_alarmSeverity;
	memcpy(m_alarmDescription, item.m_alarmDescription, ALARM_DESCRIPTION_SIZE);
	memcpy(m_alarmAcknowledgeBy, item.m_alarmAcknowledgeBy, ALARM_ACKNOWLEDGE_BY_SIZE);
	m_state = item.m_state;
	memcpy(m_locationId, item.m_locationId, LOCATION_ID_SIZE);
	memcpy(m_parentAlarmId, item.m_parentAlarmId, ALARM_ID_SIZE);
	memcpy(m_avalancheHeadId, item.m_avalancheHeadId, ALARM_ID_SIZE);
	m_isHeadOfAvalanche = item.m_isHeadOfAvalanche;
	m_isChildOfAvalanche = item.m_isChildOfAvalanche;
	m_mmsState = item.m_mmsState;
	memcpy(m_alarmValue, item.m_alarmValue, ALARM_VALUE_SIZE);
	m_omAlarm = item.m_omAlarm;

	m_updateTime = item.m_updateTime;
	memcpy(m_operatorId, item.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_profileId, item.m_profileId, PROFILE_ID_SIZE);
	memcpy(m_loginLocation, item.m_loginLocation, LOGIN_LOCATION_SIZE);
}

// size_t ISCSAlarmQueueItem::assign(char** pBuf, const char* value, size_t bufSize/*=0*/)
// {
// 	if(value == NULL)
// 	{
// 		return 0 ;
// 	}
// 
// 	size_t cpLen = 0;
// 	if (0 == bufSize)
// 		cpLen = strlen(value);
// 	else
// 		cpLen = strlen(value) >= bufSize ? bufSize - 1 : strlen(value);
// 
// 	*pBuf = (char*)TA_Base_Core::Shmem::alloc(cpLen+1);
// 	strncpy(*pBuf, value, cpLen);
// 	(*pBuf)[cpLen] = 0;
// 	
// 	return cpLen;
// }

ISCSAlarmQueueItem& ISCSAlarmQueueItem::operator=(const ISCSAlarmQueueItem& item)
{
	if (this == &item)
	{
		return *this;
	}

 
  	
	m_sourceTime = item.m_sourceTime;
	memcpy(m_alarmId, item.m_alarmId, ALARM_ID_SIZE);
	m_ackTime = item.m_ackTime;
	m_closeTime = item.m_ackTime;
	memcpy(m_assetName, item.m_assetName, ASSERT_NAME_SIZE);
	m_alarmSeverity = item.m_alarmSeverity;
	memcpy(m_alarmDescription, item.m_alarmDescription, ALARM_DESCRIPTION_SIZE);
	memcpy(m_alarmAcknowledgeBy, item.m_alarmAcknowledgeBy, ALARM_ACKNOWLEDGE_BY_SIZE);
	m_state = item.m_state;
	memcpy(m_locationId, item.m_locationId, LOCATION_ID_SIZE);

	memcpy(m_parentAlarmId, item.m_parentAlarmId, ALARM_ID_SIZE);
	memcpy(m_avalancheHeadId, item.m_avalancheHeadId, ALARM_ID_SIZE);

	m_isHeadOfAvalanche = item.m_isHeadOfAvalanche;
	m_isChildOfAvalanche = item.m_isChildOfAvalanche;
	m_mmsState = item.m_mmsState;
	memcpy(m_alarmValue, item.m_alarmValue, ALARM_VALUE_SIZE);
	m_omAlarm = item.m_omAlarm;

	m_updateTime = item.m_updateTime;
	memcpy(m_operatorId, item.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_profileId, item.m_profileId, PROFILE_ID_SIZE);
	memcpy(m_loginLocation, item.m_loginLocation, LOGIN_LOCATION_SIZE);

	return *this;
}




