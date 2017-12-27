/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreServant.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreServant.h
//  Implementation of the Class AlarmStoreServant
//  Created on:      24-Jan-2004 04:12:03 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreServant_59682BC3_C948_45ff_BF68_AA5DB6DA4D29__INCLUDED_)
#define AlarmStoreServant_59682BC3_C948_45ff_BF68_AA5DB6DA4D29__INCLUDED_

#include "core/corba/src/ServantBase.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/alarm/idl/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmStoreCallbackCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "AlarmAgentInterfaceManager.h"
#include "AlarmStoreManager.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreUpdateListener.h"
#include "core/synchronisation/src/Condition.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include <map>

#include "ATSAlarmObserver.h"
#include "bus/alarm/alarm_store_library/src/IWatchObject.h"

namespace TA_Base_Bus
{
	/**
	 * This class provides the implementation to the specified IDL for clients
	 * accessing the alarm store.
	 */
	class AlarmStoreServant : virtual public TA_Base_Core::ServantBase, 
							  virtual public POA_TA_Base_Core::IAlarmStoreCorbaDef, 
							  //public ISessionUpdateObserver, 
							  //public IDutyUpdateObserver,
							  public IRightsChangeObserver,
							  public IWatchedObject
	{
		public:
			AlarmStoreServant(const std::string &storeEntityName, AlarmStoreUpdateListener& listener);
			virtual ~AlarmStoreServant();
			virtual bool check();


		protected:
			/**
			 * This command enables a new index to be created.  It would be possible to re-use
			 * an existing one if it is already defined as index filter modification will not
			 * be permitted.
			 * @param filter
			 * @param sort
			 * @param callback
			 * 
			 */
			virtual TA_Base_Core::IAlarmStoreCorbaDef::IndexReference createIndex(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter, 
									 const TA_Base_Core::IAlarmStoreCorbaDef::SortInfo& sort, 
									// TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback,
									 unsigned long cs_pos,
									 bool isHighestSeverityNeeded,bool isDefault);
			/**
			 * Set the store to monitor all or a single agent
			 * @param allAgents    true to monitor all agents, false to monitor local agent
			 * only
			 * 
			 */
			virtual void setStoreMonitorAllLocations();
			virtual void setStoreMonitorLocation(unsigned long location);
			virtual void setStoreMonitorLocations(const TA_Base_Core::IAlarmStoreCorbaDef::LocationList& locations);

			/**
			 * This method will set/replace the current store prefilter.
			 */
			virtual void setStorePreFilter(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter);

		    /** 
             * dutyChanged
             *
             * Notifies the observer of a duty change
             *
             * @param session - the session ID for which duty has changed
             *
             */
		    virtual void dutyChanged(const std::string& session);
			virtual void dutyUpdate(std::map<unsigned long, std::list<unsigned long> > & LocationSubSytemList);
			virtual void removeDuty();
			//++Noel
			virtual void OnRightsChange(std::string sessionId, ERightsChangeType changeType, DutyChanges& changes );
			

		    /** 
		     * sessionOverrideBegin
		     *
		     * The profile is one that is being ADDED to the session.
		     *
		     * @param session
		     * @param profile
		     *
		     */		
		    virtual void sessionOverrideBegin(const std::string& session, unsigned long profile);

		    /** 
		     * sessionOverrideEnd
		     *
		     * Note that the profile is the REMAINING profile, NOT the
		     * one that has been removed.
		     *
		     * @param session
		     * @param profile
		     *
		     */
		    virtual void sessionOverrideEnd(const std::string& session, unsigned long profile);

			//////////////////////////////////////////////////////////
			// The following three functions is used for stress test
			/*
			* newAlarm
			*
			* @param alarmDetail - indicate the new alarm detailed data
			*/
			virtual void newAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarmDetail);
			
			/*
			* updateAlarm
			*
			* @param alarmUpdate - indicate the updated alarm detailed data
			*/
			virtual void updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);

			/*
			* aliveCheck
			*
			* this is used to check if alarmStore work normally
			*/
			CORBA::Long getAliveTimeStamp();

			/**
			 * getAlarm
			 *
			 * This method returns the details of a specified alarm.
			 * @param alarmID
			 */
			virtual TA_Base_Core::AlarmDetailCorbaDef* getAlarm(const char* alarmID);

			/*
			* init
			*
			* init shared memory and connect to all necessary alarm agents
			*/
			virtual void init();
            virtual void RemoveAlarms(const TA_Base_Core::StringSequence& p_alarms);
			//////////////////////////////////////////////////////////

		private:
			AlarmStoreServant();
			AlarmStoreServant(const AlarmStoreServant& theAlarmStoreServant);
			AlarmStoreServant& operator = (const AlarmStoreServant& theAlarmStoreServant);
		    /**
		     * setSessionId
		     *
		     * Tells the store what session ID to use when determining what the highest severity alarm
		     * sound is. If this method is called, alarm sounds will only be played for alarms that are
		     * acknowledgable. Otherwise, the alarm sound will be played for all alarms.
		     */
            void setSessionId(const char* sessionId);
			AlarmFilterSet* createFilter(const TA_Base_Core::IAlarmStoreCorbaDef::FilterInfo& filter);
		 
			class ActionThread : public TA_Base_Core::Thread
			{
			public:
				ActionThread(AlarmStoreServant& parent, int actionType)
					: m_parent(parent)
					, m_actionType(actionType)
					, m_terminated(false)
				{
					
				}

				void run();
				void terminate();

				//void activateAction();

			private:
				AlarmStoreServant& m_parent;
				//TA_Base_Core::Condition m_wakeCondition;
				volatile bool m_terminated;
				int m_actionType;
			};
			
			friend class ActionThread;
			ActionThread* m_rebuildAlarmThread;
			/////////////////////////////////////////////////////////////
			
		private:			
			AlarmStoreManager*              m_alarmStoreManager;
			AlarmAgentInterfaceManager*     m_alarmAgentInterfaceManager;
			TA_Base_Bus::RightsLibrary*		m_rightsLibrary;

			typedef std::map<unsigned long, std::string>	 AlarmAgentNameMap;
			AlarmAgentNameMap				m_allAlarmAgentNameMap;
			bool							m_isOCCLocation;
			unsigned long					m_currentLoginLocaitonKey;

			ATSAlarmObserver* m_ATSAlarmObserver;
			// ly++ add for check work
			time_t				m_aliaveTimeStamp;
			//time_t				m_TimeBefore;
			CheckWorker*		m_pCheckWorker;

	};
};  // namespace TA_Base_App
#endif // !defined(AlarmStoreServant_59682BC3_C948_45ff_BF68_AA5DB6DA4D29__INCLUDED_)
