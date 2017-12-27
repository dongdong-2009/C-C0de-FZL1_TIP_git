/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/AuditEvent.cpp $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the IEvent interface. It is responsible for writing Audit Messages to the database and providing
 * summary information that can be used for event processing.
 */

#if defined(__WIN32__)
#pragma warning(disable:4786)
#endif // defined(__WIN32__)

#include <ctime>

#include "core/message/src/AuditEvent.h"

#include "core/data_access_interface/src/MessageAccessFactory.h"
#include "core/data_access_interface/src/IMessageData.h"
#include "core/data_access_interface/src/MessageData.h"

#include "core/utilities/src/TAAssert.h"

namespace
{
    const std::string NO_VALUE_STR("");
}

namespace TA_Base_Core
{
    AuditEvent::AuditEvent(const AuditMessageCorbaDef& message)
        : m_eventId(NO_VALUE_STR),
          m_entityKey(EventConstants::INVALID_KEY),
          m_entityTypeKey(EventConstants::INVALID_KEY),
          m_eventTypeKey(EventConstants::INVALID_KEY)
    {
        // The event ID must be stored separately as it is required
        // to initiate the database write.
        m_eventId = std::string( message.eventID );

        // Convert the CORBA strings to std strings.
        std::string description( message.messageDescription );
        std::string paramList( message.messageParameterList );
        std::string details( message.messageDetails );
        std::string type( message.messageTypeKey );
        std::string alarmKey( message.assocAlarmID );
        std::string incidentId( message.assocIncidentID );
        std::string sessionId( message.sessionID );
        std::string assetName( message.assetName );
        // TD13673
        unsigned long locationKey = message.locationKey;

        // Error check any important values.
        TA_ASSERT( !m_eventId.empty(),"Event ID is empty");
        TA_ASSERT( !type.empty(),"TypeKey is empty");

        // Store the entity key.
        m_entityKey = message.assocEntityKey;

        // Store the event type key.
        std::stringstream strToUl;
        strToUl.str( type );
        strToUl >> m_eventTypeKey;

        // Create a new message.
        m_message = std::auto_ptr<IMessageData>( TA_Base_Core::MessageAccessFactory::getInstance().createMessage() );

        // And now set all the fields
        m_message->setAlarmKey( alarmKey );
        m_message->setDescription( description );
        m_message->setDetails( details );
        m_message->setEntityKey( message.assocEntityKey );
        m_message->setIncidentID( incidentId );
        m_message->setParamList( paramList );
        m_message->setSessionID( sessionId );
        m_message->setType( type );
        m_message->setAssetName( assetName );
        // TD13673
        m_message->setLocationKey( locationKey );

        // Set the log time.
        timeb logtime;
        ftime( &logtime );
        m_message->setLogtime( logtime );

        // Set the create time.
        timeb createtime;
        createtime.time = message.createTime.time;
        createtime.millitm = message.createTime.millitm;
        m_message->setCreatetime( createtime );
    }
  // CL-21508 begin
    AuditEvent::AuditEvent( const AuditMessageCorbaDef& message, 
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
        const std::string& OMFlag ) : m_eventId(NO_VALUE_STR),
        m_entityKey(EventConstants::INVALID_KEY), m_entityTypeKey(EventConstants::INVALID_KEY),
        m_eventTypeKey(EventConstants::INVALID_KEY)
    {
        // The event ID must be stored separately as it is required
        // to initiate the database write.
        m_eventId = std::string( message.eventID );
        
        // Convert the CORBA strings to std strings.
        std::string description( message.messageDescription );
        std::string paramList( message.messageParameterList );
        std::string details( message.messageDetails );
        std::string type( message.messageTypeKey );
        std::string alarmKey( message.assocAlarmID );
        std::string incidentId( message.assocIncidentID );
        std::string sessionId( message.sessionID );
        std::string assetName( message.assetName );
        unsigned long locationKey = message.locationKey;
        
        // Error check any important values.
        TA_ASSERT( !m_eventId.empty(),"Event ID is empty");
        TA_ASSERT( !type.empty(),"TypeKey is empty");
        
        // Store the entity key.
        m_entityKey = message.assocEntityKey;
        
        // Store the event type key.
        std::stringstream strToUl;
        strToUl.str( type );
        strToUl >> m_eventTypeKey;
        
        // Create a new message.
        m_message = std::auto_ptr<IMessageData>( TA_Base_Core::MessageAccessFactory::getInstance().createMessage() );
        
        // And now set all the fields
        m_message->setAlarmKey( alarmKey );
        m_message->setDescription( description );
        m_message->setDetails( details );
        m_message->setEntityKey( message.assocEntityKey );
        m_message->setIncidentID( incidentId );
        m_message->setParamList( paramList );
        m_message->setSessionID( sessionId );
        m_message->setType( type );
        m_message->setAssetName( assetName );
        m_message->setLocationKey( locationKey );
        m_message->setAlarmDescription( alarmDescription.c_str() );
        m_message->setAlarmValue( alarmValue.c_str() );
        m_message->setAlarmComments( alarmComments.c_str() );
        m_message->setAlarmMMSState( alarmMMSState.c_str() );
        m_message->setAlarmDSSState( alarmDSSState.c_str() );
        m_message->setAlarmAVLstate( alarmAVLstate.c_str() );
        m_message->setAlarmACKedBy( alarmACKedBy.c_str() );
        m_message->setAlarmStatus( alarmStatus.c_str() ); 
        m_message->setOMFlag( OMFlag );
        m_message->setEventLelOrSeverityKey( eventLelOrSeverityKey );
        m_message->setAlarmTypeKey( alarmTypeKey );
        
        // Set the log time.
        timeb logtime;
        ftime( &logtime );
        m_message->setLogtime( logtime );
        
        // Set the create time.
        timeb createtime;
        createtime.time = message.createTime.time;
        createtime.millitm = message.createTime.millitm;
        m_message->setCreatetime( createtime );
    }
    // CL-21058 end

    AuditEvent::~AuditEvent()
    {
    }


    unsigned long AuditEvent::getEntityKey()
    {
        return m_entityKey;
    }


    unsigned long AuditEvent::getEntityTypeKey()
    {
        // BACTODO: Audit messages are not aware of the type of an entity, so this information
        //          will need to come from another source.
        return m_entityTypeKey;
    }


    unsigned long AuditEvent::getEventTypeKey()
    {
        return m_eventTypeKey;
    }


    void AuditEvent::writeToDatabase()
    {
        m_message->applyChanges( m_eventId );
    }
}
