/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmStore.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  * 
  * This class provides the interface to the alarm store. It connects to it and sends information
  * to it.
  */

#ifndef ALARM_STORE_MON_09_02_2004_H
#define ALARM_STORE_MON_09_02_2004_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "bus/alarm/alarm_list_control/src/CommonStructures.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"

#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
    struct AlarmDetailCorbaDef;
}

namespace TA_Base_Bus
{
    class Filter;
    class Sort;

    class AlarmStore : public virtual TA_Base_Core::Thread
    {
    public:
       /**
        * Constructor
        *
        * @param Filter - This is the prefilter to apply. This must not be changed regularly and should just
        *                 filter out alarms that this list does not need to see. Eg filter on a particular
        *                 profile. This would only change when the user changed their login.
        * @param vector<unsigned long> - A vector of location keys. This lets the list know which alarms it
        *                                should be displaying. For all locations the vector should be empty.
        * @param Sort - The sort column and order the list control should start the list in
        * @param Filter - This is any active filters the user wants applied. This filter can be changed regularly.
        * @param sessionId - If set, this is the session ID that the alarm store should use, for alarm sound
        *                    purposes, to determine if an alarm is acknowledgable
        *
        * @exception - DataException - Thrown if there is an error locating the AlarmStore entity data
        * @exception - DatabaseException - Thrown if there is an error locating the AlarmStore entity data
        */
        AlarmStore( const Filter& preFilter,
                    const std::vector<unsigned long>& locations,
                    const Sort& sort,
                    const Filter& filter,
                    const std::string& sessionId = "",
					bool isHighestSeverityNeeded = false);


        /**
         * Destructor
         */
        ~AlarmStore();


        /**
         * run
         *
         * This should not be called directly. This is called indirectly by the start() method.
         * The thread should be started whenever the alarm store cannot be found
         */
		virtual void run();


        /**
         * terminate
         *
         * This should not be called directly. This is called indirectly by the terminateAndWait() method.
         */
		virtual void terminate();


        /**
         * isAlarmStoreValid
         *
         * This indicates if the alarm store is currently valid. If it is not (ie false)
         * then no alarms or alarm updates are being retrieved or sent.
         *
         * @return bool - True if the alarm store is valid and functioning.
         */
        bool isAlarmStoreValid() const
        {
            return m_bAlarmStoreValid;
        };


        /**
         * updatePreFilterLocationAndSession
         *
         * This will update the prefilter, locations, and session ID the alarm store contains. Note that setting
         * this affects all alarm store users on the same machine.
         *
         * @param Filter - This is the prefilter to apply. This must not be changed regularly and should just
         *                 filter out alarms that this list does not need to see. Eg filter on a particular
         *                 profile. This would only change when the user changed their login.
         * @param vector<unsigned long> - A vector of location keys. This lets the list know which alarms it
         *                                should be displaying. For all locations the vector should be empty.
         */
        virtual void updatePreFilterLocationsAndSession( const Filter& preFilter,
                                                  const std::vector<unsigned long>& locations,
                                                  const std::string& sessionID);

        /**
         * setFilter
         *
         * This sets a new filter on the alarm store
         *
         * @param Filter - The new filter to set
         *
         * @exception AlarmListCtrlException - Thrown if the filter is invalid
         */
        void setFilter(const Filter& filter,const int cs_pos,bool isDefault);

        /**
         * getCurrentFilter
         *
         * This will retrieve the current filter the list is using.
         *
         * @return Filter - The current filter being used
         */
        Filter getCurrentFilter() const
        {
            return m_filter;
        }
        
        /**
         * setSort
         *
         * This sets a new sort on the alarm store
         *
         * @param Sort - The new sort to set
         *
         * @exception AlarmListCtrlException - Thrown if the current filter is invalid
         */
//        void setSort(const Sort& sort);

        /**
         * getCurrentSort
         *
         * This will retrieve the current sort the list is using.
         *
         * @return Sort - The current sort being used
         */
        Sort getCurrentSort() const
        {
            return m_sort;
        }


		// This is the prefilter the alarm store is created with
        Filter m_preFilter;

		// This is the current locations the alarm store is looking for
        std::vector<unsigned long> m_locations;

		// The session ID the alarm store should use to determine which alarms are acknowledgeable,
        // or "" if no session ID should be used.
        std::string m_sessionId;

		//////////////////////////////////////////////////////////
		// The following three functions is used for stress test
		/*
		* newAlarm
		*
		* @param alarmDetail - indicate the new alarm detailed data
		*/
		void newAlarm(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail);
		
		/*
		* updateAlarm
		*
		* @param alarmUpdate - indicate the updated alarm detailed data
		*/
		void updateAlarm(TA_Base_Core::AlarmUpdateCorbaDef alarmUpdate);

		/*
		* getASTimeStamp
		*
		* this is used to check if alarmStore work normally
		*/
		long getASTimeStamp();

		//////////////////////////////////////////////////////////

		bool deleteSharedAlarm(std::vector<std::string>& alarmIDs);

    private:

        /**
         * convertFilterToCorbaDefVersion
         * 
         * This will convert the filter into a CORBA sequence to be passed to the Alarm Store
         *
         * @param Filter - The filter to convert
         *
         * @return IAlarmStoreCorbaDef::FilterInfo - The sequence to pass to the Alarm Store
         */
        TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo convertFilterToCorbaDefVersion(const Filter& filterToConvert) const;

        /**
         * convertSortToCorbaDefVersion
         * 
         * This will convert the sort into a CORBA sequence to be passed to the Alarm Store
         *
         * @param Sort - The sort to convert
         *
         * @return IAlarmStoreCorbaDef::SortInfo - The sequence to pass to the Alarm Store
         */
        TA_Base_Core::IAlarmStoreCorbaDef::SortInfo convertSortToCorbaDefVersion(const Sort& sortToConvert) const;

        /**
         * setFilterAndSort
         *
         * This updates the filter and sort that are currently stored in the alarm store.
         *
         * @exception AlarmListCtrlException - Thrown if the filter is invalid
         */
        void setFilterAndSort(const int cs_pos,bool isDefault);

    private:

		volatile bool m_bTerminate;

        volatile bool m_bAlarmStoreValid;

        TA_Base_Core::Condition m_resolveAlarmStore;

        // This is the index the Alarm store gives us when we create an index with it. This will
        // be used to perform other calls on the alarm store.
        TA_Base_Core::IAlarmStoreCorbaDef::IndexReference m_alarmStoreIndexId;

        // This is the alarm store itself.
        TA_Base_Core::IAlarmStoreCorbaDef_var m_alarmStore;
        
        // The thread lock for the m_alarmStore
        mutable TA_Base_Core::NonReEntrantThreadLockable m_alarmStoreLock;
    
        // This is the currently applied sort
        Sort m_sort;

        // This is the currently applied filter
        Filter m_filter;
        
        // The alarm store port number run param
        static const std::string PORT_NUMBER;

		bool m_isHighestSeverityNeeded;
		bool	m_bFilterCreated;
		TA_Base_Core::Condition	m_alarmStoreReady;
    };
}

#endif // ALARM_STORE_MON_09_02_2004_H
