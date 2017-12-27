/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Ross Tucker
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton class is responsible for maintaining a cache of all
  * alarms that are open or unacknowledged.
  */
#if !defined(ALARMCACHE)
#define ALARMCACHE

#include <string>
#include <map>
#include <vector>
#include <deque>
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/alarm/alarm_agent/src/IDataLoader.h"
#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "app/alarm/alarm_agent/src/AlarmMMSSubmitter.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/data_access_interface/src/IOperator.h"
#include "app/alarm/alarm_agent/src/AlarmHistoricalQueue.h"
//Mintao ++
//TD 13650
#include "core/naming/src/NamedObject.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsGlobalInhibitCorbaDef.h"
//Mintao ++
//TD 13650

namespace TA_Base_Core
{
    class StateUpdateMessageSender; // Forward declaration
	// TD 13530 Audit messages to be sent by the alarm queue instead.
	// This is to ensure that the alarm in the alarm table is inserted or updated before event is created
    class AuditMessageSender;       // Forward declaration //20448++
    class AlarmSender; // Forward declaration
    class IAlarmData; // Forward declaraction
    class IAlarmPlanAssocData;
}

namespace TA_Base_App
{
    class QueueItem; // Forward declaration
    class AlarmQueue; // Forward declaration
    class DatabaseLoader; // Forward declaration

    class AlarmCache : public virtual IDataLoader,
                       public virtual TA_Base_Core::IOnlineUpdatable
    {

    public:
    
        /**
          * getInstance
          *
          * @return Returns the single instance of the AlarmCache
          */
        static AlarmCache* getInstance();

		static void releaseInstance();

        /**
          * Destructor
          */
        ~AlarmCache();

        /**
          * terminate
          *
          * This method allows us to explicitly clean up the cache rather than
          * waiting for the program to finish.
          */
        void terminate();

        /**
          * insert
          *
          * This method inserts a new alarm into the cache
          *
          * @param The alarm
          * @param Set to true if more than one alarm with this entity key and
          *        alarm type are allowed to exist
          *
          * @exception Throws an AlarmAgentException if p_multipleOccurancesAllowed
          *            is false and another alarm with the same entity key and alarm
          *            type is found.
          */
        bool insert( TA_Base_Core::AlarmDetailCorbaDef& p_alarm,
                     bool p_multipleOccurancesAllowed );

        /**
          * insertSuppressed
          *
          * This method inserts a new suppressed alarm into the cache
          *
          * @param The alarm
          * @param Set to true if more than one alarm with this entity key and
          *        alarm type are allowed to exist
          *
          * @exception Throws an AlarmAgentException if p_multipleOccurancesAllowed
          *            is false and another alarm with the same entity key and alarm
          *            type is found.
          */
        void AlarmCache::insertSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

        /**
          * getAll
          *
          * This method returns a sequence containing all alarms that are currently
          * stored in the cache
          *
          * @return A sequence containing all alarms stored in the cache.
          */
        TA_Base_Core::AlarmSequence* getAll();

        /**
          * getByEntity
          *
          * This method takes an entity key and returns all alarms that are
          * stored in the cache that are associated with the entity
          *
          * @param The entity key
          * 
          * @return A sequence containing all alarms that are associated with
          *         the entity.
          */
        TA_Base_Core::AlarmSequence* getByEntity( unsigned long p_entity );

        /**
          * getByEntityAndAlarmType
          *
          * This method takes an entity key and alarm type and returns all alarms that are
          * stored in the cache that are associated with the entity and of the given alarm type
          *
          * @param The entity key
          * @param The alarm type
          * 
          * @return A sequence containing all alarms that are associated with
          *         the entity and of the supplied type.
          */
        TA_Base_Core::AlarmSequence* getByEntityAndAlarmType( unsigned long p_entity, unsigned long p_alarmType );

        /**
          * getMostRecentByEntity
          *
          * This method takes an entity key and returns the alarm for that entity
          * with the most recent createTime field. If no alarms exist for the entity,
          * the method throws an AlarmAgentException.
          *
          * @param The entity key
          *
          * @return The most recent alarm for the passed in entity or NULL if no alarms
          *         exist for the entity.
          *
          * @exception AlarmAgentException raised if no alarms exist for the entity.
          */
        TA_Base_Core::AlarmDetailCorbaDef* getMostRecentByEntity( unsigned long p_entity );

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
	    TA_Base_Core::AlarmDetailCorbaDef* getActiveAlarmForEntityOfType( unsigned long p_alarmTypeKey, unsigned long p_entity );

        /**
          * getAlarmById
          *
          * This method takes an alarm ID and returns the appropriate alarm
          *
          * @param The alarm ID
          *
          * @return The appropriate alarm or NULL if the alarm does not exist
          */
        TA_Base_Core::AlarmDetailCorbaDef* getAlarmById( const std::string& alarmId );

        /**
          * openAlarms
          *
          * This method takes a list of alarm ids and searches the cache for an alarm
          * with the same id. If an alarm is found, it is set to open.
          *
          * @param The list of alarm ids
          */
        void openAlarms( const std::vector<std::string>& p_alarmIds );

        /**
          * openAlarm
          *
          * This method takes an alarm id and searches the cache for the alarm
          * with the same id. If the alarm is found, it is set to open.
          *
          * @param The alarm id
          */
        void openAlarm( const std::string& p_alarmId );

        /**
          * closeAlarm
          *
          * This method takes an alarm id and searches the cache for an open alarm
          * with the same id. If an open alarm is found, it is set to closed.
          *
          * This close method should be used to close alarms that were submited
          * with the multiple occurances allowed flag set to TRUE.
          *
          * @param The alarm id
          * @param The session id of the operator that closed the alarm. This is an optional
          *        parameter that defaults to an empty string (since not all alarms are
          *        closed by operators).
          */
        void closeAlarm( const std::string& p_alarmId , bool isOperatorTriggered = false, const std::string& p_sessionId = std::string(""),
			const std::string& p_closeValue = std::string(""), unsigned long p_closeTime = 0 );

        /**
          * closeAlarm
          *
          * This method takes an alarm type and entity key and searches the cache 
          * for an open alarm that has the same type and entity key associated with it. 
          * If an open alarm is found, it is set to closed.
          *
          * This close method should be used to close alarms that were submited
          * with the multiple occurances allowed flag set to FALSE. If more than one
          * alarm exists, they are all closed
          *
          * @param The alarm id
          */
        void closeAlarm( unsigned long p_entityKey, unsigned long p_alarmType, bool isOperatorTriggered = false,
			const std::string& p_closeValue = std::string(""), unsigned long p_closeTime = 0 );

        /**
          * updateAlarmComment
          *
          * This method updates an alarms comment
          *
          * @param The alarm id
          * @param The new comment
          * @param The session id of the operator who updated the comment
          */
        void updateAlarmComment( const std::string& p_alarmId, const std::string& p_comment,
                                 const std::string& p_session );

        /**
          * updateAlarmAcknowledgeStateNonReentrant
          *
          * This method sets an alarm to acknowledged. This is a non reentrant call, it should be used whenever
          * the call results from an external call (ie the only time this should *not* be used is if the alarm is
          * being acknowledged by an autoacknowledge rule).
          *
          * @param The alarm id
          * @param The session id of the operator who acknowledged
          *        the alarm or an empty string if the alarm is being unacknowledged
          */
        void updateAlarmAcknowledgeStateNonReentrant( const std::string& p_alarmId, const std::string& p_acknowledgedBy,
                                                      const std::string& p_session, bool isOperatorTriggered = false  );

        /**
          * updateAlarmAcknowledgeStateReentrant
          *
          * This method sets an alarm to acknowledged. This is a reentrant call, it should only be used when
          * called by an AutomaticAcknowledgement alarm rule.
          *
          * @param The alarm id
          * @param The session id of the operator who acknowledged
          *        the alarm or an empty string if the alarm is being unacknowledged
          */
        void updateAlarmAcknowledgeStateReentrant( const std::string& p_alarmId, const std::string& p_acknowledgedBy,
                                          const std::string& p_session, bool isOperatorTriggered = false  );

        /**
          * updateDecisionSupportState
          *
          * This method updates the decisionSupportState of a given alarm.
          *
          * @param The alarm id
          * @param The new decision support state
          */
        void updateDecisionSupportState( const std::string& p_alarmId, const TA_Base_Core::DecisionSupportStateType& p_state,
                                         const std::string& p_session );

        /**
          * updateMMSState
          *
          * This method updates the MMS State of a given alarm in the cache.
          *
          * @param The alarm id
          * @param The new MMS state
          * @return
          */
        void updateMMSState( const std::string& p_alarmId, const TA_Base_Core::MmsStateType& p_state );

		/**
          * updateMMSStateToSend
          *
          * This method updates the MMS State of a given alarm to MMS_ALM_SEND 
		  * even if the alarm is not present in cache.
		  *
          * @param The alarm
          * @return
          */
		void AlarmCache::updateAlarmMMSStateToSend(TA_Base_Core::AlarmDetailCorbaDef* alarm);

		 /**
          * updateAlarmMMSState
          *
          * This method updates the MMS State of a given alarm.
		  *
          * @param The alarm
          * @param The new MMS state
          * @return
          */
		void updateAlarmMMSState( TA_Base_Core::AlarmDetailCorbaDef* alarm, const TA_Base_Core::MmsStateType& p_state );

        /**
          * loadAlarms
          *
          * This method instructs the cache to load its alarms from the database.
          * or from the controlling agent depending on the current operation mode.
		  *
		  * @param bool bSkipAlarmLoading - exclude alarm loading from database
          */
        void loadAlarms(bool bSkipDataLoading);
		void unloadAlarms();

        /**
          * loadFromDatabase
          *
          * This method overloads the pure virtual loadFromDatabase method in 
          * IDataLoader
          */
        void loadFromDatabase();

        /**
          * hasDataLoaded
          *
          * This method overloads the pure virtual hasDataLoaded method in IDataLoader
          *
          * @return TRUE if data has been successfully loaded, FALSE otherwise
          */
        bool hasDataLoaded();

        /**
          * requestFromControllingAgent
          *
          * This method overloads the pure virtual requestFromControllingAgent method in 
          * IDataLoader
          */
        void requestFromControllingAgent(); 

        /**
          * sendBatchStateUpdates
          *
          * This method sends the current state of the cache and the update queue
          * as AlarmAgentBatchCacheUpdate and AlarmAgentBatchQueueUpdate messages.
          */
        void sendBatchStateUpdates();

        /**
          * receiveAlarmCacheBatchUpdate
          *
          * This method processes a AlarmAgentBatchCacheUpdate message.
          */
        void receiveAlarmCacheBatchUpdate( const TA_Base_Core::AlarmSequence& update);

        /**
          * receiveAlarmQueueBatchUpdate
          *
          * This method processes a AlarmAgentBatchQueueUpdate message.
          */
        void receiveAlarmQueueBatchUpdate( const TA_Base_Core::AlarmAgentQueueItemSequence& update);

        /**
          * receiveQueueItemStateUpdate
          *
          * This method removes a queue item from the update queue
          */
        void removeQueueItem( const TA_Base_Core::AlarmAgentQueueItemCorbaDef& queueItem);

        /**
          * Prints the cache sizes (for testing only)
          */
        void printCacheSize();

        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * processAlarmRuleUpdate
         *
         * When an alarm rule is created, updated or deleted, this checks if it is a plan
         * rule and if it is, update the plan status of the associated alarms.
         */
        void processAlarmRuleUpdate(AbstractRule& alarmRule, TA_Base_App::AbstractRule::ERuleState action);

        /**
          * getOperatorNameFromSession
          *
          * This method takes a session key, retrieves the operator ID from the Authentication Agent
          * and then looks up the operator name in the database. Each time a name is found, it is cached
          * in the m_operatorMap to avoid unecessary look ups.
          *
          * @param Session ID
          * 
          * @return The operator name
          */
        std::string getOperatorNameFromSession(const std::string& p_session);

		/**
		 * getBatchQueueUpdate()
		 *
		 * This is to obtain all current queue updates from the control agent
		 *
		 */
		TA_Base_Core::AlarmAgentQueueItemSequence* getBatchQueueUpdate();

		/**
		 * terminateDataLoading()
		 *
		 * This is to terminate any data loading if it is still processing the loading
		 *
		 */
		void terminateDataLoading();

		/**
		 * loadMMSAlarmFromDatabase()
		 *
		 * This method is used to process MMS Alarms from the database.
		 *
		 */
		void AlarmCache::loadMMSAlarmFromDatabase();

		/**
		 * resetDataLoader()
		 *
		 * Reset the data loader
		 *
		 */
		void resetDataLoader();

		/**
		 * isSystemOperator()
		 *
		 * determines if the session is system operator
		 *
		 * @param Session ID
		 * 
		 * @return bool
		 */
		bool isSystemOperator(const std::string& p_sessionID);


    private:

        typedef std::map<std::string, TA_Base_Core::AlarmDetailCorbaDef*> AlarmIdToAlarmMap;
        typedef std::multimap<unsigned long, TA_Base_Core::AlarmDetailCorbaDef*> EntityToAlarmMap;
        typedef std::vector<TA_Base_Core::AlarmDetailCorbaDef*> AlarmVector;


		/**
          * closeAlarmInternal
          *
          * This method takes an alarm id and searches the cache for an open alarm
          * with the same id. If an open alarm is found, it is set to closed.
          *
          * the implementation of close alarm
          *  
          *
          * @param The alarm id
          * @param The session id of the operator that closed the alarm. This is an optional
          *        parameter that defaults to an empty string (since not all alarms are
          *        closed by operators).
          */
        void closeAlarmInternal(TA_Base_Core::AlarmDetailCorbaDef* alarm , bool isOperatorTriggered = false, const std::string& p_sessionId = std::string(""),
			const std::string& p_closeValue = std::string(""), unsigned long p_closeTime = 0 );


        /**
         * updatePlanStatusOfRelevantAlarms
         *
         * This method updates the plan status of the alarms associated to the given entity type key,
         * entity key and alarm type key.
         */
        void updatePlanStatusOfRelevantAlarms(unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey);

        /**
         * updateCacheForInhibitedAlarms
         *
         * When an alarm rule is created or updated, this checks if it is a suppression by type or by description rule
         * and if it is, update the cache by inhibiting (ie close and ack) current alarms that match the rule.
		 * === Add function ====
		 * @ param		ruleState: if (ruleState == delete) then open all the matched alarms, otherwise ack and close them.
		 * =====================
         */
        void updateCacheForInhibitedAlarms(std::string sessionId, unsigned long entityTypeKey,
			unsigned long entityKey, unsigned long alarmTypeKey,
			AbstractRule::ERuleState ruleState /*TD19819*/,
			const std::string& assocAlarmType  /*20448++*/,
			const std::string& alarmDescription = "");

		// Yanrong++ TD19819
		void inhibitAlarm(TA_Base_Core::AlarmDetailCorbaDef* alarm, const std::string& sessionId);
		void uninhibitAlarm(TA_Base_Core::AlarmDetailCorbaDef* alarm, const std::string& sessionId);
		// ++Yanrong TD19819


        /**
          * find
          *
          * This method takes an alarm id and returns the alarm that is stored
          * in the cache with the same id. If the alarm does not exist in the
          * cache, NULL is returned.
          *
          * This find method should be used to retrieve alarms that were submited
          * with the multiple occurances allowed flag set to TRUE.
          *
          * @param The alarm id
          *
          * @return The alarm
          */
        TA_Base_Core::AlarmDetailCorbaDef* find( const std::string& p_alarmId );

        /**
          * find
          *
          * This method takes an alarm type and entity key and returns returns the 
          * alarm that is stored in the cache with the same id. If the alarm does 
          * not exist in the cache, NULL is returned.
          *
          * This find method should be used to retrieve alarms that were submited
          * with the multiple occurances allowed flag set to FALSE.
          *
          * @param The alarm type
          * @param The entity key
          *
          * @return The alarm
          */
        void find( unsigned long p_alarmType, unsigned long p_entityKey, AlarmVector& vec);

        /**
          * erase
          *
          * This method deletes the pointer to an alarm and removes it from all 
          * the maps it is stored in.
          *
          * @param The alarm
          */
        void erase( TA_Base_Core::AlarmDetailCorbaDef* p_alarm );

        /**
          * doMessageSendingForNewAlarm
          *
          * This method sends a synchronisation message for the monitoring
          * agent to receive as well as a new alarm for the Alarm Viewer
          * to receive.
          *
          * @param The new alarm
          */
        void doMessageSendingForNewAlarm( TA_Base_Core::AlarmDetailCorbaDef* p_alarm );

		/**
		  * doMessageSendingForUnsuppressedAlarm
		  *
		  * This method sends a synchronisation message to the monitoring agent
		  * A message to the database for updating the state
		  * Another to the alarm viewer to display the unsuppressed alarm as new alarm
		  * 
		  * @param The unsuppressed alarm
		  */
		void doMessageSendingForUnsuppressedAlarm ( TA_Base_Core::AlarmDetailCorbaDef* p_alarm );

        /**
          * doMessageSendingForUpdatedAlarm
          *
          * This method sends a synchronisation message for the monitoring
          * agent to receive as well as an alarm update for the Alarm Viewer
          * to receive and an Audit message.
          *
          * @param The updated alarm
          * @param The associated entity key (audit purposes)
          * @param The session ID of the operator who's action initiated the update
          */
        void doMessageSendingForUpdatedAlarm( TA_Base_Core::AlarmUpdateCorbaDef* p_alarm,
                                              const std::string& p_entityName,
                                              const std::string& p_session,
                                              const std::string& p_assetName,
											  bool bSendAuditUpdate = true,
											  const std::string& p_closeValue = "");

        /**
          * getCORBATimeout
          *
          * This method calculates and returns the CORBA timeout
          */
        unsigned long getCORBATimeout();

        /**
          * alarmDataToAlarmMessage
          *
          * Helper method to convert the alarm data that is retrieve from the
          * database, to the type that is stored in our cache
          *
          * @param The alarm data
          *
          * @return A pointer to an alarm message
          */
        TA_Base_Core::AlarmDetailCorbaDef* alarmDataToAlarmMessage( TA_Base_Core::IAlarmData* p_alarmData );

        /**
          * alarmAlreadyRaised
          *
          * This method is used to help the insert method to determine if it should
          * process an alarm based on whether or not a unique alarm with the same type
          * has already been raised for the same entity.
          *
          * @param The alarm we are checking for
          *
          * @return TRUE if a unique alarm of the same type, associated with the
          *         same entity already exists in our cache.
          */
        bool alarmAlreadyRaised( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

        /**
          * getNextMessageNumber
          *
          * This method returns a message sequence number that can be used to detect packet loss
          * by the client.
          */
        unsigned long getNextMessageNumber();


		/**
		* getNextStateMessageNumber
		*
		* This method returns a message sequence number that can be used to detect packet loss
		* by the client.
		*/
		unsigned long getNextStateMessageNumber();

        /** 
          * truncateTo3Digits
          *
          * Truncates the given integer to 3 digits. Used to convert fractions of a second
          * to milliseconds.
          *
          * @param anInteger  The integer to truncate to 3 digits.
          *
          * @return The first 3 digits of the parameter.
          */
        unsigned int truncateTo3Digits(unsigned int anInteger);

        /** 
          * convertToDecisionSupportStateType
          *
          * Convert to decison support state type depending on the given type string.
          *
          * @param stateTypeString  The given decision suppport state type string
          *
          * @return The updated decision support state type
          */
        TA_Base_Core::DecisionSupportStateType convertToDecisionSupportStateType(const std::string& stateTypeString);

        /** 
          * checkDecisionSupportStateType
          *
          * Determines the decison support state type depending on the give current DSS State, entity and alarm type.
		  * If no plan is found in the alarm rule or alarm assoc plan table, NO_ASSOC_PLAN string is returned
          *
		  * @param stateType		Current alarm or update DSS state type
          * @param entityTypeKey    Entity type key associated to related alarm
          * @param entityKey        Entity key associated to related alarm
          * @param alarmTypeKey     Alarm type key of related alarm
          *
          * @return The decision support state type
          */
		TA_Base_Core::DecisionSupportStateType checkDecisionSupportStateType(TA_Base_Core::DecisionSupportStateType stateType, unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey);

        void loadAlarmPlanAssociations();

        bool alarmPlanAssociationExists(unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey);

        /**
          * raiseExceededMaxAlarm
          *
          * This is to raise an alarm for not able to load (from DB) or accept from new submission of alarm into
          * the cache as the max allowable qty has exceeded
		  *
		  * @param string type: Provide description of the problem e.g. unable to load or unable to accept new alarm
          */
		void raiseExceededMaxAlarm(std::string problem);

		/**
		  * updateAlarmValue
		  *
		  * This is to update the alarm value field when an alarm is closed
		  *
		  * @param AlarmDetailCorbaDef*: The alarm detail that value is to be updated
		  */
		void updateAlarmValue(TA_Base_Core::AlarmDetailCorbaDef* alarm);
		std::string obtainValueFromParameter(unsigned long alarmTypeKey,std::string alarmType, TA_Base_Core::AlarmDetailCorbaDef* alarm, bool isSystemAlarm = false, bool isClosed = false);

        // Disable constructors and assignment operator for Singleton
        AlarmCache();
        AlarmCache( const AlarmCache& theAlarmCache);
        AlarmCache& operator=(const AlarmCache&);

        static AlarmCache* m_instance;

        // The maps alarm uuid's to alarms. While it would be faster to store the
        // alarm pkey (as it is a number), this is not possible because we have to
        // be able to operate even if the database goes down.
        AlarmIdToAlarmMap m_cache;

        // This maps entity keys to alarms. It is possible that more than one
        // alarm can be associated with a single entity.
        EntityToAlarmMap m_entityToAlarmMap;

        // Thread locks for the cache
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		//NOTE: The use of readwrite thread guard caused the alarm agent to freeze as there are some functions that call for processing of the alarms
		// via the RuleRepository and within some of the Rules alarm details are read from the Cache, forming read, write, read loop.
		// Thus a ReEntrantThread is used. If a readwrite thread is to be used, the design needs major refactoring.
        TA_Base_Core::ReEntrantThreadLockable m_cacheLock;
        TA_Base_Core::NonReEntrantThreadLockable m_assocLock;
		TA_Base_Core::ReEntrantThreadLockable m_messageNumLock;

        // The queue of updates that need persisting in the database
        AlarmQueue* m_queue;

        // CORBA Timeout
        unsigned long m_corbaTimeout;

        // Maximum number of alarms we can fit in a packet
        unsigned long m_maximumReturnableAlarms;

		// TD 13530 Audit messages to be sent by the alarm queue instead.
		// This is to ensure that the alarm in the alarm table is inserted or updated before event is created
        // Audit message sender
        TA_Base_Core::AuditMessageSender* m_auditSender; //20448++

        // State update message sender
        TA_Base_Core::StateUpdateMessageSender* m_stateSender;

        // Alarm sender
        TA_Base_Core::AlarmSender* m_alarmSender;

        // DatabaseLoader thread (used to periodically attempt to load our alarms
        // if the first attempt fails)
        TA_Base_App::DatabaseLoader* m_dbLoader;

        bool m_dataLoaded;

        unsigned long m_messageSequenceCount;


        // The hard-coded limit for the number of alarms that can be placed into the alarm cache
        static const unsigned int MAX_CACHED_ALARMS;

        std::map<unsigned long, TA_Base_Core::IAlarmPlanAssocData*> m_alarmPlanAssociations;

        // Authenticaiton library
        TA_Base_Bus::AuthenticationLibrary* m_authLib;

        // session id to operator name map
        std::map<std::string, TA_Base_Core::IOperator*> m_operatorMap;

		// TD 14082 this is the Control AlarmAgent's address for callback
		typedef TA_Base_Core::NamedObject< TA_Base_Core::AlarmAgentCorbaDef, 
			TA_Base_Core::AlarmAgentCorbaDef_ptr, 
			TA_Base_Core::AlarmAgentCorbaDef_var > AlarmAgentNamedObject;
		//TD17851 - marc_bugfix Add vectors of "to be closed" alarms 
		//so that alarm would be erased at one location at a time
		std::deque<std::string> m_CurrentAlarmsToBeClosed;

        //MMS_ALM_SENT state change message does not need to send to MFT, but it needs to be send to monitor alarm agent
		//so give state message a different sequence number
		unsigned long m_stateMessageSequenceCount;
		TA_Base_Core::ReEntrantThreadLockable m_stateSeqNumLock;
		//++Noel
		// Keep a record of the alarm that has been submitted.
		AlarmHistoricalQueue m_alarmHistoricalQueue;
    };
}

#endif // !defined(ALARMCACHE)
