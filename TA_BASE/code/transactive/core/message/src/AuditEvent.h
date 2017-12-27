/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/AuditEvent.h $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the IEvent interface. It is responsible for writing Audit Messages to the database and providing
 * summary information that can be used for event processing.
 */

#if !defined(_AUDIT_EVENT_H_)
#define _AUDIT_EVENT_H_

#include <memory>
#include <string>

#include "core/event_processing/src/IEvent.h"
#include "core/message/IDL/src/AuditMessageCorbaDef.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class IMessageData;

    class AuditEvent : public IEvent
    {
    public:
        /**
         * Constructor
         *
         * Constructs an event based around the audit message corba def
         */
        AuditEvent( const AuditMessageCorbaDef& message );

 		// CL-21508 begin
        AuditEvent( const AuditMessageCorbaDef& message, 
            const std::string& alarmDescription, 
            const unsigned long eventLelOrSeverityKey, 
            const std::string& alarmValue, 
            const unsigned long alarmTypeKey, 
            const std::string& alarmComments, 
            const std::string& alarmMMSState, 
            const std::string& alarmDSSState,
            const std::string& alarmAVLstate,
            const std::string& alarmACKedBy,
            const std::string& alarmStatus,
            const std::string& OMFlag);
        // CL-21508 end

        /**
         * Destructor
         */
        virtual ~AuditEvent();


        /** 
          * getEntityKey
          *
          * Returns the key of the entity associated with the event.
          *
          * @return unsigned long The entity key.
          */
        virtual unsigned long getEntityKey();


        /** 
          * getEntityTypeKey
          *
          * Returns the key of the entity type associated with the event.
          *
          * @return unsigned long The entity type key.
          */
        virtual unsigned long getEntityTypeKey();


        /** 
          * getEventTypeKey
          *
          * Returns the event type key.
          *
          * @return unsigned long The event type key.
          */
        virtual unsigned long getEventTypeKey();


        /**
         * writeToDatabase
         *
         * Writes the data contained in this object to a new entry in the event table in the database.
         */
        void writeToDatabase();        

    private:
        // Hidden Methods.
        AuditEvent();
        AuditEvent( const AuditEvent& );
        const AuditEvent& operator=( const AuditEvent& );

        // The key to use when storing the audit event.
        std::string m_eventId;

        // The keys required to summarise the audit event.
        unsigned long m_entityKey;
        unsigned long m_entityTypeKey;
        unsigned long m_eventTypeKey;

        // The database object representing this message.
        std::auto_ptr<IMessageData> m_message;
    };
}

#endif // !defined(_AUDIT_EVENT_H_)
