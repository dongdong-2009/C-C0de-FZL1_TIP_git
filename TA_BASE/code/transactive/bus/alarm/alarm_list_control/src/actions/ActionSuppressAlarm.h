/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h $
 * @author:  Anita Lee
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that allows the user to create a suppression rule.
 */

#ifndef ACTION_SUPPRESS_ALARM_H
#define ACTION_SUPPRESS_ALARM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include <string>

#include "bus/alarm/alarm_list_control/src/IAlarmAction.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
namespace TA_Base_Core
{
    class AlarmModificationHelper;
}


namespace TA_Base_Bus
{

    class ActionSuppressAlarm: public virtual IAlarmAction
    {

    public:

        /**
         * Constructor
         *
         * @param bool - This indicates if this action should be available in the alarm list context
         *               menu. This will default to true and the action will be added to the menu.
         */
        ActionSuppressAlarm(bool isAvailableInContextMenu = true);


        /**
         * Destructor
         */
        virtual ~ActionSuppressAlarm();


        /**
         * isAvailableInContextMenu
         *
         * This should indicate whether this action is available in the alarm list context menu
         *
         * @return bool - true if this action should be in the context menu. False otherwise
         */
        virtual bool isAvailableInContextMenu() const
        {
            return m_isAvailableInContextMenu;
        }


        /**
         * getMenuEntry
         *
         * This will determine the entry that should appear in any menus that display this action
         *
         * @return string - The menu entry including & for the shortcut letter
         */
        virtual std::string getMenuEntry() const
        {
            //return "In&hibit...";
			return "抑制(&H)...";
        };


        /**
         * getActionControlId
         *
         * This should return the id to use for the menu item. This id will be used to determine what
         * item the user has selected from the context menu.
         *
         * @return unsigned int - A control id. This should not be the same as any other control id and must be within
         *                        the range 4000 - 4999.
         */
        virtual unsigned int getActionControlId() const
        {
            return INHIBIT_ALARM;
        };


		std::map<unsigned int, std::string> getSubActionControlIds() const
		{
			std::map<unsigned int, std::string> subActionIds;
			subActionIds.insert(std::pair<unsigned int, std::string> (INHIBIT_ALARM_BY_TYPE, "根据类型 &T") );
			subActionIds.insert(std::pair<unsigned int, std::string> (INHIBIT_ALARM_BY_DESCRIPTION, "根据描述 &D") );
			return subActionIds;
		};

		void setSubActionControlId(const unsigned int commandId)
		{
			m_currentSubActionControl = commandId;
		};

		unsigned int getSubActionControlId() const
		{
			return m_currentSubActionControl;
		};

        /**
         * isAvailable
         *
         * This is called when the application/list needs to determine if an action is available at
         * the current time. This is based on the user's access rights as well as which alarms are
         * currently selected.
         *
         * @param IAlarmRetriever& - The object to use to find details and perform actions.
         *
         * @return bool - true if the action is available to the user, false if it is not.
         */
        virtual bool isAvailable(IAlarmRetriever& alarmRetriever);


        /** 
         * performAction
         *
         * This is called when this action needs to be performed. This should only be called once checking
         * that the action isAvailable().
         *
         * @param IAlarmRetriever& - The object to use to find details and perform actions.
         *
         * @exception AlarmActionException - This should be thrown if any error is encountered. The message
         *                                   in the exception should be set to a suitable error message that
         *                                   can be displayed directly to the user.
         */
        virtual void performAction(IAlarmRetriever& alarmRetriever);

        /** 
         * performActionByType
         *
         * This is called when suppress/inhibit action by Type needs to be performed.
		 * This should only be called once checking that the action isAvailable().
         *
         * @param IAlarmRetriever& - The object to use to find details and perform actions.
         *
         * @exception AlarmActionException - This should be thrown if any error is encountered. The message
         *                                   in the exception should be set to a suitable error message that
         *                                   can be displayed directly to the user.
         */
        void performActionByType(IAlarmRetriever& alarmRetriever);

        /** 
         * performActionByDescription
         *
         * This is called when suppress/inhibit action by Description needs to be performed.
         * This should only be called once checking that the action isAvailable().
         *
         * @param IAlarmRetriever& - The object to use to find details and perform actions.
         *
         * @exception AlarmActionException - This should be thrown if any error is encountered. The message
         *                                   in the exception should be set to a suitable error message that
         *                                   can be displayed directly to the user.
         */
        void performActionByDescription(IAlarmRetriever& alarmRetriever);

    private:
        // Hidden as not required
        ActionSuppressAlarm(ActionSuppressAlarm&);
        ActionSuppressAlarm& operator=(const ActionSuppressAlarm&);

        /** 
         * createAlarmTypeSystemMap
         *
         * This is called to create a map of alarm type and system alarm for 
		 * quick check of system alarmwhen this action needs to be performed
         *
         */
		void createAlarmTypeKeySystemMap();

		bool canPerformActionOnAlarm(IAlarmRetriever& alarmRetriever);

		// Alarm type and isSystemAlarm map
		std::map<unsigned long, bool> m_alarmTypeKeySystemMap;
		// Alarm Details retreived from list
        
		//std::vector<TA_Base_Core::AlarmItem*> m_selectedAlarmDetails;
		
		boost::shared_ptr<TA_Base_Bus::AlarmDataItem> m_pAlarmDetails;
		// profile key of user activating the rule
		unsigned long m_profileKey;
		// session key of the current user activating the rule
		std::string m_sessionKey;
		std::string m_operatorName;

		TA_Base_Core::AlarmModificationHelper* m_alarmHelper;

        bool m_isAvailableInContextMenu;
		unsigned int m_currentSubActionControl;
    };

}
#endif // ACTION_SUPPRESS_ALARM_H
