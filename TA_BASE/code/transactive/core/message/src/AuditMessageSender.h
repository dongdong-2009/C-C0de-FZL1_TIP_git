#if !defined(AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_)
#define AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/AuditMessageSender.h $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Used to send Audit messages
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include <map>
#include "core/message/src/NameValuePair.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/event_processing/src/IEvent.h"

#include "core/event/src/EventItem.h"


using namespace TA_Base_Core::EventConstants;

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // AuditMessage header:

    // domain_name = message context name
    // type_name = message type
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - client supplied (name, value) pairs

    class AuditMessageSender
    {
    public:

        /**
        * constructor
        *
        * @param    eventSupplier    StructuredEventSupplier used to send events
        */
        AuditMessageSender(StructuredEventSupplier* eventSupplier);

        /**
        * destructor
        */
        virtual ~AuditMessageSender();

		/** 
		 * sendAuditMessage
		 *
		 * To send a IEvent
		 *
		 * @return std::string  The message's unique identifier 
		 * @param  IEvent       The event need to be submitted.  
		 */
		std::string sendAuditMessage (IEvent * event);


		/** 
		 * sendAuditMessage
		 *
		 * for the client to submit non-alarm event
		 * 
		 * @return   std::string     The uuid of the new event 
		 * @param    messageType        The specific type of message
		 * @param    entityKey        The key of an entity that is associated with this Message
		 * @param    subsystemKey     The subsystemKey of the entity
		 * @param    physicalsubystemKey   The physicalsubystemKey of the entity
		 * @param    entityLocationKey   The location key of the entity
		 * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
		 * @param    details            Any additional information associated with the Message.
		 * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
		 * @param    operatorKey       The operator key who initiate this Audit Message
		 * @param    operatorName      The operator name 
		 * @param    profileID         The profile Id of current operator
		 * @param    sessionLocationKey the location key from session info
		 * @param    value            The value of the event
		 * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
		 * @param    incidentID        The UUID of an incident that this event should be associated with
		 * @param    parentEventID   If this event needs to be associated with another audit event (i.e. plan steps), to display a related event tree in the viewer, put the key here
		 * @param    assetName       The name of the asset associated with the audit event.
		 * @param    filterableData  Any additional filterable information that you wish to include in the event.
		 * @param    timeStamp       The time at which the event took place (current time if unspecified)
		 */
		std::string sendAuditMessage(const MessageType& messageType,
			unsigned long entityKey,
			unsigned long subsystemKey,						 //++
			long physicalsubystemKey,				 //++ when agents call this function, will provide a valid value
			unsigned long entityLocationKey,					 //++
			const DescriptionParameters& description,
			const std::string& details,
			const std::string& sessionID,
			unsigned long operatorKey,                         // ++
			std::string& operatorName,               // ++
			long profileID,                           // no need for realtime event
			unsigned long sessionLocationKey,                  // no need for realtime event
			std::string& value ,			         // ++, 
			const std::string& alarmID,
			const std::string& incidentID,
			const std::string& parentEventID,
			const std::string& assetName,
			const FilterData* filterableData /* = 0 */,
			const timeb& timeStamp /* = 0 */,
			bool etyInfoValid = true,
			bool sessionInfoValid=true) ;

		

        /**
        * sendAuditMessageWithAsset,This interface will be deleted in the future. As it can be implemented by sendAuditMessage(...)
        *
        * Used to send an AuditMessage.  (for non-alarm event)
        *
        * @return   std::string     The message's unique identifier
        * @param    messageType        The specific type of message
        * @param    entityKey        The key of an entity that is associated with this Message
        * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
        * @param    details            Any additional information associated with the Message.
        * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
        * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
        * @param    incidentID        The UUID of an incident that this event should be associated with
        * @param    parentEventID   If this event needs to be associated with another audit event (i.e. plan steps), to display a related event tree in the viewer, put the key here
        * @param    assetName       The name of the asset associated with the audit event.
        * @param    filterableData  Any additional filterable information that you wish to include in the event.
        * @param    timeStamp       The time at which the event took place (current time if unspecified)
        */
        std::string sendAuditMessageWithAsset( const MessageType& messageType, 
                                               unsigned long entityKey,
                                               const DescriptionParameters& description,
                                               const std::string& details,
                                               const std::string& sessionID,
                                               const std::string& alarmID,
                                               const std::string& incidentID,
                                               const std::string& parentEventID,
                                               const std::string& assetName,
                                               const FilterData* filterableData = NULL,
                                               const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );

        /**
        * sendAuditMessage, This interface will be deleted in the future. As it can be implemented by sendAuditMessage( with all paramters )
        *
        * Used to send an AuditMessage.  The asset name will be unspecified.  
        *
        * @return   std::string     The message's unique identifier
        * @param    messageType        The specific type of message
        * @param    entityKey        The key of an entity that is associated with this Message
        * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
        * @param    details            Any additional information associated with the Message.
        * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
        * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
        * @param    incidentID        The UUID of an incident that this event should be associated with
        * @param    parentEventID   If this event needs to be associated with another audit event (ie plan steps), to display a related event tree in the viewer, put the key here
        * @param    assetName       The name of the asset associated with the audit event.
        * @param    filterableData  Any additional filterable information that you wish to include in the event.
        * @param    timeStamp       The time at which the event took place (current time if unspecified)
        */
        std::string sendAuditMessage( const MessageType& messageType,
                                      unsigned long entityKey,
                                      const DescriptionParameters& description,
                                      const std::string& details,
                                      const std::string& sessionID,
                                      const std::string& alarmID,
                                      const std::string& incidentID,
                                      const std::string& parentEventID,
                                      const FilterData* filterableData = NULL,
                                      const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );

        // CL-21508 begin - for alarm event
		//This interface will be deleted in the future. As it can be implemented by sendAuditMessageEx( with all paramters )
        std::string sendAuditMessageEx( const MessageType& messageType,             
                               const unsigned long entityKey, 
                               const DescriptionParameters&description, 
                               const std::string& details,
                               const std::string& sessionID, 
                               const std::string& alarmID, 
                               const std::string& incidentID, 
                               const std::string& parentEventID, 
                               const std::string& assetName,
                               const std::string& alarmDescription,
                               const unsigned long eventLelOrSeverityKey,
                               const std::string& alarmvalue,
                               const unsigned long alarmTypeKey,
                               const std::string& alarmComments, 
                               const std::string& alarmMMSState,
                               const std::string& alarmDSSState,
                               const std::string& alarmAVLstate,
                               const std::string& alarmACKedBy,
                               const std::string& alarmStatus, 
                               const std::string& OMFlag,
                               const FilterData* filterableData = NULL,
                               const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );

		
		/** 
		 * sendAuditMessageEx  
		 * 
		 * for client to send an alarm event 
		 *
		 * @return std::string  the uuid of the new event.
		 *
		 *
		 */
		std::string AuditMessageSender::sendAuditMessageEx( const MessageType& messageType, 
			const unsigned long entityKey,
			unsigned long subsystemKey,                 //++
			long physiscalSubsystemKey,        //++
			unsigned long entityLocationKey,            //++
			const DescriptionParameters&description, 
			const std::string& details, 
			const std::string& sessionID, 
			unsigned long operatorKey,                    //++
			std::string operatorName,           //++
			std::string severityName,           //++
            //unsigned long actionId, // client will not provide action id
			unsigned long sessionLocationKey,				// no need for realtime event
			long profileID,                      // no need for realtime event 
			const std::string& alarmID, 
			const std::string& incidentID, 
			const std::string& parentEventID, 
			const std::string& assetName, 
			const std::string& alarmDescription, 
			const unsigned long eventLelOrSeverityKey, 
			const std::string& alarmvalue, 
			const unsigned long alarmTypeKey, 
			const std::string& alarmComments, 
			const std::string& alarmMMSState, 
			const std::string& alarmDSSState,
			const std::string& alarmAVLstate,
			const std::string& alarmACKedBy,
			const std::string& alarmStatus, 
			const std::string& OMFlag,
			const FilterData* filterableData/* = NULL*/,
			const timeb& timeStamp/* = TA_Base_Core::AlarmConstants::defaultTime*/ ,
			bool etyInfoValid = true,
			bool sessionInfoValid=true) ;

        // CL-21508 end
    private:
        // NOT CLONEABLE
        AuditMessageSender( const AuditMessageSender& theAuditMessageSender);

	
	//For Realtime-Event-Init begin

		/** 
		 * getValueForEvent
		 * 
		 * get value for non-alarm event
		 *
		 * @param eventTypeKey  the message type key of the event
		 * @param description   the description of the event
		 * @param event_type_name  the message type name of the event
		 * @param paramList     the parameter list string of the event
		 * @param value         the value of event to be set
		 */
		void getValueForEvent(unsigned long eventTypeKey, const DescriptionParameters& description, 
			std::string event_type_name, std::string paramList, std::string & value)  ;

		/** 
		 * getValueFromDescPrameters  
		 *
		 * get "ClosedValue" from DescriptionParameters, if not exist, get "DataPointValue" from DescriptionParameters,
		 * if still not exist, get the value from first item of DescriptionParameters
		 * this function is only used by non-alarm event now
		 * 
		 * @return std::string the value got from DescriptionParameters
		 * @param parameters   the DescriptionParameters of the event
		 *
		 */
		std::string getValueFromDescPrameters(const DescriptionParameters& parameters);

		/** 
		 * sendAuditMessageBeforeToAgent
		 * 
		 * process a non-alarm event before sending to real-time event agent
		 * 
		 * @return std::string the uuid of the new event
		 * @return   std::string     The message's unique identifier
		 * @param    messageType        The specific type of message
		 * @param    entityKey        The key of an entity that is associated with this Message
		 * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
		 * @param    details            Any additional information associated with the Message.
		 * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
		 * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
		 * @param    incidentID        The UUID of an incident that this event should be associated with
		 * @param    parentEventID   If this event needs to be associated with another audit event (i.e. plan steps), to display a related event tree in the viewer, put the key here
		 * @param    assetName       The name of the asset associated with the audit event.
		 * @param    filterableData  Any additional filterable information that you wish to include in the event.
		 * @param    timeStamp       The time at which the event took place (current time if unspecified)
		 * @param    descriptionStr  the description string to be set in the function
		 * @param    parameterListStr the parameterlist string to be set in the function
		 */
		std::string sendAuditMessageBeforeToAgent( const MessageType& messageType, 
			unsigned long entityKey,
			const DescriptionParameters& description,
			const std::string& details,
			const std::string& sessionID,
			const std::string& alarmID,
			const std::string& incidentID,
			const std::string& parentEventID,
			const std::string& assetName,
			const FilterData* filterableData ,
			 timeb& timeStamp ,
			 std::string& descriptionStr,
			 std::string& parameterListStr);

		/** 
		 * sendAuditMessageExBeforeToAgent
		 * 
		 * process an alarm event before sending to real-time event agent
		 *
		 * @return std::string the uuid of the new event
		 */
		std::string AuditMessageSender::sendAuditMessageExBeforeToAgent( const MessageType& messageType, 
			const unsigned long entityKey, 
			const DescriptionParameters&description, 
			const std::string& details, 
			const std::string& sessionID, 
			const std::string& alarmID, 
			const std::string& incidentID, 
			const std::string& parentEventID, 
			const std::string& assetName, 
			const std::string& alarmDescription, 
			const unsigned long eventLelOrSeverityKey, 
			const std::string& alarmvalue, 
			const unsigned long alarmTypeKey, 
			const std::string& alarmComments, 
			const std::string& alarmMMSState, 
			const std::string& alarmDSSState,
			const std::string& alarmAVLstate,
			const std::string& alarmACKedBy,
			const std::string& alarmStatus, 
			const std::string& OMFlag,
			const FilterData* filterableData,
			timeb& timeStamp ,
			std::string & descriptionStr);
		
		/** 
		 * processDescByParametes
		 *
		 * Currently only used by DataPointStateChange Event to get description from DescriptionParameters(from the first item)
		 *
		 * @return std::string the description of the event
		 * @param  parameters  the DescriptionParameters passed from DataPointStateChange Event
		 * @param  eventTypeKey the messsage type key of the DataPointStateChange Event
		 */
		std::string processDescByParametes(const DescriptionParameters& parameters,  unsigned long eventTypeKey);


		/** 
		 * createEventData
		 *
		 * generate a EventItemInfo data and initialized by all the parameters.
		 *
		 * @return EventItemInfo* the point of the EventItemInfo data generated.
		 *
		 *
		 */
		EventItemInfo*  AuditMessageSender::createEventData(bool etyInfoValid,bool sessionInfoValid,
			std::string eventID, std::string sourceTable, timeb createTime, std::string assetName, std::string desc, std::string value , 
			unsigned long entitykey,unsigned long subsystemKey, long physicalSubsystemKey, unsigned long locationKey,
			unsigned long eventTypeKey, std::string eventTypeName, unsigned long actionID, unsigned long eventLevel, long profileID,
			std::string operationMode="", unsigned long systemKey=0,
			std::string sessionKey = "", unsigned long sessionLocation=0, 
			unsigned long operatorKey =0, std::string operatorName ="", 
			std::string alarmID="", unsigned long severityKey = 0, std::string severityName="", 
			unsigned long alarmTypKey =0 , std::string mmsState = "", std::string dssState ="", 
			std::string avlState="", std::string alarmComment="",
			std::string alarmACK="", std::string alarmStatus="", std::string alarmHeadID="" 
			);

	//For Realtime-Event-Init end

        StructuredEventSupplier* m_supplier;

    }; // AuditMessageSender


}; // TA_Base_Core

#endif // !defined(AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_)
