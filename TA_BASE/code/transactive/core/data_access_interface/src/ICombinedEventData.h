/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ICombinedEventData.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * ICombinedEventData is an interface to the combined data from the EVENT
  * and LO_DATAPT_STATE_CHANGE tables required by the event viewer. It provides a 
  * read-only view of select information from both of these tables.
  *
  * The data that is accessible from this interface is everything required by the
  * Event Viewer to populate its columns.
  *
  * NOTE: All references to 'event' in the comments below refer to entries from either
  * the event table or the lo_datapt_state_change table.
  *
  */

#if !defined(_I_COMBINED_EVENT_DATA_H_)
#define _I_COMBINED_EVENT_DATA_H_

#include <string>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    enum ECombinedEventType
    {
        E_AUDIT_EVENT,
        E_DPSTATECHANGE_EVENT
    };

    enum EventCategory
	{
		ATSCategory,
		ISCSCategory
	};

    class ICombinedEventData
    {
    public:
        virtual ~ICombinedEventData() {}

        /**
         * getEventType
         *
         * Returns the type of the event, that is, whether it is an audit event or a datapoint state change
         * event.
         *
         * @return ECombinedEventType The type of the event.
         */
        virtual const ECombinedEventType getEventType() =0;

		/**
         * getEventCategoryType
         *
         * Returns the Event category type of the event (ATS/ISCS), 
         *
         * @return EventCategoryType The category of the event.
         */
		virtual const EventCategory getEventCategoryType() = 0;


        /**
         * getKey
         *
         * Returns the key for this event.
         *
         * @return unsigned long The key for the event.
         */
        virtual const unsigned long getKey() =0;


        /**
         * getCreateTime
         *
         * Returns the creation time for this event.
         *
         * @return timeb The time at which the event was created.
         */
        virtual const timeb getCreateTime() =0;


        /**
         * getAlarmSeverityName
         *
         * Returns the severity name of the alarm associated with this event.
         *
         * @return std::string& The name of the severity of the alarm associated with
         *                      this event.
         */
        virtual const std::string& getAlarmSeverityName() =0;


        /**
         * getAssetName
         *
         * Returns the asset name for this event.
         *
         * @return std::string& The name of the asset that generated this event.
         */
        virtual const std::string& getAssetName() =0;

	    /**
         * getAlarmStatusEventType
         *
         * Returns the event type name if it's like 
		 * (AlarmAcknowledged,AlarmClosed ->AlarmNormalized,AlarmSubmitted,AlarmSupressed) for this event.
		 * Otherwise, return ""
         *
         * @return std::string& The name of the asset that generated this event.
         */
		virtual const std::string& getEventTypeName() = 0;

        /**
         * getDescription
         *
         * Returns the description for this event.
         *
         * @return std::string& The description for this event.
         */
        virtual const std::string& getDescription() =0;


        /**
         * getValue
         *
         * Returns the value for this event.
         *
         * @return std::string& The value for this event.
         */
        virtual const std::string& getValue() =0;

		/**
         * getOperationMode
         *
         * Returns the Operation_mode(B,M,O,'') for the alarm associated with this event.
         *
         * @return std::string& The Operation mode of the alarm associated with this event.
         */
        virtual const std::string& getOperationMode() =0;


        /**
         * getAlarmMmsState
         *
         * Returns the MMS state for the alarm associated with this event.
         *
         * @return std::string& The MMS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmMmsState() =0;


        /**
         * getAlarmDssState
         *
         * Returns the DSS state for the alarm associated with this event.
         *
         * @return std::string& The DSS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmDssState() =0;


        /**
         * getAlarmAvlState
         *
         * Returns the AVL state for the alarm associated with this event.
         *
         * @return std::string& The AVL state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmAvlState() =0;


        /**
         * getOperatorName
         *
         * Returns the name of the operator directly responsible for generating
         * this event.
         *
         * @return std::string& The name of the operator.
         */
        virtual const std::string& getOperatorName() =0;


        /**
         * getAlarmComment
         *
         * Returns the comment of the alarm associated with this event.
         *
         * @return std::string& The comment.
         */
        virtual const std::string& getAlarmComment() =0;


        /**
         * getAlarmId
         *
         * Returns the ID of the alarm associated with this event.
         *
         * @return std::string& The alarm ID.
         */
        virtual const std::string& getAlarmId() =0;

        /**
         * getAlarmAck
         *
         * Returns the Ack status of the alarm associated with this event.
         *
         * @return std::string& The alarm ack status.
         */
        virtual const std::string& getAlarmAck() =0;

        /**
         * getAlarmStatus
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return std::string& The alarm status.
         */
        virtual const std::string& getAlarmStatus() =0;

		
		/**
         * getSourceTable
         *
         * Returns the Indicator of the source: for EVENT table, anything else for LO_DATAPT_STATE_CHANGE table..
         *
         * @return std::string& The name of the source table.
         */
        virtual const std::string& getSourceTable() =0;

		
		/**
         * getSubsystemKey
         *
         * Returns the Subsystem Derived from the source table.
         *
         * @return unsigned long the subsystem key which this event belongs to.
         */
        virtual const unsigned long getSubsystemKey() =0;

		
		/**
         * getPhysicalSubsystemKey
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return unsigned long for physical Sub system key.
         */
        virtual const unsigned long getPhysicalSubsystemKey() =0;

		
		/**
         * getLocationKey
         *
         * Returns the location key for this event.
         *
         * @return unsigned long the location key.
         */
        virtual const unsigned long getLocationKey() =0;

		
		/**
         * getSeverityKey
         *
         * Returns the severity of associated with this event.
         *
         * @return unsigned long the severity key.
         */
        virtual const unsigned long getSeverityKey() =0;

		
		/**
         * getEventTypeKey
         *
         * Returns the event type key of this event.
         *
         * @return unsigned long the event type key.
         */
        virtual const unsigned long getEventTypeKey() =0;

		/**
         * getAlarmTypeKey
         *
         * Returns the alarm type key of the alarm associated with this event.
         *
         * @return unsigned long The alarm type key.
         */
        virtual const unsigned long getAlarmTypeKey() =0;

		/**
         * getOperatorKey
         *
         * Returns the operator key associated with this event.
         *
         * @return unsigned long operator key.
         */
        virtual const unsigned long getOperatorKey() =0;

		
		/**
         * getEventLevel
         *
         * Returns the event level of this event.
         *
         * @return unsigned long the event level.
         */
        virtual const unsigned long getEventLevel() =0;

		
		/**
         * getSessionKey
         *
         * Returns the session key associated with this event.
         *
         * @return std::string& The session key.
         */
        virtual const std::string& getSessionKey() =0;

		
		/**
         * getSessionLocation
         *
         * Returns the location key of the source location related to the session ID associated with this event.
         *
         * @return unsigned long The location key.
         */
        virtual const unsigned long getSessionLocation() =0;

		/**
         * getProfileId
         *
         * Returns the profile ID related to session associated with this event.
         *
         * @return unsigned long The profile ID.
         */
        virtual const unsigned long getProfileId() =0;

		/**
         * getActionId
         *
         * Returns the Action ID associated with this event.
         *
         * @return unsigned long the Action ID.
         */
        virtual const unsigned long getActionId() =0;

		/**
         * getEntityKey
         *
         * Returns the entity key associated with this event.
         *
         * @return unsigned long The entity key.
         */
        virtual const unsigned long getEntityKey() =0;

		/**
         * getAvlAlarmHeadId
         *
         * Returns the avalanche Head ID of alarm associated with this event.
         *
         * @return std::string& The avalanche head ID.
         */
        virtual const std::string& getAvlAlarmHeadId() =0;

		/**
         * getSystemKey
         *
         * Returns the system key associated with this event.
         *
         * @return unsigned long the system key.
         */
        virtual const unsigned long getSystemKey() =0;

		/**
         * getEventID
         *
         * Returns the event ID associated with this event.
         *
         * @return std::string the event ID.
         */
        virtual const std::string& getEventID() =0;
	
    };
};

#endif // !defined(_I_COMBINED_EVENT_DATA_H_)