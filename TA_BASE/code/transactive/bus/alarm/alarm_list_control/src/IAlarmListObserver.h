/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/IAlarmListObserver.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class allows the alarm list control to pass information to and from an application
 * that is using it.
 */

#if !defined(AFX_IAlarmListObserver_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_IAlarmListObserver_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{

	class IAlarmListObserver  
	{
	public:

        enum Availability
        {
            ENABLED, DISABLED
        };

        // The unsigned int is one of the command id's from
        // bus/alarm/alarm_list_control/src/actions/ActionResources.h and the
        // Availability enumeration indicates whether the item is disabled or enabled.
        typedef std::map<unsigned int, Availability> ItemAvailability;

        
        virtual ~IAlarmListObserver(){};


	   /**
		* updateAlarmCounts
		* 
	    * The virtual method that updates the container's total acknowledge and unacknowledge values.
		*
		* @param The number of alarms in the system.
		*
		* @param The number of unacknowledged alarms in the system.
		*
		* @param The indicator whether the store has more than the alarms displayed
		*
		* @param The indicator whether store is still busy loading all alarms
		*/
		virtual void updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms) = 0;


       /**
        * updateAlarmSound
        * 
        * The virtual method that updates the container's alarm severity tones.
        *
        * @param The severity tone to update as an unsigned long.
        * @param True if new alarms were inserted, and false otherwise.
        */
        virtual void updateAlarmSound(unsigned long severity, bool wereAlarmsAdded) = 0;


        /**
         * updateAvailableAlarmActions
         *
         * This will pass in the current availability of each alarm action. This will be called whenever
         * the alarm selection changes or the current alarms change.
         *
         * @param ItemAvailability - This should pass the availability of every possible alarm action currently
         *                           being used.
         */
        virtual void updateAvailableAlarmActions(const ItemAvailability& availability) =0;

        
        /**
         * setCurrentDescriptionText
         *
         * This will pass some appropriate text to be shown by the observer in a status bar or the like.
         * This string will hold the description of the selected alarm. If multiple alarms are selected it
         * will indicate how many alarms are selected
         *
         * @param CString - The text to be displayed
         */
        virtual void setCurrentDescriptionText(const CString& description) =0;


       /**
		* getProgressBar
		* 
	    * The virtual method that retrieves the progress bar (if any) and then updates it. Once
        * the progress bar is finished with the method finishWithProgressBar() is called.
		*
		* @return 
		*/
		virtual CProgressCtrl* getProgressBar() = 0;


	   /**
		* finishedWithProgressBar
		* 
	    * This will be called after a progress bar has finished updating. This will only be called if
        * the call to getProgressBar does not return NULL.
		*/
        virtual void finishedWithProgressBar() = 0;

       /**
		* notifyNoMatchingAlarms
		* 
	    * This will be called if no alarms match the current filter. 
		*/
        virtual void notifyNoMatchingAlarms() = 0;

		/**
		 * notifyFilterIndexReset
		 *
		 * This will be called if alarmlist control is notified of by alarm store to unmap 
		 * so filter will be set to default
		 */
		virtual void notifyFilterReset() = 0;
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_IAlarmListObserver_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
