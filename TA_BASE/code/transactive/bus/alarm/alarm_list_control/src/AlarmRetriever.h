/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmRetriever.h $
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

#if !defined(AFX_AlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_AlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/alarm_list_control/src/IAlarmRetriever.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/TAAssert.h"
//++Noel
#include "AlarmSelectionWrap.h"

namespace TA_Base_Bus
{
    class AlarmRetriever : public virtual IAlarmRetriever
	{
	public:

        /**
         * Constructor
         */
        AlarmRetriever();


        /**
         * Destructor
         */
        virtual ~AlarmRetriever();


        /**
         * clearCache
         *
         * This will clear any cached data so it is generated fresh
         */
        virtual void clearCache();


        /**
         * getCurrentAction
         *
         * This will return the string indicating what action is currently running so that it can be displayed
         * in a title bar, status bar etc.
         * 
         * @return string - The text to display. This will be an empty string if no action is running
         */
        virtual std::string getCurrentAction();


        /**
         * setAlarmListCtrl
         *
         * This is called by the alarm list control to set a pointer to itself in here. This allows this
         * class to perform callbacks on it such as retrieving selected alarms or the database cache
         *
         * @param AlarmListCtrl& - The alarm list control object
         */
        virtual void setAlarmListCtrl(AlarmListCtrl& alarmList);


        /**
         * getSelectedAlarms
         *
         * This will retrieve all currently selected alarms from the alarm list
         *
         * @return vector<string> - The alarm uuid's for each selected alarm
         */
        virtual std::vector<std::string> getSelectedAlarms();
		virtual std::vector<TA_Base_Core::AlarmModifyItem*> getSelectedModItems();


        /**
         * getAllVisibleAlarms
         *
         * This will retrieve all currently visible alarms from the alarm list
         *
         * @return vector<string> - The alarm uuid's for each alarm
         */
		virtual std::vector<std::string> getAllVisibleAlarms();
		virtual std::vector<TA_Base_Core::AlarmModifyItem*> getAllVisibleModItems();

        /**
         * getRightsChecker
         *
         * This will return the object that allows rights to be checked
         *
         * @return RightsChecker& - The object to use to determine if the user can perform a certain action
         */
        virtual RightsChecker& getRightsChecker();


        /**
         * getDatabaseCache
         *
         * This will return the object that caches database information for faster performance
         *
         * @return DatabaseCache& - The object to use to retrieve database information
         */
        virtual DatabaseCache& getDatabaseCache();


        /**
         * getAlarmListControl
         *
         * This will retrieve the actual alarm list so actions like setting filters and sorts
         * etc can be performed.
         *
         * @return AlarmListCtrl& - The list control object.
         */
        virtual AlarmListCtrl& getAlarmListControl() const;


        /**
         * isActionAlreadyRunning
         *
         * This will indicate if an action is already running. Some actions require that other actions
         * are not running before they run. This method allows all actions to check the current running
         * status.
         *
         * @return bool - Indicates if an action is currently running
         */
        virtual bool isActionAlreadyRunning();


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
        virtual void setIsActionAlreadyRunning(bool isActionRunning, const std::string& actionName = "");


		//++Noel
		virtual std::string getStringAttr(std::string strAlarmID, STRING_ATTR subItem);	
		
		virtual unsigned long getNumberAttr(std::string strAlarmID, NUMBER_ATTR subItem);

		// This function returns a copy of the AlarmItem
		// Users must deallocate this alarm after use
		virtual AlarmDataItem* getAlarmItem(std::string& strAlarmID);
		void setSelectionRetriever(TA_Base_Bus::AlarmSelectionWrap* pSelectionWrap);
		// This function return a new AlarmModifyItem
		// Users must deallocate this alarm after use
		virtual TA_Base_Core::AlarmModifyItem* getAlarmModifyItem(std::string& strAlarmID);
		
		// deallocate the alarm modify items
		// This function must be called to free the std::vector<TA_Base_Core::AlarmModifyItem*>
		virtual void freeAlarmModifyItems(std::vector<TA_Base_Core::AlarmModifyItem*>& alarmModifyList);

		// Retrieve the child alarms
		virtual void getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms);

		virtual bool isAlarmsVisible();
		virtual bool deleteSharedAlarm(std::vector<std::string>& alarmIDs);

	private: // functions
// 		TA_Base_Core::AlarmItem* _getAlarmItem(std::string strAlarmID);
// 		TA_Base_Core::AlarmModifyItem* _getAlarmModifyItem(std::string strAlarmID);
// 		void _getSelectedModifyItems(std::vector<TA_Base_Core::AlarmModifyItem*>& selectedAlarmsModItem);

    private:
        AlarmListCtrl* m_alarmListCtrl;
		TA_Base_Bus::AlarmSelectionWrap* m_pSelectionWrap;
        // Cache the selected alarms. Calling clear cache will reset this.

		typedef std::vector<std::string> CacheSelectedAlarms;
        // This locks the alarm list so they cannot be cleared while they are being retrieved etc
        TA_Base_Core::ReEntrantThreadLockable m_alarmListLock;

        // Keep a record of when certain actions are running
        bool m_isActionRunning;
        static TA_Base_Core::ReEntrantThreadLockable m_actionLock;
        std::string m_actionName;

        // Cache the selected alarms. Calling clear cache will reset this.
		bool m_areSelAlarmsCached;
        bool m_areVisAlarmIDsCached;

		//++noel
		std::vector<std::string> m_selectedAlarms;
		std::vector<TA_Base_Core::AlarmModifyItem*> m_selectedAlarmsModItem;

		std::vector<std::string> m_visibleAlarms;
		std::vector<TA_Base_Core::AlarmModifyItem*> m_visibleAlarmsModItem;

		CacheSelectedAlarms m_cacheSelectedAlarms;
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_AlarmRetriever_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
