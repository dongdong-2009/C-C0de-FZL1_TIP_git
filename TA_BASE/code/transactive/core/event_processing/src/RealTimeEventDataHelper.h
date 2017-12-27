/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/RealTimeEventDataHelper.h $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * RealTimeEventDataHelper is a thread for 1).retreiving all the data for event 2)processing some data in event
 *
 */

#pragma once

#include <map>
#include "core/threads/src/Thread.h"
#include "core/event_processing/src/IEvent.h"
#include "core/data_access_interface/src/MessageTypeData.h"
#include "core/event/src/EventItem.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/MessageTypeEventData.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/event_processing/src/ISessionHelper.h"
#include "core/message/src/AuditMessageSender.h"

namespace TA_Base_Core
{
	class RealTimeEventDataHelper
	{
	public:

		/** 
		 * destructor
		 */
		~RealTimeEventDataHelper(void);

		/** 
		 * getInstance
		 * 
		 * get the singleton instance
		 */
		static RealTimeEventDataHelper& getInstance();

		
		/** 
		 * ReleaseInstance
		 * 
		 * Release the singleton instance
		 */
		static void releaseInstance();

		/** 
		 * prepareAllDataForEvent
		 *
		 * prepare all data for the event
		 *
		 * @param event the event to be filled with all the necessary data
		 */ 
		void prepareAllDataForEvent(EventItemEx * event,ISessionHelper* sessionHelper);


	private:

		/** 
		 * constructor
		 */
		RealTimeEventDataHelper(void);

		/** 
		 * modifyValue
		 *
		 * modify value string , using the real eventTypeName to substitute the symbol.
		 * 
		 * @param value         the value as input and output
		 * @param eventTypeName the event type name of the event 
		 */ 
		void modifyValue(std::string & value, std::string eventTypeName);

		/** 
		 * processSubsystemKeyNPhisicalSubsystemKey
		 *
		 * processing the subsystemKey and physicalSubsystemKey
		 *
		 * @param subsystemKey the input and output of subsystemKey
		 * @param physicalSubsystemKey the input and output of physicalSubsystemKey
		 */ 
		void processSubsystemKeyNPhisicalSubsystemKey(unsigned long & subsystemKey,long & physical_subsytem_key);

		/** 
		 * getEntityInfoByKey
		 * 
		 * retrieve entity related data from DB
		 * @param entityKey the input of entitykey
		 * @param subsystemKey  the output of subsystemKey
		 * @param physicalSubsystemKey the output of physicalSubsystemKey
		 * @param entityLocationKey the output of entity locationKey
		 */
		void getEntityInfoByKey(unsigned long entityKey, unsigned long & subsystemKey,long & physicalSubsystemKey, unsigned long & entityLocationKey );

		/** 
		 * getSessionInfoByKey
		 *
		 * retrieve session data from cache
		 * 
		 * @param sessionId  the input of session ID
		 * @param operatorKey the output of operator key
		 * @param profileID   the output of profile ID
		 * @param sessionLocationKey the output of session location key
		 */
		void getSessionInfoByKey(ISessionHelper* sessionHelper,std::string sessionId,unsigned long& operatorKey, long& profileID, unsigned long& sessionLocationKey );

		/** 
		 * getOperatorNameNAdjustProfileID
		 *
		 * retrieve operator name from cache and adjust profileID
		 * 
		 * @param operatorKey  the input of operator key
		 * @param operatorName the output of operator name
		 * @param profileID   the profileId need to be adjusted(If is_system, profileId=-1)
		 */
		void getOperatorNameNAdjustProfileID(unsigned long operatorKey, std::string & operatorName,long& profileID);
		
		/** 
		 * getMessageTypeInfoByKey
		 *
		 * retrieve me_message_type(plus actionId) data from cache
		 * 
		 * @param eventTypeKey  the input of message type key
		 * @param viewLevel the output of viewLevel of this message type
		 * @param eventTypeName   the output name of this message type
		 * @param actionId       the output of actionId of this message type
		 */
		void getMessageTypeInfoByKey(unsigned long eventTypeKey, unsigned long & viewLevel,
			std::string & eventTypeName, unsigned long & actionId);

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
		 * initOCCLoactionKey
		 *
		 * get OCC location key from DB and save it to m_OCCLocationKey
		 */
		void initOCCLoactionKey();

		/** 
		 * initOperatorData
		 *
		 * get all the operator data from DB and save to cache: m_operatorMap
		 */
		void initOperatorData();

		/**  
		 * initMessageTypeData
		 * 
		 * get all the me_message_type+actionID data from DB and save to m_msgTypeMap
		 */
		void initMessageTypeData();

		/** 
		 * releaseOperatorCache
		 *
		 * release operator cache
		 */ 
		void releaseOperatorCache();
				
		/** 
		 * releaseMessageTypeCache
		 *
		 * release MessageType cache
		 */ 
		void releaseMessageTypeCache();


		/** 
		 * processLocKeyForEvent
		 *
		 * process location key for non-alarm event
		 * @return unsigned long location key 
		 * @param sourceLocKey the location key from current runtime parameter
		 * @param etyLocationKey the location key from entity data
		 */
		unsigned long processLocKeyForEvent(unsigned long sourceLocKey, unsigned long etyLocationKey)  ;

		void InitAlarmSeverity();
		
		std::string getSeverityNameByKey(unsigned long severityKey);
		
		void releaseAlarmSeverityCache();

		bool IsAlarmSeverityLoaded();

		static RealTimeEventDataHelper* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		std::map<unsigned long, TA_Base_Core::IOperator*> m_operatorMap;

		std::map<unsigned long, TA_Base_Core::MessageTypeEventData*> m_msgTypeMap;

		TA_Base_Core::NonReEntrantThreadLockable m_lock;

		std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*> m_alarmSeverityMap;
		bool m_alarmSeverityLoaded;

		unsigned long m_OCCLocationKey;
	};

};
