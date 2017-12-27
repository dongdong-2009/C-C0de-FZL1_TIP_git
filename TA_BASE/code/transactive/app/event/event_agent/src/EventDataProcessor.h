/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/EventDataProcessor.h $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * EventDataProcessor is a thread for 1).retreiving all the data for event 2)processing some data in event
 *
 */

#pragma once

#include <map>

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/event/src/EventItem.h"
#include "core/event/IDL/src/EventDetailCorbaDef.h"

namespace TA_Base_App
{
	class EventDataProcessor
	{
	public:

		/** 
		 * destructor
		 */
		~EventDataProcessor(void);

		/** 
		 * getInstance
		 * 
		 * get the singleton instance
		 */
		static EventDataProcessor& getInstance();

		
		/** 
		 * ReleaseInstance
		 * 
		 * Release the singleton instance
		 */
		static void releaseInstance();

		/** 
		 * initialize
		 * 
		 * Load data from database
		 */
		void initialize();

		TA_Base_Core::EventItem* processEventInfo(const TA_Base_Core::EventInfoCorbaDef& eventInfo);
		void processEventInfo(const TA_Base_Core::EventInfoSequence& eventInfos, TA_Base_Core::T_EventItemList& retVal);

	private:

		/** 
		 * constructor
		 */
		EventDataProcessor(void);
		EventDataProcessor(const EventDataProcessor& rhs);
		EventDataProcessor& operator=(const EventDataProcessor &); // Assignment operator
		/** 
		 * initOCCLoactionKey
		 *
		 * get OCC location key from DB and save it to m_OCCLocationKey
		 */
		void initLocationKeys();

		/** 
		 * initOperatorData
		 *
		 * get all the operator data from DB and save to cache: m_operatorMap
		 */
		void initOperatorData();

		/** 
		 * updateSessionInfo
		 *
		 * retrieve operator name from cache and adjust profileID
		 * 
		 * @param event - the event details
		 */
		void updateSessionInfo(TA_Base_Core::EventItem* pEvent);

		/** 
		 * updateOperator
		 *
		 * retrieve operator name from cache and adjust profileID
		 * 
		 * @param event - the event details
		 */
		void updateOperator(TA_Base_Core::EventItem* pEvent);

		/** 
		 * processLocKeyForAlarm
		 *
		 * for processing location key for alarm event
		 * @return the location key 
		 * @param sourceLocKey the current location key from runtime parameter
		 * @param etyLocationKey the location key from entity
		 */
		unsigned long processLocKeyForAlarm(unsigned long sourceLocKey, unsigned long etyLocationKey) ;
		
		/** 
		 * processLocKeyForEvent
		 *
		 * process location key for non-alarm event
		 * @return unsigned long location key 
		 * @param sourceLocKey the location key from current runtime parameter
		 * @param etyLocationKey the location key from entity data
		 */
		unsigned long processLocKeyForEvent(unsigned long sourceLocKey, unsigned long etyLocationKey)  ;

		/** 
		 * releaseOperatorCache
		 *
		 * release operator cache
		 */ 
		void releaseOperatorCache();
	
		void InitAlarmSeverity();
		
		std::string getSeverityNameByKey(unsigned long severityKey);
		
		void releaseAlarmSeverityCache();

		TA_Base_Core::EventItem* processEventInternal(const TA_Base_Core::EventInfoCorbaDef& eventInfo);

		TA_Base_Core::IEntityData* getEntityData(unsigned long ulEntityKey);

		void assignNewValue(char* pszDest, const char* pszSource, size_t maxBuffer);
		
		typedef std::map<unsigned long, TA_Base_Core::IEntityData* > T_EntityMap;
		typedef std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*> T_AlarmSeverityMap;
		typedef std::map<unsigned long, TA_Base_Core::IOperator*> T_OperatorMap;

		static EventDataProcessor* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		T_OperatorMap m_operatorMap;

		TA_Base_Core::NonReEntrantThreadLockable m_lock;

		T_AlarmSeverityMap m_alarmSeverityMap;
		bool m_alarmSeverityLoaded;

		unsigned long m_OCCLocationKey;

		//++Noel
		unsigned long m_myLocation;
		TA_Base_Bus::AuthenticationLibrary* m_authLib;
		T_EntityMap m_entityCache;

		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
		bool m_bInitialized;

	};

};
