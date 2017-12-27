/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreManager.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreManager.h
//  Implementation of the Class AlarmStoreManager
//  Created on:      24-Jan-2004 04:17:50 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreManager_98406E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_)
#define AlarmStoreManager_98406E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_

#include "AlarmStoreManager.h"
#include "IAlarmUpdate.h"
#include "AlarmFilterSet.h"

#include <map>
#include <string>

#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include "bus/alarm/shared_alarm/src/ICheckVisibility.h"

#include "core/synchronisation/src/ReEntrantThreadLockableWithMutex.h"
#include "core/threads/src/Thread.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/alarm/alarm_store_library/src/AlarmFilterBySystemDisplay.h"

namespace TA_Base_Bus
{
	/**
	 * This class will manage the alarm store and its associated indexes.  It will
	 * ensure that index entries are maintained when the main store item is updated,
	 * inserted or removed
	 * @version 1.0
	 * @created 24-Jan-2004 04:17:50 PM
	 */
	class AlarmStoreManager : virtual public IAlarmUpdate, public ICheckVisibility
	{
		public:

			/**
			 * Constructor
			 * @param indexCallback
             * @param sessionUpdate The object that will report update changes on the SessionId.
             * @param dutyUpdate The object that will report update changes to duty
			 * 
			 */
			AlarmStoreManager(TA_Base_Bus::RightsLibrary* rightsLibrary);
			virtual ~AlarmStoreManager();

		public:

		    /** 
		     * sessionDetailsChanged
		     *
		     * This is a notification that *something* has changed about the session. It could be a duty update,
             * or a begin/end override. The store manager needs to check all active filters to determine if one
             * contains this sessionID. The function will return a boolean indication of whether the pre-filter 
             * was affected by the session change.
		     *
		     * @param session - the session ID that has had a change notification
             *
		     * @return A boolean indication of whether the pre-filter was reset by the session change.
		     */
		    bool sessionDetailsChanged( const std::string& session );
            /**
             * setSessionId
             *
             * Sets what session ID should be should be used to determine if an alarm is acknowledgeable.
             * 
             * @param sessionId - The session ID
             */
            void setSessionId(const std::string& sessionId);
			
			/* 
			 * Determines if the prefilter has changed to avoid multiple reloading when not necessary.
			 *
			 */
			bool hasPrefilterChanged(const AlarmFilterSet& filterSet);
			/**
			 * Sets a new pre-filter to the store.  This will require a total reload of alarm
			 * records if a pre-filter was already assigned.  If there was no previous pre-
			 * filter then just apply filter to current store.  Remeber to update index
			 * entries!
			 * @param filter    Set of pre-filters to be applied.
			 * 
			 */
			bool setPreFilter(AlarmFilterSet* filter);			
			//++ Added for dutychange
			void removePrefilter();

			bool passesPrefilter(const  TA_Base_Core::AlarmItem* alarm);
			bool passesPrefilter(const TA_Base_Core::AlarmDetailCorbaDef& alarm);

			//++ Noel
			bool passesPrefilter(const TA_Base_Bus::AlarmDataItem* alarm);

			AlarmFilterSet* m_preFilterSet;
			long m_lLoadAgents;

			virtual bool isAlarmVisible(TA_Base_Bus::AlarmDataItem* pAlarm);

			// DTL_Alarm_Display_Filter
			void setSystemDisplayFilter(const unsigned long& _locationkey);

		protected:
			/**
			 * New alarm
			 * @param alarm
			 * 
			 */
			virtual void newAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);

			/**
			 * An alarm has been updated
			 * @param alarm
			 * 
			 */
			virtual void updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdates);

			/**
			 * This method is called when all the alarms from a location should be removed.
			 * This would happen if we disconnect from an Agent.
			 * @param locationID    Location for which the alarms should be removed.
			 * 
			 */
			//virtual void clearAlarms(unsigned long location);

			/**
			 * shouldQueueAlarmUpdate
			 *
			 * check if alarmUpdate should be queued for later processing if AlarmStoreManager is busy
			 *
			 */
			virtual bool shouldQueueAlarmUpdate(){return false;};

    	private:

			AlarmStoreManager(const AlarmStoreManager& theAlarmStoreManager);
			AlarmStoreManager& operator = (const AlarmStoreManager& theAlarmStoreManager);

			TA_Base_Bus::RightsLibrary*  m_rightsLibrary;// Used to check Acknowledge permission
			bool m_alarmDefaultVisible;
			TA_Base_Core::ReEntrantThreadLockable    m_ProfileLock;

			std::string  m_sessionID;
			TA_Base_Core::ReEntrantThreadLockable    m_SessionLock;

			// DTL_Alarm_Display_Filter
			AlarmFilterBySystemDisplay* m_systemDisplayFilterSet;

			/*
			* subsystem key of the FAS		 
			*/
			unsigned long  m_FAS_Alarm_Type_Key;
	};
};
#endif // !defined(AlarmStoreManager_98406E84_61BC_41cd_AA36_0E533BDE359B__INCLUDED_)
