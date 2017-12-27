#if !defined(AlarmHelperBase_EE540CEE_FC89_4ecb_95E3_A0177D9F5980__INCLUDED_)
#define AlarmHelperBase_EE540CEE_FC89_4ecb_95E3_A0177D9F5980__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelperBase.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <queue>
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_Base_Core
{
	class MessageType;
	class AuditMessageSender;

	// What type of operation has been queued
	enum AlarmOperation
	{
		SubmitAlarmUnique,
		SubmitAlarmNonUnique,
		CloseAlarmUnique,
		CloseAlarmNonUnique,
		CloseAlarmUniqueAtLocation,
		CloseAlarmNonUniqueAtLocation
	};

    class AlarmMessageCorbaDef; // Forward Declaration

	class AlarmHelperBase : public TA_Base_Core::Thread,
                            public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{
		// Define the helpers as friend classes
		friend class AlarmHelper;
        friend class AlarmModificationHelper;
		friend class NonUniqueAlarmHelper;
	public:
		
		/**
		* typedef looks prettier
		*/
		typedef TA_Base_Core::NamedObject< TA_Base_Core::AlarmAgentCorbaDef, 
			TA_Base_Core::AlarmAgentCorbaDef_ptr, 
			TA_Base_Core::AlarmAgentCorbaDef_var > AlarmAgentNamedObject;

		typedef std::vector< AlarmAgentNamedObject* > AlarmAgentList;

		/**
		 * The Alarm Queue Item keeps track of the submission status of
		 * the item
		 */
		struct AlarmQueueItem
		{
			// Give a default constructor that sets the locationKey & retryTime to 0
			AlarmQueueItem() : locationKey( 0 ), retryTime( 0 ) {}

			// The type of operation
			AlarmOperation operation;

			// The alarm (SubmitAlarm)
			AlarmDetailCorbaDef* alarm;
			
			// The alarm ID (CloseAlarmNonUnique)
			std::string alarmID;

			// The entityKey (CloseAlarmUnique)
			unsigned long entityKey;

			// The alarmTypeKey (CloseAlarmUnique)
			unsigned long alarmTypeKey;

			// The alarm location (for close with location)
			unsigned long locationKey;

			// The alarm new close value
			std::string closeValue;

			// The alarm close time
			TA_Base_Core::DateTime closeTime;

			// The time when this attempt should be retried
			long retryTime;

			// This will contain pointers to the NamedObjects of 
			// Agents that need to be retried
			AlarmAgentList agentsLeftToTry;
		};

		/**
		* constructor
		*/
		AlarmHelperBase();

		/**
		* destructor
		*/
		virtual ~AlarmHelperBase();

		/**
		* run
		* 
		* start the thread running
		*/
		virtual void run();

		/**
		* terminate
		* 
		* stop the thread running
		*/
		virtual void terminate();

	private:

		/**
		* disable the default copy constructor
		*/
		AlarmHelperBase( const AlarmHelperBase& theAlarmHelperBase);
		AlarmHelperBase& operator = ( const AlarmHelperBase& theAlarmHelperBase);
		/**
		* addAlarmQueueItem
		*
		* Add a new item to the queue, this 
		* will get sent by the queue when a connection is available
		*
		* Memory will be cleaned up by AlarmHelperBase
		*
		* @param	alarmQueueItem	the queue item to add
		*/
		void addAlarmQueueItem(AlarmQueueItem* alarmQueueItem);

		/** 
		  * createAlarmWithParams
		  *
		  * This function will populate the default alarm parameters
		  * such as:
		  *		alarmAcknowledgedBy = ""
		  *		alarmOpen = true
		  *		transactiveTime = 0
		  *		alarmID = TA_Base_Core::TA_Uuid::generateUUID()
		  *		ackTime = 0
		  *		sourceTime = time( NULL )
		  *		closeTime = 0
		  *		alarmComments = ""
		  *		parentAlarmID = ""
		  *		isUnique = true
		  *
	   	  * @param  alarmType    The type of alarm as defined in the specific Message Type header file
		  * @param  entityKey    The entity that has raised the alarm
		  * @param  entityTypeKey    The entityType of the entity that has raised the alarm
		  * @param  description    The parameters to be inserted into the description as defined in the AlarmType table
		  * @param  entityName    The name of the entity
		  * @param  locationKey    The location the entity belongs to
		  * @param  subsystemKey    The subsystem the entity belongs to
		  * @param  alarmSeverityKey    The severity to associate with this Alarm
		  * @param	sourceTime	The time the alarm was first detected.
		  * @param	isUnique	A boolean to indicate whether this alarm is unique -> default is unique
		  *
		  * @return AlarmDetailCorbaDef* the populated alarm
		  */
		AlarmDetailCorbaDef* createAlarmWithParams( const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			timeb sourceTime,
			const std::string& assetName,
            bool isUnique = true,
			TA_Base_Core::MmsStateType mmsState = MMS_NONE);

		/** 
		  * getMostRecentActiveAlarmForEntity
		  *
		  * This function will retrieve the most recent alarm for an entity by querying
		  * the AlarmAgent assigned to the entities location
		  *
		  * @return AlarmDetailCorbaDef* the alarm
		  *
		  * @param  unsigned long entityKey - the entitykey of the entity
		  * @param unsigned long locationKey - the locationkey of the entity
          *
          * @exception AlarmAgentException raised if no alarms exist for the entity.
		  * @exception ObjectResolutionException if unable to contact the Agent
		  * @exception NoAlarmAgentExcption if unable to location an agent at the specified location
		  */
		AlarmDetailCorbaDef* getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey );

		/** 
		  * getActiveAlarmForEntityOfType
		  *
		  * This function will retrieve the unack/open alarm for an entity of a defined 
		  * alarm type by querying the AlarmAgent assigned to the entities location
		  *
		  * @return AlarmDetailCorbaDef* the alarm. If no such alarm, NULL will be returned
		  *
		  * @param alarmType - The type key of alarm as defined
		  * @param unsigned long entityKey - the entitykey of the entity
		  * @param unsigned long locationKey - the locationkey of the entity
          *
          * @exception AlarmAgentException raised if encounter problem in locating alarm for the entity.
		  * @exception ObjectResolutionException if unable to contact the Agent
		  * @exception NoAlarmAgentException if unable to location an agent at the specified location
		  */
		AlarmDetailCorbaDef* getActiveAlarmForEntityOfType(unsigned long alarmTypeKey, 
			unsigned long entityKey, unsigned long locationKey );

	  /**
		* submitQueueItem
		*
		* Submit the queue item to the AlarmAgent. Can remove item
		* from queue if successful. Only reason for failure is being
		* unable to contact the AlarmAgent
		* 
		* @param	alarmQueueItem	the alarm to submit
		* @return	bool	true if submitted
		*/
		bool submitQueueItem( AlarmQueueItem* alarmQueueItem );

		
		/**
		* submitQueueItemAtLocation
		*
		* Submit the queue item to the AlarmAgent. Can remove item
		* from queue if successful. Only reason for failure is being
		* unable to contact the AlarmAgent
		* 
		* @param	alarmQueueItem	the alarm to submit
		* @return	bool	true if submitted
		*/
		bool submitQueueItemAtLocation( AlarmQueueItem* alarmQueueItem );

		/**
		* generateAuditMessage
		*
		* This is called to generate the AuditMessage that goes along with every alarm
		*
		* @param	messageType	         The AlarmTypekey to Audit
		* @param	alarmType	         The type of alarm being submitted
		* @param	associatedEntityName The name of the entity associated with the alarm
        * @param    alarmId              The id of the alarm being submitted
		* @param	entityKey			The entityKey the alarm is being raised against
		*/
		void generateAuditMessage( const TA_Base_Core::MessageType& messageType,
			                      const std::string& alarmType,
                                  const std::string& associatedEntityName,
                                  const std::string& alarmId,
								  unsigned long entityKey );

		/** 
		  * getAlarmAgentAtLocation
		  *
		  * This function will return the NamedObject for the Alarm Agent at the 
		  * specified location. If the AlarmAgent has not been loaded for that
		  * location yet then it will be loaded and returned.
		  *
		  * @return AlarmAgentNamedObject* - This is the NamedObject or NULL if unable to load
		  *
		  * @param unsigned long locationKey
		  */
		AlarmAgentNamedObject* getAlarmAgentAtLocation( unsigned long locationKey );

		/** 
		  * getAlarmAgentForEntity
		  *
		  * This function will return the NamedObject for the Alarm Agent for
		  * the specified entity.
		  *
		  * @return AlarmAgentNamedObject* - This is the NamedObject or NULL if unable to load
		  *
		  * @param unsigned long locationKey
		  */
		AlarmAgentNamedObject* getAlarmAgentForEntity( unsigned long entityKey );

		/** 
		  * loadAlarmAgents
		  *
		  * This function will load all the alarm agents from the database and 
		  * store them mapped by location
		  */
		void loadAlarmAgents( );

		/** 
		  * getValueFromParameters
		  *
		  * This function will parse the parameters string to obtain the
		  * alarm value
		  */
		std::string getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description);

        //TD15302 Mintao++
	    /**
	      *   checkSystemAlarmMMSInhibition
	      * 
	      * <description>
	      * 
	      * @return bool  
	      * 
	      * @exception <exceptions> Optional
	      */
        bool    checkSystemAlarmMMSInhibition(unsigned long entityKey);
        /**
          * receiveSpecialisedMessage
          *
          * This method is called whenever the Agent receives a state update or
          * state update request (depending on their mode of operation)
          *
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	    /**
	      * subscribeToEntityMMSInhibitUpdateMsg
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void subscribeToEntityMMSInhibitUpdateMsg();  
         
	    /**
	      * updateInhibitAlarmMMSEntityMap
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void updateInhibitAlarmMMSEntityMap(unsigned long entitykey, bool action);//true means MMS inhibit, false means not MMS inhibit  
         
	    /**
	      * unsubscribeToEntityMMSInhibitUpdateMsg
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void unsubscribeToEntityMMSInhibitUpdateMsg(); 
        //TD15302 Mintao++

		AlarmQueueItem* getAlarmQueueItem();
		unsigned long getAlarmQueueSize();
		void deleteAllAlarmQueueItems();

		/**
		* getActiveAlarms
		* 
		* get all of the active alarms for the alarmagent of this location
		* currently, this interface is only used by SCADA data point library.
		* 
		* @return TA_Base_Core::AlarmSequence* 
		* 
		* @exception <exceptions> Optional
		*/
		TA_Base_Core::AlarmSequence* getActiveAlarms(unsigned long locationKey);

	private:
		/**
		* Map the alarm agents to location
		*/
		typedef std::map< unsigned long, AlarmAgentNamedObject* > AlarmAgents;
		AlarmAgents m_alarmAgents;

		/**
		* Hold a pointer to the Audit Message Sender
		*/
		TA_Base_Core::AuditMessageSender* m_auditMessageSender;

		/**
		* controls the thread
		*/
		bool m_terminated;

		/**
		* store the alarms in a queue
		* with the type of operation
		*/
		typedef std::queue<AlarmQueueItem*> AlarmItemQueue;
		AlarmItemQueue m_alarmItemQueue;

		/**
		* guard the AlarmItemQueue
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_queueLock;

		/**
		* semaphore for waiting on the queue
		*/
		Semaphore m_queueSemaphore;

		/**
		* true if the list of alarm agents has been loaded
		*/
		bool m_initialised;
        //TD15302 Mintao++
        /**
        * This map contains a collection of entity keys and its MMS Inhibit status,
        */
        std::map<unsigned long, bool>    m_inhibitAlarmMMSEntityMap;  
		/**
		* guard the m_inhibitAlarmMMSEntityMap
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_inhibitAlarmMMSEntityMapLock;
	};


}; // namespace TA_Base_Core

#endif // !defined(AlarmHelperBase_EE540CEE_FC89_4ecb_95E3_A0177D9F5980__INCLUDED_)
