/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/CombinedEventData.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is an implementation of the ICombinedEvent interface. It provides all
  * the data required by the Event Viewer from both the EVENT and LO_DATAPT_STATE_CHANGE
  * tables.
  *
  * All data is populated when the object is constructed and is read-only.
  */

#if !defined(_COMBINED_EVENT_DATA_H_)
#define _COMBINED_EVENT_DATA_H_

#include "core/data_access_interface/src/ICombinedEventData.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Core
{
    class CombinedEventData : public ICombinedEventData
    {
    public:
        /**
         * Constructor
         *
         * This constructs a CombinedEventData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         * @param key The key that identifies the event.
         * @param logTime The time at which the event was logged.
         * @param alarmSeverityName The name of the severity for the alarm associated
         *                          with the event.
         * @param assetName The asset name for the equipment that generated the event.
         * @param description The description of the event.
         * @param value The value of the event.
         * @param alarmMmsState The MMS state for the alarm associated with the event.
         * @param alarmDssState The DSS state for the alarm associated with the event.
         * @param alarmAvlState The AVL state for the alarm associated with the event.
         * @param operatorName The name of the operator responsible for generating
         *                     the event.
         * @param alarmComment The comment for the alarm associated with the event.
         * @param alarmId The ID for the alarm associated with the event.
         */
        CombinedEventData( const ECombinedEventType type,
                           const unsigned long key,
                           const timeb& createTime,
                           const std::string& alarmSeverityName,
                           const std::string& assetName,
                           const std::string& description,
                           const std::string& value,
                           const std::string& alarmMmsState,
                           const std::string& alarmDssState,
                           const std::string& alarmAvlState,
                           const std::string& operatorName,
                           const std::string& alarmComment,
                           const std::string& alarmId,
						   const std::string& alarmAck,
						   const std::string& alarmStatus, 
						   const std::string& eventTypeName = "",
						   const std::string& operationMode = "",
						   const EventCategory categoryType = ISCSCategory,
						   const std::string& atsEventID = "") :
        m_type(type), m_key(key), m_createTime(createTime), m_alarmSeverityName(alarmSeverityName),
        m_assetName(assetName), m_description(description), m_value(value),
        m_alarmMmsState(alarmMmsState), m_alarmDssState(alarmDssState),
        m_alarmAvlState(alarmAvlState), m_operatorName(operatorName),
        m_alarmComment(alarmComment), m_alarmId(alarmId), m_alarmAck(alarmAck), 
		m_alarmStatus(alarmStatus),m_EventTypeName(eventTypeName),
		m_operationMode(operationMode),m_categoryType(categoryType),m_eventID (atsEventID)
        {
        }

		//++Noel
		CombinedEventData( 
			const std::string& eventID,
			const ECombinedEventType type,
			const unsigned long key,
			const timeb& createTime,
			const std::string& alarmSeverityName,
			const std::string& assetName,
			const std::string& description,
			const std::string& value,
			const std::string& alarmMmsState,
			const std::string& alarmDssState,
			const std::string& alarmAvlState,
			const std::string& operatorName,
			const std::string& alarmComment,
			const std::string& alarmId,
			const std::string& alarmAck,
			const std::string& alarmStatus, 
			//++Noel
			const std::string& sourceTable, 
			const unsigned long	subsystemKey, 
			const long	physicalSubsystemKey, 
			const unsigned long	locationKey, 
			const unsigned long	severityKey, 
			const unsigned long	eventTypeKey, 
			const unsigned long	alarmTypeKey, 
			const unsigned long	operatorKey, 
			const unsigned long	eventLevel, 
			const std::string& 	sessionKey, 
			const unsigned long	sessionLocation, 
			const long	profileId, 
			const unsigned long	actionId, 
			const unsigned long	entityKey, 
			const std::string&  avlalarmheadid, 
			const unsigned long	systemKey, 
			//Noel++
			const std::string& eventTypeName = "",
			const std::string& operationMode = "",
			const EventCategory categoryType = ISCSCategory,
			const std::string& atsEventID = "") :
			m_eventID(eventID), m_type(type), m_key(key), m_createTime(createTime), m_alarmSeverityName(alarmSeverityName),
			m_assetName(assetName), m_description(description), m_value(value),
			m_alarmMmsState(alarmMmsState), m_alarmDssState(alarmDssState),
			m_alarmAvlState(alarmAvlState), m_operatorName(operatorName),
			m_alarmComment(alarmComment), m_alarmId(alarmId), m_alarmAck(alarmAck), 
			m_alarmStatus(alarmStatus),m_EventTypeName(eventTypeName),
			m_operationMode(operationMode),m_categoryType(categoryType),
			m_sourceTable(sourceTable),m_subsystemKey(subsystemKey),m_physicalSubsystemKey(physicalSubsystemKey),
			m_locationKey(locationKey),m_severityKey(severityKey),m_eventTypeKey(eventTypeKey),
			m_alarmTypeKey(alarmTypeKey),m_operatorKey(operatorKey),m_eventLevel(eventLevel),
			m_sessionKey(sessionKey),m_sessionLocation(sessionLocation),m_profileId(profileId),
			m_actionId(actionId),m_entityKey(entityKey),m_avlalarmheadid(avlalarmheadid),m_systemKey(systemKey)
		{
		}

		void setEventID(std::string eventID)
		{
			m_eventID = eventID;
		}

        //------------- Methods inherited from ICombinedEventData -------------//

        /**
         * getEventType
         *
         * Returns the type of the event, that is, whether it is an audit event or a datapoint state change
         * event.
         *
         * @return ECombinedEventType The type of the event.
         */
        virtual const ECombinedEventType getEventType();

		/**
         * getEventCategoryType
         *
         * Returns the Event category type of the event (ATS/ISCS), 
         *
         * @return EventCategoryType The category of the event.
         */
		virtual const EventCategory getEventCategoryType() ;


        
        /**
         * getKey
         *
         * Returns the key for this event.
         *
         * @return unsigned long The key for the event.
         */
        virtual const unsigned long getKey();


        /**
         * getCreateTime
         *
         * Returns the creation time for this event.
         *
         * @return timeb The time at which the event was created.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const timeb getCreateTime();


        /**
         * getAlarmSeverityName
         *
         * Returns the severity name of the alarm associated with this event.
         *
         * @return std::string& The name of the severity of the alarm associated with
         *                      this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmSeverityName();


        /**
         * getAssetName
         *
         * Returns the asset name for this event.
         *
         * @return std::string& The name of the asset that generated this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAssetName();

		/**
         * getAlarmStatusEventType
         *
         * Returns the event type name if it's like 
		 * (AlarmAcknowledged,AlarmClosed ->AlarmNormalized,AlarmSubmitted,AlarmSupressed) for this event.
		 * Otherwise, return ""
         *
         * @return std::string& The name of the asset that generated this event.
         */
		virtual const std::string& getEventTypeName();

        /**
         * getDescription
         *
         * Returns the description for this event.
         *
         * @return std::string& The description for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getDescription();


        /**
         * getValue
         *
         * Returns the value for this event.
         *
         * @return std::string& The value for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getValue();

		/**
         * getOperationMode
         *
         * Returns the Operation_mode(B,M,O,'') for the alarm associated with this event.
         *
         * @return std::string& The Operation mode of the alarm associated with this event.
         */
        virtual const std::string& getOperationMode() ;

	    /**
         * getAlarmMmsState
         *
         * Returns the MMS state for the alarm associated with this event.
         *
         * @return std::string& The MMS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmMmsState() ;


        /**
         * getAlarmDssState
         *
         * Returns the DSS state for the alarm associated with this event.
         *
         * @return std::string& The DSS state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmDssState();


        /**
         * getAlarmAvlState
         *
         * Returns the AVL state for the alarm associated with this event.
         *
         * @return std::string& The AVL state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmAvlState();


        /**
         * getOperatorName
         *
         * Returns the name of the operator directly responsible for generating
         * this event.
         *
         * @return std::string& The name of the operator.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getOperatorName();


        /**
         * getAlarmComment
         *
         * Returns the comment of the alarm associated with this event.
         *
         * @return std::string& The comment.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmComment();

        /**
         * getAlarmId
         *
         * Returns the ID of the alarm associated with this event.
         *
         * @return std::string& The alarm ID.
         */
        virtual const std::string& getAlarmId();

        /**
         * getAlarmAck
         *
         * Returns the Ack status of the alarm associated with this event.
         *
         * @return std::string& The alarm ack status.
         */
        virtual const std::string& getAlarmAck();

        /**
         * getAlarmStatus
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return string& The alarm status.
         */
        virtual const std::string& getAlarmStatus();

		/************************************************************************/
		/*                     Added for new Event Agent                        */
		/************************************************************************/
		/**
         * getSourceTable
         *
         * Returns the Indicator of the source: for EVENT table, anything else for LO_DATAPT_STATE_CHANGE table..
         *
         * @return std::string& The name of the source table.
         */
        virtual const std::string& getSourceTable() ;

		
		/**
         * getSubsystemKey
         *
         * Returns the Subsystem Derived from the source table.
         *
         * @return unsigned long the subsystem key which this event belongs to.
         */
        virtual const unsigned long getSubsystemKey();

		
		/**
         * getPhysicalSubsystemKey
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return unsigned long for physical Sub system key.
         */
        virtual const unsigned long getPhysicalSubsystemKey();

		
		/**
         * getLocationKey
         *
         * Returns the location key for this event.
         *
         * @return unsigned long the location key.
         */
        virtual const unsigned long getLocationKey();

		
		/**
         * getSeverityKey
         *
         * Returns the severity of associated with this event.
         *
         * @return unsigned long the severity key.
         */
        virtual const unsigned long getSeverityKey();

		
		/**
         * getEventTypeKey
         *
         * Returns the event type key of this event.
         *
         * @return unsigned long the event type key.
         */
        virtual const unsigned long getEventTypeKey();

		/**
         * getAlarmTypeKey
         *
         * Returns the alarm type key of the alarm associated with this event.
         *
         * @return unsigned long The alarm type key.
         */
        virtual const unsigned long getAlarmTypeKey() ;

		/**
         * getOperatorKey
         *
         * Returns the operator key associated with this event.
         *
         * @return unsigned long operator key.
         */
        virtual const unsigned long getOperatorKey() ;

		
		/**
         * getEventLevel
         *
         * Returns the event level of this event.
         *
         * @return unsigned long the event level.
         */
        virtual const unsigned long getEventLevel() ;

		
		/**
         * getSessionKey
         *
         * Returns the session key associated with this event.
         *
         * @return std::string& The session key.
         */
        virtual const std::string& getSessionKey() ;

		
		/**
         * getSessionLocation
         *
         * Returns the location key of the source location related to the session ID associated with this event.
         *
         * @return unsigned long The location key.
         */
        virtual const unsigned long getSessionLocation() ;

		/**
         * getProfileId
         *
         * Returns the profile ID related to session associated with this event.
         *
         * @return unsigned long The profile ID.
         */
        virtual const unsigned long getProfileId() ;

		/**
         * getActionId
         *
         * Returns the Action ID associated with this event.
         *
         * @return unsigned long the Action ID.
         */
        virtual const unsigned long getActionId() ;

		/**
         * getEntityKey
         *
         * Returns the entity key associated with this event.
         *
         * @return unsigned long The entity key.
         */
        virtual const unsigned long getEntityKey() ;

		/**
         * getAvlAlarmHeadId
         *
         * Returns the avalanche Head ID of alarm associated with this event.
         *
         * @return std::string& The avalanche head ID.
         */
        virtual const std::string& getAvlAlarmHeadId();

		/**
         * getSystemKey
         *
         * Returns the system key associated with this event.
         *
         * @return unsigned long the system key.
         */
        virtual const unsigned long getSystemKey();

		/**
         * getEventID
         *
         * Returns the event ID associated with this event.
         *
         * @return std::string the event ID.
         */
        virtual const std::string& getEventID();

    private:
        // Disable copy constructor and assignment operator.
        CombinedEventData( const CombinedEventData& );
        CombinedEventData& operator= ( const CombinedEventData& );

        // Variables used to store event data.
        ECombinedEventType m_type;
        unsigned long      m_key;
        timeb              m_createTime;
        std::string        m_alarmSeverityName;
        std::string        m_assetName;
        std::string        m_description;
        std::string        m_value;
        std::string        m_alarmMmsState;
        std::string        m_alarmDssState;
        std::string        m_alarmAvlState;
        std::string        m_operatorName;
        std::string        m_alarmComment;
        std::string        m_alarmId;
		std::string        m_alarmAck;
		std::string        m_alarmStatus;
		std::string        m_operationMode;

		// this is actually Event type name, but only for these ones: 
		// "AlarmAcknowledged, AlarmClosed ->AlarmNormalized, AlarmSubmitted, AlarmSupressed"
		std::string        m_EventTypeName; 
		EventCategory m_categoryType;


		//++Noel =========================
		std::string     m_sourceTable;
		unsigned long	m_subsystemKey;
		long			m_physicalSubsystemKey;
		unsigned long	m_locationKey;
		unsigned long	m_severityKey;
		unsigned long	m_eventTypeKey;
		unsigned long	m_alarmTypeKey;
		unsigned long	m_operatorKey;
		unsigned long	m_eventLevel;
		std::string		m_sessionKey;
		unsigned long	m_sessionLocation;
		long			m_profileId;
		unsigned long	m_actionId;
		unsigned long	m_entityKey;
		std::string     m_avlalarmheadid;
		unsigned long	m_systemKey;
		std::string		m_eventID;
    };
};

#endif // !defined(_COMBINED_EVENT_DATA_H_)