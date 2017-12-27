#include "StdAfx.h"
#include "DataGenerator.h"
#include <utility>
#include <cstdlib>
#include <ctime>
#include <objbase.h>
#include "ThreadGuard.h"
#include <boost/random.hpp>


using namespace std;

// Configuration path
const string g_strConfigPath = "C:\\Transactive\\config\\ATSAlarmGenerator\\ATSAlarmGenerator.ini";

// Constant Defaults //
const string g_strDefault = "Default";
const string g_strDefaultProfID = "";
const string g_strDefualtOpID = "Noel";
const string g_strDefaultLocID = "DT01";
const int	g_nDefaultDisplayOnly = 1;
const int g_nDefaultThreads			= 1;
const int g_nDefaultSubmitInterval	= 1000;
const int g_nDefaultAlarmCount		= 30;
const int g_nDefaultAckInterval		= 1000;

// Section Constant Value //
const string g_strSectionAuto		= "Auto Submit";
const string g_strSectionManual		= "ATS Alarm Data";
const string g_strSectionProfile	= "ATS Profile Info";
const string g_strSectionSubsystems = "ATS SubSystem";

// Key Names Constant
const string g_strLocationID		= "LocationID";
const string g_strThreads			= "Threads";
const string g_strSubmitInterval	= "SubmitInterval";
const string g_strAlarmCount		= "AlarmCount";
const string g_strAckInterval		= "AckCloseInterval";

const string g_strProfileID			= "ProfileID";
const string g_strOperatorID		= "OperatorID";
const string g_strLogInLocation		= "LogInLocation";
const string g_strDisplayOnly		= "DisplayOnly";

std::string ATSAlarmData[] = {
	"AssetName",
	"Description",
	"LocationID",
	"AlarmValue",
	"AlarmType",
  	"SystemName",
	"MMSState"
//  	"SubsystemName",
	};

typedef boost::minstd_rand rng_type;
typedef boost::uniform_smallint<int> dist_type;
typedef boost::variate_generator<rng_type&, dist_type> generator_type;

TA_Base_Bus::NonReEntrantThreadLockable CDataGenerator::m_singletonLock;
CDataGenerator* CDataGenerator::m_pInstance = NULL;
CDataGenerator& CDataGenerator::getInstance()
{
	TA_Base_Bus::ThreadGuard guard(m_singletonLock);
	if (m_pInstance == NULL)
	{
		m_pInstance = new CDataGenerator();
	}

	return *m_pInstance;
}

CDataGenerator::CDataGenerator(void)
{
	m_mapDataContainer.clear();
}

CDataGenerator::~CDataGenerator(void)
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CDataGenerator::LoadData(void)
{
	// ATS Alarm String Data
	for(int nIndex = 0; nIndex < Max_AtsAlarmStr; nIndex++)
	{
		STRING_LIST itVecData;
		GetConfigData(g_strSectionManual, ATSAlarmData[nIndex], g_strDefault, itVecData);
		m_mapDataContainer[ATSAlarmData[nIndex]] = itVecData;
		itVecData.clear();
	}

	// ATS Alarm SubSystems
	STRING_LIST systemlist = m_mapDataContainer[ATSAlarmData[SystemName]];
	for (int nIdx = 0; nIdx < (int)systemlist.size(); nIdx++)
	{
		STRING_LIST listSubSystem;
		GetConfigData(g_strSectionSubsystems, systemlist[nIdx], g_strDefault, listSubSystem);
		m_mapSubSystems[systemlist[nIdx]] = listSubSystem;
		listSubSystem.clear();
	}
}

int CDataGenerator::GetConfigData(std::string strSection, std::string strKeyName, int nDefault)
{
	int nResult;
	
	nResult = GetPrivateProfileInt(strSection.c_str(), strKeyName.c_str(), nDefault, g_strConfigPath.c_str());
	
	return nResult;
}

std::string CDataGenerator::GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault)
{
	std::string strResult = "";
	TCHAR pszRetData[1024];
	DWORD dwRet = 0;	

	dwRet = GetPrivateProfileString(strSection.c_str(), strKeyName.c_str(), strDefault.c_str(), pszRetData, sizeof(pszRetData), g_strConfigPath.c_str());
	if (dwRet != 0)
	{
		strResult = pszRetData;
	}

	return strResult;
}

void CDataGenerator::GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault, std::vector<std::string>& vecDataResult)
{
	char pszRetData[1024];
	DWORD dwRet = 0;

	dwRet = GetPrivateProfileString(strSection.c_str(), strKeyName.c_str(), strDefault.c_str(), pszRetData, sizeof(pszRetData), g_strConfigPath.c_str());
	
	if (dwRet != 0)
	{
		std::string strData;
		strData = pszRetData;
		if(strData.compare(strDefault) == 0)
		{
			GenerateDefaultTestString(strKeyName, vecDataResult);
		}
		else
		{
			strData.clear();
			int lSize =  strlen(pszRetData);
			for(int nIndex = 0; nIndex < lSize; nIndex++)
			{
				if (pszRetData[nIndex] == ',') // comma separator
				{
					vecDataResult.push_back(strData);
					strData.clear();
					
				}
				else if (pszRetData[nIndex] == '\0') // null terminated
				{
					vecDataResult.push_back(strData);
					strData.clear();
					break;
				}
				else
				{
					strData += pszRetData[nIndex];
				}
			}
			if (strData.length() != 0)
			{
				vecDataResult.push_back(strData);
			}
		}
	}

	//return vecDataResult;
}

void CDataGenerator::GenerateDefaultTestString(std::string strKey, std::vector<std::string>& vecDataResult)
{
	std::string stdDefault;
	for(int i = 0; i < 5; i++)
	{
		stdDefault = strKey;
		char szNum[10];
		sprintf(szNum, "-%2d", i+1);
		stdDefault += std::string(szNum);
		vecDataResult.push_back(stdDefault);

		stdDefault.clear();
	}
}

long CDataGenerator::GenNumberID(long lMaxValue)
{
	long lResult = 0;

	srand( (unsigned)time(NULL) );
 	lResult =  (rand() % lMaxValue) + 1;

// 	dist_type dist(1, 1000);
// 	rng_type generator;
// 	generator_type gen(generator, dist);
// 
// 	lResult = gen() % lMaxValue;

	return lResult;
}

std::string CDataGenerator::GenStringID(long lMaxValue)
{
	char szData[100];
	long lRandomID = 0;
	std::string strReturn = "";

	lRandomID = GenNumberID(lMaxValue);
	sprintf(szData, "%ld", lRandomID);
	strReturn = szData;

	return strReturn;
}

std::string CDataGenerator::GenStringGUID()
{
//	std::string strGUIDRet = "";
//	char szData[256];
// 
// 	static const char alphanum[] =
// 		"0123456789"
// 		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
// 		"abcdefghijklmnopqrstuvwxyz";
// 
// 	int len = 20;
// 	for (int i = 0; i < len; ++i) {
// 		szData[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
// 	}
// 	szData[len] = 0;
// 
// 	strGUIDRet = szData;

	// Create the new UUID
	UUID uuid;
	UuidCreate(&uuid);

	// convert the UUID to a string
	// Note, UuidToString does not free memory!
	UCHAR* buf;
	UuidToString(&uuid, &buf);
	// Copy the string into the a std::string
	std::string strGUIDRet((char *)buf);
	// Free the allocated memory
	RpcStringFree(&buf);

	return strGUIDRet;
}

std::string CDataGenerator::GenATSAlarmStringProp(ATSAlarmStr atsEVStr)
{
	std::vector<std::string> strProp;
	std::vector<std::string>::iterator it;

	strProp = m_mapDataContainer[ATSAlarmData[atsEVStr]];

	long nIndex = GenNumberID(strProp.size());
	--nIndex;

	return strProp[nIndex];
}

std::string CDataGenerator::GenSubSystemName(std::string strSystem)
{
	STRING_LIST subsystemlist = m_mapSubSystems[strSystem];
	int nIdx = (int)GenNumberID(subsystemlist.size()-1);

	return subsystemlist[nIdx];
}

int CDataGenerator::GetAutoThreadCount()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	return GetConfigData(g_strSectionAuto, g_strThreads, g_nDefaultThreads);
}

int CDataGenerator::GetAutoSubmitInterval()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	return GetConfigData(g_strSectionAuto, g_strSubmitInterval, g_nDefaultSubmitInterval);
}

int CDataGenerator::GetAutoAckCloseInterval()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	return GetConfigData(g_strSectionAuto, g_strAckInterval, g_nDefaultAckInterval);
}

std::string CDataGenerator::GetAutoLocationID()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	return GetConfigData(g_strSectionAuto, g_strLocationID, g_strDefaultLocID);
}

int CDataGenerator::GetAutoAlarmCount()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	return GetConfigData(g_strSectionAuto, g_strAlarmCount, g_nDefaultAlarmCount);
}

void CDataGenerator::getStringList(ATSAlarmStr alarmProp, STRING_LIST& objList)
{
	if (alarmProp > Max_AtsAlarmStr) return;
	objList = m_mapDataContainer[ATSAlarmData[alarmProp]];
}

void CDataGenerator::getProfileConfig(I_ALARM_PROVIDER::ProfileInfo& profileInfo)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	profileInfo.profileId = GetConfigData(g_strSectionProfile, g_strProfileID, g_strDefaultProfID);
	profileInfo.operatorId = GetConfigData(g_strSectionProfile, g_strOperatorID, g_strDefualtOpID);
	profileInfo.loginLocation = GetConfigData(g_strSectionProfile, g_strLogInLocation, g_strDefaultLocID);
	profileInfo.displayOnly = GetConfigData(g_strSectionProfile, g_strDisplayOnly, g_nDefaultDisplayOnly);
}

I_ALARM_PROVIDER::AlarmDetail* CDataGenerator::createAlarm(CString& strAlarmID, CString& strAlarmValue, CString& strSystemName, 
										   CString& strSubSystemName, CString& strDescription, CString& strAssestName,
										   CString& strAlarmType, CString& strLocation,int nServerity, int nMMSState, int nAlarmState){
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	I_ALARM_PROVIDER::AlarmDetail * pAlarmDetail = NULL;

	pAlarmDetail = new I_ALARM_PROVIDER::AlarmDetail;
	pAlarmDetail->alarmID = strAlarmID.GetBuffer(0);
	pAlarmDetail->sourceTime.time = time(NULL);
	pAlarmDetail->ackTime = 0;
	pAlarmDetail->alarmDescription = strDescription.GetBuffer(0);
	pAlarmDetail->alarmAcknowledgeBy = "";	 
	pAlarmDetail->alarmValue = strAlarmValue.GetBuffer(0);
	pAlarmDetail->alarmSeverity = nServerity;
	pAlarmDetail->locationId = strLocation.GetBuffer(0);
	pAlarmDetail->assetName = strAssestName.GetBuffer(0);
	pAlarmDetail->avalancheHeadID = "";
	pAlarmDetail->parentAlarmID = "";
	pAlarmDetail->state = (I_ALARM_PROVIDER::AlarmState)nAlarmState;
	pAlarmDetail->alarmType = "50009";//strAlarmType.GetBuffer(0);
	pAlarmDetail->subsytemkey = strSubSystemName.GetBuffer(0);
	pAlarmDetail->systemkey = strSystemName.GetBuffer(0);
	pAlarmDetail->mmsState = (I_ALARM_PROVIDER::MmsStateType)nMMSState; //I_ATS_ISCS_ALARM::MMS_NONE;
	pAlarmDetail->omAlarm = 79;
	pAlarmDetail->alarmSource = VA_SUBSYSTEM_ID;
	

	return pAlarmDetail;
}

I_ALARM_PROVIDER::AlarmDetail* CDataGenerator::createRandomAlarm()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	I_ALARM_PROVIDER::AlarmDetail * pAlarmDetail = NULL;

	pAlarmDetail = new I_ALARM_PROVIDER::AlarmDetail;
	pAlarmDetail->alarmID = GenStringGUID();
	pAlarmDetail->sourceTime.time = time(NULL);
	pAlarmDetail->ackTime = 0;
	pAlarmDetail->alarmDescription = GenATSAlarmStringProp(Description);
	pAlarmDetail->alarmAcknowledgeBy = "";	 
	pAlarmDetail->alarmValue = GenATSAlarmStringProp(AlarmValue);
	pAlarmDetail->alarmSeverity = GenNumberID(3);
	pAlarmDetail->locationId = GetAutoLocationID();
	pAlarmDetail->assetName = GenATSAlarmStringProp(AssetName);
	pAlarmDetail->avalancheHeadID = "";
	pAlarmDetail->parentAlarmID = "";
	pAlarmDetail->state = I_ALARM_PROVIDER::AlarmOpen;
	pAlarmDetail->alarmType = "50009"; //GenATSAlarmStringProp(AlarmType);
	pAlarmDetail->systemkey = GenATSAlarmStringProp(SystemName);
	pAlarmDetail->subsytemkey = GenSubSystemName(pAlarmDetail->systemkey);
	pAlarmDetail->mmsState = I_ALARM_PROVIDER::MMS_NONE;
	pAlarmDetail->omAlarm = 79;
	pAlarmDetail->alarmSource = VA_SUBSYSTEM_ID;

	return pAlarmDetail;

	return NULL;
}

void CDataGenerator::getAllSubSystemList(STRING_LIST& subSystemList)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	STRING_LIST strSystems = m_mapDataContainer[ATSAlarmData[SystemName]];
	for (int nIdx = 0; nIdx < (int)strSystems.size(); nIdx++)
	{
		for (int nIdxSub = 0; nIdxSub < (int)(strSystems[nIdx].size()); nIdxSub++)
		{
			std::string strSubSystem = m_mapSubSystems[strSystems[nIdx]][nIdxSub].c_str();
			subSystemList.push_back(strSubSystem);
		}
	}
}

void CDataGenerator::getSubSystemList(std::string& strSystem, STRING_LIST& subSystemList)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	subSystemList = m_mapSubSystems[strSystem];
}
