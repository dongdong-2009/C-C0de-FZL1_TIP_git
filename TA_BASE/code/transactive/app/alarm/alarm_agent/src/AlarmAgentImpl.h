/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmAgentImpl.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 14:24:00 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  * This class implements the IAlarmAgentCorbaDef interface
  */
#if !defined(ALARMAGENTIMPL)
#define ALARMAGENTIMPL

#include <map>

#include "core/corba/src/ServantBase.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class AuditMessageSender;   // Forward declaration
}

namespace TA_Base_Bus
{
    class RightsLibrary;        // Forward declaration
}

namespace TA_Base_App
{
    class RuleRepository; // Forward declaration

    class AlarmAgentImpl : public virtual POA_TA_Base_Core::AlarmAgentCorbaDef,
                           public virtual TA_Base_Core::ServantBase
    {
    public:

        /**
          * Constructor
          */
        AlarmAgentImpl();

        /**
          * Destructor
          */
        virtual ~AlarmAgentImpl();

        /**
          * submitAlarm
          *
          * This method allows clients to submit a new alarm
          *
          * @param Set to TRUE if it is acceptable for more than one 
          *        instance of this alarm to be active at once
          * @param The alarm
          *
          * @exception Throws a TA_Base_Core::AlarmAlreadyExists 
          *            exception (as defined in the IDL) if the multiple
          *            occurances allowed exception is set to false and
          *            an alarm of this type already exists.
          */
        void submitAlarm(CORBA::Boolean p_multipleOccurancesAllowed, 
                         const TA_Base_Core::AlarmDetailCorbaDef& p_alarm);

        /**
          * updateComments
          *
          * This method allows an operator to modify the comment associated with
          * a given alarm.
          *
          * @param The new comment
          * @param The alarm id
          * @param The operators session id
          *
          * @exception AccessDeniedException raised if the session check fails
          */
        void updateComments(const char* p_description, const char* p_alarmID, const char* p_sessionID);

        /**
          * acknowledge
          *
          * This method allows an operator to acknowledge an alarm
          *
          * @param The alarm id
          * @param The operator session id
          *
          * @exception AccessDeniedException raised if the session check fails
          */
        void acknowledge(const char* p_alarmID, const char* p_sessionId);

        /**
          * acknowledgeSeq
          *
          * This method allows an operator to acknowledge a group of alarma
          *
          * @param The collection of alarm ids
          * @param The operator session id
          *
          * @exception AccessDeniedException raised if the session check fails
          */
        void acknowledgeSeq(const TA_Base_Core::StringSequence& p_alarms,
                            const char* p_sessionId);

        /**
          * operatorAcknowledgeSeq
          *
          * This method allows an operator via banner/viewer to acknowledge a group of alarms
          * manually
		  *
          * @param The collection of alarm ids
          * @param The operator session id
          */
        void operatorAcknowledgeSeq(const TA_Base_Core::StringSequence& p_alarms,
									const char* p_sessionId);
		/**
		* operatorAcknowledgeSeqEx
		*
		* This method allows an operator via banner/viewer to acknowledge a group of alarms
		* manually, it will return a alarm id list which includes alarms cannot be found on alarm agent.
		*
		* @param The collection of alarm ids
		* @param The operator session id
		* @return Alarms cannot be found on alarm agent
         */
		TA_Base_Core::StringSequence* operatorAcknowledgeSeqEx(const TA_Base_Core::StringSequence& p_alarms, const char* p_sessionId);
        /**
          * closeAlarmById
          *
          * This close should be used if there may be more than one instance
          * of this alarm (ie if it was submitted with p_multipleOccurancesAllowed
          * set to TRUE)
          *
          * @param The alarm id
          */
        void closeAlarmById(const char* p_alarmID, const char* p_closeValue, CORBA::ULong p_closeTime);

        /**
          * getAlarm
          *
          * return The alarm with the specified UID.
          *
          * @param alarmId: the UID of the alarm
          *
          * @return The alarm with the specified UID.
          *
          * @exception AlarmException raised if the specified alarm does not exist.
          */  
		TA_Base_Core::AlarmDetailCorbaDef* getAlarm(const char* alarmId);

        /**
          * closeAlarmByTypeAndKey
          *
          * This close should be used if there is only one active instance
          * of this alarm (ie if it was submitted with p_multipleOccurancesAllowed
          * set to FALSE)
          *
          * @param The alarm type
          * @param The entity key
          */
        void closeAlarmByTypeAndKey(CORBA::ULong p_alarmType, CORBA::ULong p_entityKey, const char* p_closeValue, CORBA::ULong p_closeTime);

        /**
          * operatorCloseAlarm
          *
          * This close should be used by the Alarm Manager when alarms are manually
          * closed by operators. It takes a session key as a parameter.
          *
          * @param The alarm Id
          * @param A session key
          */
        void operatorCloseAlarm( const char* p_alarmID, const char* p_sessionId );

        /**
          * operatorCloseAlarmSequence
          *
          * This close should be used by the Alarm Manager when a sequence of alarms are manually
          * closed by operators. It takes a session key as a parameter.
          *
          * @param The alarm Id
          * @param A session key
          */
        void operatorCloseAlarmSequence( const TA_Base_Core::StringSequence& p_alarms, const char* p_sessionId );

		/**
		* operatorCloseAlarmSequenceEx
		*
		* This close should be used by the Alarm Manager when a sequence of alarms are manually
		* closed by operators. It takes a session key as a parameter.
		* Please note that the close time and value should be updated in the alarm's closeTime and ParamList (with ClosedTime param)
		* This close method is used by an operator via a banner/viewer, it will return a alarm id list which includes alarms cannot be found on alarm agent.
		*
		* @param The alarm Id
		* @param A session key
		* @return Alarms cannot be found on alarm agent
          */
        TA_Base_Core::StringSequence* operatorCloseAlarmSequenceEx( const TA_Base_Core::StringSequence& p_alarms, const char* p_sessionId );
        /**
          * getAlarms
          *
          * This method returns all of the currently open and/or unacknowledged alarms
          *
          * @return All currently open and/or unacknowledged alarms
          */
        TA_Base_Core::AlarmSequence* getAlarms();

        /** TD 14082
          * getQueueUpdates
          *
          * This method returns all of the currently items in the alarm agent's queue for updating
          *
          * @return All currently queue item
          */
		TA_Base_Core::AlarmAgentQueueItemSequence* getQueueUpdates();

        /**
          * getEntityAlarms
          *
          * This method returns all of the currently open and/or unacknowledged alarms
          * for a specific entity.
          *
          * @param The entity key
          *
          * @return All currently open and/or unacknowledged alarms
          */
        TA_Base_Core::AlarmSequence* getEntityAlarms(unsigned long p_entityKey);

        /**
          * getMostRecentActiveAlarmForEntity
          *
          * This method takes an entity key and returns the alarm for that entity
          * with the most recent createTime field. If no alarms exist for the entity,
          * the method throws an AlarmAgentException.
          *
          * @param The entity key
          *
          * @return The most recent alarm for the passed in entity.
          *
          * @exception AlarmAgentException raised if no alarms exist for the entity.
          */
        TA_Base_Core::AlarmDetailCorbaDef* getMostRecentActiveAlarmForEntity( unsigned long p_entityKey );

        /**
		 * getActiveAlarmForEntityOfType
		 *
		 * This function will retrieve the unack/open alarm for an entity of a defined 
		 * alarm type by querying the AlarmAgent assigned to the entities location
		 *
		 * @return AlarmDetailCorbaDef* the alarm. If no such alarm, NULL will be returned
		 *
		 * @param alarmTypeKey - The type of alarm as defined in the specific Message Type header file
		 * @param unsigned long entityKey - the entitykey of the entity
         *
         * @return The most unique open/unack alarm for the passed in entity and alarm type.
         *
         * @exception AlarmAgentException raised if there is problem with getting the alarm.
         */
        TA_Base_Core::AlarmDetailCorbaDef* getActiveAlarmForEntityOfType(CORBA::ULong  p_alarmTypeKey, unsigned long p_entityKey);

        /**
          * setMmsState
          *
          * This method is called when the MMS state of an alarm has changed.
          * It results in the mmsState variable for the alarm being updated appropriately.
          *
          * @param p_alarmId: The alarm ID
	      * @param p_mmsState: The new mmsState of the alarm
          *
          * @exception AlarmException raised if no alarm exists with the specified ID,
	      *            or if the specified mmsState is already set
          */
        void setMmsState( const char* p_alarmId, TA_Base_Core::MmsStateType p_mmsState);

        /**
          * planAssociatedWithAlarmWasRun
          *
          * This method is called when a plan that is associated with a given alarm
          * is run. It results in the decisionSupportState variable for the alarm being
          * updated appropriately.
          *
          * @param The alarm
          * @param Operator session id
          */
        void planAssociatedWithAlarmWasRun(const char* p_alarmId, const char* p_sessionId);

        /**
          * planAssociatedWithAlarmWasPostponed
          *
          * This method is called when a plan that is associated with a given alarm
          * is postponed. It results in the decisionSupportState variable for the alarm being
          * updated appropriately.
          *
          * @param The alarm
          * @param Operator session id
          */
        void planAssociatedWithAlarmWasPostponed(const char* p_alarmId, const char* p_sessionID);

	public:
        /**
          * loadAlarmsAndRules
          *
          * This method instructs the Alarm Cache and Rule Repositories to load their required data
          * from the database or monitoring agent (depending on operating mode). It then blocks until
          * the loading has completed. This ensures that GenericAgent::run is not called until the Agent
          * has finished initialising.
		  *
		  * @param bool bSkipAlarmLoading - exclude alarm loading from database
          */
        void loadAlarmsAndRules(bool bSkipLoadAlarms);
        /**
          * terminateLoading
          *
          * This method is called to break the loading of alarms and rules if it is still loading
          *
          * @param bool terminate To set the loading termination: True = terminate & False = normal ie loading
          * @param Operator session id
          */

		/** TD 14082
		  * isLoading
		  *
		  * This method provides an indication whether the cache is still loading the data
		  *
		  * @return bool Cache is still loading data
		  */
		bool isLoading();


		void unloadAlarmsAndRules();
    private:
        
        // Disable copy constructor and assignment operator
        AlarmAgentImpl( const AlarmAgentImpl& theAlarmAgentImpl);
        AlarmAgentImpl& operator=(const AlarmAgentImpl &);

		void operatorCloseAlarmWithValueAndTime(const std::string alarmID, const std::string sessionId, 
			const std::string closeValue, const TA_Base_Core::DateTime closeTime);

		void operatorAcknowledge(const char* p_alarmID, const char* p_sessionId, bool isOperatorTriggered =true);

		void operatorCloseAlarmInternal( const char* p_alarmID, const char* p_sessionId );
        void acknowledgeAlarmSeq(const TA_Base_Core::StringSequence& p_alarms,
									const char* p_sessionId, bool isOperatorTriggered );

		//chenlei++
		void EnsureInControlState();
		 
		//++chenlei

        // Pre Cache lock
        TA_Base_Core::ReEntrantThreadLockable m_preCacheLock;

        // Audit message sender
        TA_Base_Core::AuditMessageSender* m_auditSender;

        // Set to true when terminate gets called
        //volatile bool m_terminated;

        // This is only used in monitor mode
        bool m_haveReceivedAlarmBatch;

        // Rights library object
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;


	   };
}

#endif // !defined(ALARMAGENTIMPL)
