/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.cpp $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is responsible for checking if action is enabled on external alarms. 
 */

#include "ExtAlarmActionChecker.h"
#include "core/alarm/src/AlarmItem.h"

namespace TA_Base_Bus
{	
	//Config path
	const std::string g_strConfigPath = "C:\\Transactive\\config\\Alarm.ini";

	// Default Values
	const std::string g_strDefaultVAClientPath = "D:\\VA\\MICSClient.VAClientShell.exe";

	// Section Constant Value //
	const std::string g_strSectionVA		= "VA_SUBSYSTEM";

	// Key Names Constant
	const std::string g_strVAClientPath		= "VAClient";

	ExtAlarmActionChecker* ExtAlarmActionChecker::m_pInstance = NULL;


	ExtAlarmActionChecker& ExtAlarmActionChecker::getInstance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new ExtAlarmActionChecker();
		}

		return *m_pInstance;
	}

	void ExtAlarmActionChecker::freeInstance()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	ExtAlarmActionChecker::ExtAlarmActionChecker()
	{
		// ------------------==== NOTE ====------------------
		// This information should be provided by the third party vendor so
		// we can dynamically enable actions for them based on their supported 
		// functionality.
		// For now we will hard code this part

		T_ActionRightsMap actionRights;
		actionRights[VIEW_COMMENT] = false;     
		actionRights[ACKNOWLEDGE_ALARM] = true;     
		actionRights[EDIT_COMMENT] = false;     
		actionRights[RELATED_SCHEMATIC] = false;             
		actionRights[CLOSE_AND_ACK_ALARM] = true;           
		actionRights[RELATED_INSPECTOR] = true;             
		actionRights[RELATED_DSS] = false;                   
		actionRights[POSTPONE_DSS] = false;                  
		actionRights[RELATED_TREND] = false;                 
		actionRights[RELATED_EVENTS] = false;                
		actionRights[JOB_REQUEST] = false;                   
		actionRights[INCIDENT_LOG_SHEET] = false;            
		actionRights[ACKNOWLEDGE_ALL] = true;               
		actionRights[RELATED_AVALANCHE_EVENTS] = false;      
		actionRights[INHIBIT_ALARM] = false;                 
		actionRights[INHIBIT_ALARM_BY_TYPE] = false;     	 
		actionRights[INHIBIT_ALARM_BY_DESCRIPTION] = false;     
		actionRights[EVENTS_VIEWER] = false;                 
		actionRights[RELATED_PLAN_INSTANCE] = false;         

		m_actionSourceMap[VA_SUBSYSTEM_ID] = actionRights;

		m_strVAClientPath = GetConfigData(g_strSectionVA, g_strVAClientPath, g_strDefaultVAClientPath);
	}

	bool ExtAlarmActionChecker::isActionEnabled(AlarmDataItem* pAlarmItem, unsigned long actionID)
	{
		bool bRetVal = false;

		T_ActionSourceMap::iterator itFindSource = m_actionSourceMap.find(pAlarmItem->dataSource);
		if (itFindSource != m_actionSourceMap.end())
		{
			T_ActionRightsMap rightsMap = itFindSource->second;
			T_ActionRightsMap::iterator itRightsMap = rightsMap.find(actionID);
			if (itRightsMap != rightsMap.end())
			{
				bRetVal = itRightsMap->second;
			}
		}

		return bRetVal;
	}

	std::string ExtAlarmActionChecker::getVAClientPath()
	{
		return m_strVAClientPath;
	}

	std::string ExtAlarmActionChecker::GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault)
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

} //end TA_Base_Bus
