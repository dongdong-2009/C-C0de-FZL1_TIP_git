#ifndef __DATA_GENERATOR_H__
#define __DATA_GENERATOR_H__

#pragma once
#include "CommonDefine.h"
#include "NonReEntrantThreadLockable.h"

enum ATSAlarmStr
{
	AssetName = 0,
	Description,
	LocationID,
	AlarmValue,
	AlarmType,
 	SystemName,
	MMSState,
// 	SubsystemName,
	
	Max_AtsAlarmStr
};

class CDataGenerator
{
public:
	~CDataGenerator(void);
	void LoadData(void);
	static CDataGenerator& getInstance();
	
	long GenNumberID(long lMaxValue=0);
	std::string GenStringID(long lMaxValue=0);
	std::string GenStringGUID();
	void getStringList(ATSAlarmStr alarmProp, STRING_LIST& objList);
	void getProfileConfig(I_ALARM_PROVIDER::ProfileInfo& profileInfo);
	
	I_ALARM_PROVIDER::AlarmDetail* createAlarm(CString& strAlarmID, CString& strAlarmValue, CString& strSystemName, 
							CString& strSubSystemName, CString& strDescription, CString& strAssestName,
							CString& strAlarmType, CString& strLocation,int nServerity, int nMMSState, int nAlarmState);

	I_ALARM_PROVIDER::AlarmDetail* createRandomAlarm();

	//ATSDateTime GenATSDateTime(); 
	int GetAutoThreadCount();
	int GetAutoSubmitInterval();
	int GetAutoAckCloseInterval();
	std::string GetAutoLocationID();
	int GetAutoAlarmCount();
	void getAllSubSystemList(STRING_LIST& subSystemList);
	void getSubSystemList(std::string& strSystem, STRING_LIST& subSystemList);


private: // functions
	CDataGenerator(void);
	void GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault, std::vector<std::string>& vecDataResult);
	std::string GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault);
	int GetConfigData(std::string strSection, std::string strKeyName, int nDefault);

	void GenerateDefaultTestString(std::string strKey, std::vector<std::string>& vecDataResult);
	std::string GenATSAlarmStringProp(ATSAlarmStr atsEVStr);
	std::string GenSubSystemName(std::string strSystem);
	
private: // variables	
	static TA_Base_Bus::NonReEntrantThreadLockable m_singletonLock;
	static CDataGenerator* m_pInstance;
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
	MAP_CONTAINER m_mapDataContainer;
	MAP_CONTAINER m_mapSubSystems;
};


#endif // __DATA_GENERATOR_H__