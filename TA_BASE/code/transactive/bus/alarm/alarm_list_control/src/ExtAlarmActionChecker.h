/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is responsible for checking if action is enabled on external alarms.
 */

#ifndef __EXTERNAL_ALARM_ACTION_CHECKER_H_INCLUDED__
#define __EXTERNAL_ALARM_ACTION_CHECKER_H_INCLUDED__

#include "bus/alarm/alarm_list_control/src/actions/ActionResources.h"
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include <map>
#include <string>

namespace TA_Base_Bus
{
	class ExtAlarmActionChecker
	{
	public:
		~ExtAlarmActionChecker(){};

		static ExtAlarmActionChecker& getInstance();

		static void freeInstance();

		bool isActionEnabled(AlarmDataItem* pAlarmItem, unsigned long actionID);

		std::string getVAClientPath();
	private:
		ExtAlarmActionChecker();
		std::string GetConfigData(std::string strSection, std::string strKeyName, std::string strDefault);

		// Disable
		ExtAlarmActionChecker(const ExtAlarmActionChecker& rhs);
		ExtAlarmActionChecker& operator=(const ExtAlarmActionChecker& rhs);

		typedef std::map<unsigned long, bool> T_ActionRightsMap;
		typedef std::map<unsigned long , T_ActionRightsMap > T_ActionSourceMap;

		T_ActionSourceMap m_actionSourceMap;

		static ExtAlarmActionChecker* m_pInstance;
		std::string m_strVAClientPath;
	};


} //end TA_Base_Bus


#endif //__EXTERNAL_ALARM_ACTION_CHECKER_H_INCLUDED__
