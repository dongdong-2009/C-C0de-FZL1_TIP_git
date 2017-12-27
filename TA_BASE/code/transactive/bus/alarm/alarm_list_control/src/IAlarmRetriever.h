/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/IAlarmRetriever.h $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is used by alarm actions to retrieve certain alarm details and
 * other classes that provide various functionality (eg check rights, retrieve objects from
 * the database etc).
 */

#if !defined(AFX_IAlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_IAlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

#include "core/alarm/src/AlarmModifyItem.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
namespace TA_Base_Bus
{
    // Forward declare
    class RightsChecker;
    class DatabaseCache;
    class AlarmListCtrl;

    class IAlarmRetriever
	{
	public:

        virtual ~IAlarmRetriever(){};


        /**
         * getSelectedAlarms
         *
         * This will retrieve all currently selected alarms from the alarm list
         *
         * @return vector<string> - The alarm uuid's for each selected alarm
         */
        virtual std::vector<std::string> getSelectedAlarms() = 0;
		virtual std::vector<TA_Base_Core::AlarmModifyItem*> getSelectedModItems() = 0;
		//virtual std::vector<TA_Base_Core::AlarmItem*>& getSelectedAlarmItems() = 0;
		

        /**
         * getAllVisibleAlarms
         *
         * This will retrieve all currently visible alarms from the alarm list
         *
         * @return vector<string> - The alarm uuid's for each alarm
         */
		virtual std::vector<std::string> getAllVisibleAlarms() = 0;
		virtual std::vector<TA_Base_Core::AlarmModifyItem*> getAllVisibleModItems() = 0;

        /**
         * getRightsChecker
         *
         * This will return the object that allows rights to be checked
         *
         * @return RightsChecker& - The object to use to determine if the user can perform a certain action
         */
        virtual RightsChecker& getRightsChecker() = 0;


        /**
         * getDatabaseCache
         *
         * This will return the object that caches database information for faster performance
         *
         * @return DatabaseCache& - The object to use to retrieve database information
         */
        virtual DatabaseCache& getDatabaseCache() = 0;


        /**
         * getAlarmListControl
         *
         * This will retrieve the actual alarm list so actions like setting filters and sorts
         * etc can be performed.
         *
         * @return AlarmListCtrl& - The list control object.
         */
        virtual AlarmListCtrl& getAlarmListControl() const = 0;


        /**
         * isActionAlreadyRunning
         *
         * This will indicate if an action is already running. Some actions require that other actions
         * are not running before they run. This method allows all actions to check the current running
         * status.
         *
         * @return bool - Indicates if an action is currently running
         */
        virtual bool isActionAlreadyRunning() =0;


        /**
         * setIsActionAlreadyRunning
         *
         * This will set if an action is already running. Some actions require that other actions
         * are not running before they run. This method allows actions that must run solo to indicate
         * this.
         *
         * @param bool - Indicates if an action is currently running
         * @param string - The name of the action so that it can be displayed in the status bar or similar.
         *                 This only needs to be set when setting isActionRunning to true.
         */
        virtual void setIsActionAlreadyRunning(bool isActionRunning, const std::string& actionName = "") =0;

		virtual bool deleteSharedAlarm(std::vector<std::string>& alarmIDs) =0;

		//++Noel
		virtual std::string getStringAttr(std::string strAlarmID, STRING_ATTR subItem) = 0;

		virtual unsigned long getNumberAttr(std::string strAlarmID, NUMBER_ATTR subItem) = 0;
		
		// This function returns a copy of the AlarmItem
		// Users must deallocate this alarm after use
		virtual AlarmDataItem* getAlarmItem(std::string& strAlarmID) = 0;
		
		// This function return a new AlarmModifyItem
		// Users must deallocate this alarm after use
		virtual TA_Base_Core::AlarmModifyItem* getAlarmModifyItem(std::string& strAlarmID) = 0;

		// deallocate the alarm modify items
		virtual void freeAlarmModifyItems(std::vector<TA_Base_Core::AlarmModifyItem*>& alarmModifyList) = 0;

		virtual void getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms) = 0;

		virtual bool isAlarmsVisible() = 0;
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_IAlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
