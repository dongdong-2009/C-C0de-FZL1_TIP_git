/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/AuditMessageSender.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/13 14:40:41 $
  * Last modified by:  $Author: Noel $
  * 
  * Used to send Audit messages
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/AuditMessageSender.h"
#include "core/message/IDL/src/AuditMessageCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/types/MessageTypes.h"
#include "core/uuid/src/TAuuid.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/message/src/AuditEvent.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/event_processing/src/RealTimeEventSender.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    AuditMessageSender::AuditMessageSender( StructuredEventSupplier* eventSupplier )
        : m_supplier(eventSupplier)
    {
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
    }


    AuditMessageSender::~AuditMessageSender()
    {
    }



	//this func will only called by datapointStateChange event now. we assume description is saved in DescriptionParameters as required format. [EventMessage]
	std::string AuditMessageSender::processDescByParametes(const DescriptionParameters& parameters,  unsigned long eventTypeKey)
	{
		unsigned long dpStateChangeTypeKey = strtoul( TA_Base_Core::DataPointAudit::DataPointStateChange.getTypeKey().c_str(), NULL, 10); 
		if ( eventTypeKey == dpStateChangeTypeKey)   // DataPointStateChange Event
		{ 
			// directly get EventMessge value from dp (which is the only paramter);
			for (unsigned int i = 0; i < parameters.size(); i++)
			{
				return parameters[i]->value ;
			}
		}
		return "";
	}

	void AuditMessageSender::getValueForEvent(unsigned long eventTypeKey, const DescriptionParameters& description,
		std::string event_type_name, std::string paramList, std::string & value)  
	{ 
		//string valueFromParam = getValueFromEventParamList(event_param_list);
		std::string valueFromParam = getValueFromDescPrameters(description);

		if(eventTypeKey ==30306 ||eventTypeKey ==30291 ||eventTypeKey ==30304 ||eventTypeKey ==30310  )  //RadioSelectCall, RadioDeselectCall, RadioReleaseCall,RadioVoiceCall
		{  value = "Successfull";}
		else if( 0!=paramList.compare(valueFromParam ) )
		{  value = valueFromParam ;}
		else 
		{  value = event_type_name + " "; }   //todo: to be confirmed: p_alarm_value:= p_event_type_name||' '||p_alarm_type_name;  

		if(eventTypeKey ==30253 || eventTypeKey ==30232)  //PlanManualGRCBadLaunch , PlanAutoGRCBadLaunch
		{ value = "Bad Launch";}
		else if(eventTypeKey ==30254 || eventTypeKey ==30233)  //PlanManualGRCBadReturn , PlanAutoGRCBadReturn
		{ value ="Bad Return"; }

	}

	std::string AuditMessageSender::getValueFromDescPrameters(const DescriptionParameters& parameters)   //from procedure dp_param_list
	{       
		std::string paramName;
		std::string paramValue;  
		for (unsigned int i = 0; i < parameters.size(); i++)
		{
			paramName = parameters[i]->name;
			if(paramName.compare("ClosedValue")==0)
			{ return parameters[i]->value; }
		}       
		for (unsigned int i = 0; i < parameters.size(); i++)
		{
			paramName = parameters[i]->name;
			if(paramName.compare("DataPointValue") == 0)
			{ return parameters[i]->value; }
		}
		if (parameters.size()>0)
		{ return parameters[0]->value;}
		return "";
	}

	EventItemInfo* AuditMessageSender::createEventData(
		bool etyInfoValid ,bool sessionInfoValid,
		std::string eventID, std::string sourceTable, timeb createtime, std::string assetName, std::string desc, std::string value , 
		unsigned long entitykey,unsigned long subsystemKey, long physicalSubsystemKey, unsigned long locationKey,
		unsigned long eventTypeKey, std::string eventTypeName, unsigned long actionID, unsigned long eventLevel, long profileID,
		std::string operationMode, unsigned long systemKey,
		std::string sessionKey , unsigned long sessionLocation, 
		unsigned long operatorKey , std::string operatorName , 
		std::string alarmID, unsigned long severityKey , std::string severityName, 
		unsigned long alarmTypKey  , std::string mmsState , std::string dssState , 
		std::string avlState, std::string alarmComment,
		std::string alarmACK, std::string alarmStatus, std::string alarmHeadID
	)
	{
		EventItemInfo* eventDetail = new EventItemInfo();
		eventDetail->isEntityInfoValid = etyInfoValid;
		eventDetail->isSessionInfoValid = sessionInfoValid;

		MillisecondTimeEx time(createtime.time, createtime.millitm);
		EvMgsSequenceNumber seqNum;
		//subsystem int --> unsinged long
		eventDetail->eventItemDetail = new EventItem(
			eventID.c_str(), sourceTable.c_str(), time,subsystemKey, physicalSubsystemKey,
			locationKey, severityKey, severityName.c_str(), assetName.c_str(), desc.c_str(),
			eventTypeKey, eventTypeName.c_str(),
			alarmID.c_str(), alarmTypKey, value.c_str(),
			mmsState.c_str(), dssState.c_str(), avlState.c_str(),
			operatorKey, operatorName.c_str(), alarmComment.c_str(),
			eventLevel, alarmACK.c_str(),alarmStatus.c_str(),
			sessionKey.c_str(), sessionLocation, profileID,
			actionID, operationMode.c_str(), entitykey,
			alarmHeadID.c_str(), systemKey,seqNum);

		return eventDetail;

	}

	//this will be used by DataPointStateChange event
	std::string AuditMessageSender::sendAuditMessage (IEvent * event)   
	{
		//entity related data, assetName , value can be get from IEvent
		unsigned long entitykey= event->getEntityKey();
		unsigned long eventTypeKey = event->getEventTypeKey();
		unsigned long subsystemKey = event->getSubsystemKey();
		long physicalSubsystemKey = event->getPhysicalSubsystemKey();  //This function returns unsigned long, but will save to long.
		std::string value = event->getValue();
		std::string assetName = event->getAssetName();
		DescriptionParameters dparams = event->getDescriptionParameters();
		timeb createTime = event->getCreateTime();

		std::string desc = processDescByParametes( dparams,eventTypeKey  );

		std::string uuid = event->getEventId();  //uuid is prepared in DataPointStateChangeEvent->std::auto_ptr<TA_Base_Core::IDataPointStateChangeData> m_stateChange;

		EventItemInfo * eventDetail = createEventData(true,true,uuid, SOURCE_TABLE_DP_CHANGE, createTime, assetName, desc, value , 
			entitykey, subsystemKey, physicalSubsystemKey, event->getEntityLocationKey(),
			eventTypeKey, "", 0, 0, -1 );

		std::stringstream messageTypeKey;
		messageTypeKey << eventTypeKey;
		//will only send event to EventAgent when it's Loggable && isEnableRealTime
		if( MessageTypeTableAccessor::getInstance().isLoggable(messageTypeKey.str()) 
			&& MessageTypeTableAccessor::getInstance().isRealTimeEvent(messageTypeKey.str()))
		{
			RealTimeEventSenderInstance::instance()->processEvent(eventDetail);
		}

		Event eventSharedPtr(event);
		EventProcessingLibrary::instance()->processEvent( eventSharedPtr);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "An DP state change event has been sent to EventProcessingLibrary, event id is %s", uuid.c_str());
		
		return uuid.c_str();

	}



	std::string AuditMessageSender::sendAuditMessage(const MessageType& messageType,
		unsigned long entityKey,
		unsigned long subsystemKey,						 //++
		long physicalSubsystemKey,				 //++
		unsigned long entityLocationKey,					 //++
		const DescriptionParameters& description,
		const std::string& details,
		const std::string& sessionID,
		unsigned long operatorKey,                         // ++
		std::string& operatorName,               // ++ 
		long profileID,                           // no need for realtime event. 
		unsigned long sessionLocationKey,                  // no need for realtime event. 
		std::string& value ,			         // ++
		const std::string& alarmID,
		const std::string& incidentID,
		const std::string& parentEventID,
		const std::string& assetName,
		const FilterData* filterableData /* = 0 */,
		const timeb& timeStamp /* = 0 */,
		bool etyInfoValid  /*= true*/,
		bool sessionInfoValid /* =true */) 
	{
		timeb createTime = timeStamp;
		std::string desc = "";
		std::string parameterListStr = "";
		
		std::string uuid = sendAuditMessageBeforeToAgent(messageType, entityKey, description, 
			details, sessionID, alarmID, incidentID, parentEventID, assetName, filterableData, createTime, desc, parameterListStr);

		std::string messgaeTypeKey = messageType.getTypeKey();

		//will only send event to EventAgent when it's Loggable && isEnableRealTime
		if( MessageTypeTableAccessor::getInstance().isLoggable(messgaeTypeKey) 
			&& MessageTypeTableAccessor::getInstance().isRealTimeEvent(messgaeTypeKey))
		{
			unsigned long eventTypeKey =strtoul( messgaeTypeKey.c_str(), NULL, 10); 

			if(value == "") //if value not provided
			{
				getValueForEvent(eventTypeKey, description, EVENT_TYPE_NAME_STUB,parameterListStr, value);  
			}

			// send the event to Realtime event
			EventItemInfo * eventDetail = createEventData(etyInfoValid,sessionInfoValid,
				uuid, SOURCE_TABLE_EVENT, createTime, assetName, desc, value , 
				entityKey, subsystemKey, physicalSubsystemKey, entityLocationKey,
				eventTypeKey, "", 0, 0, profileID,
				"", 0,sessionID, sessionLocationKey, operatorKey, operatorName);
			RealTimeEventSenderInstance::instance()->processEvent(eventDetail);
		}

		// return the message's uuid
		return uuid;
	}

	
	std::string AuditMessageSender::sendAuditMessageWithAsset( const MessageType& messageType,
		unsigned long entityKey,
		const DescriptionParameters& description,
		const std::string& details,
		const std::string& sessionID,
		const std::string& alarmID,
		const std::string& incidentID,
		const std::string& parentEventID,
		const std::string& assetName,
		const FilterData* filterableData /* = 0 */,
		const timeb& timeStamp /* = 0 */)
	{
		std::string operatorName = "";
		std::string value = "";
		return sendAuditMessage(messageType,entityKey,0,0,
			0,description,details,sessionID,0,operatorName,0,0,value,alarmID,
			incidentID, parentEventID, assetName,filterableData,timeStamp,false,false);
	}


    std::string AuditMessageSender::sendAuditMessageBeforeToAgent( const MessageType& messageType,
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
															   std::string& parameterListStr)
    {
        // Collate Variable Header fields
        FilterData varHeader;

        // as priority is currently not used and MessageTypeTableAccessor is expensive, disable it and assign to 0;
        //short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        //short priority = 0;
        std::string priorityString( "0" );
        NameValuePair priorityField( CosNotification::Priority, priorityString );
        varHeader.push_back( &priorityField );

        // Create AuditMessageCorbaDef and fill in details
        AuditMessageCorbaDef* auditMessage = new AuditMessageCorbaDef;

        // If timestamp is specified, use it.
        if ( timeStamp.time != 0 )
        {
            auditMessage->createTime.time = timeStamp.time;
            auditMessage->createTime.millitm = timeStamp.millitm;
        }
        else
        {
            // else use the current time.
            timeb currentTime;
            ftime(&currentTime);
            auditMessage->createTime.time = currentTime.time;
            auditMessage->createTime.millitm = currentTime.millitm;
        }
		
		//save time to timeStamp:
		timeStamp.time = auditMessage->createTime.time;
		timeStamp.millitm = auditMessage->createTime.millitm;

        auditMessage->logTime.time = 0; // unknown
        auditMessage->logTime.millitm = 0;
        auditMessage->messageTypeKey = messageType.getTypeKey().c_str();
        auditMessage->assocEntityKey = entityKey;

        // Create the message description
        parameterListStr="";
        gGetStringFromParameters( description, parameterListStr );
        auditMessage->messageParameterList = parameterListStr.c_str();
		descriptionStr = "";
		descriptionStr = MessageTypeTableAccessor::getInstance().getExpandedDescription( messageType, description );
        auditMessage->messageDescription = descriptionStr.c_str();

        auditMessage->messageDetails = details.c_str();
        auditMessage->sessionID = sessionID.c_str();
        auditMessage->assocAlarmID = alarmID.c_str();
        auditMessage->assocIncidentID = incidentID.c_str();
        auditMessage->parentEventID = parentEventID.c_str();
        auditMessage->assetName = assetName.c_str();
        // TD13673
        unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
        auditMessage->locationKey = locationKey;

        // generate the unique identifier
        std::string uuid = TA_Base_Core::TA_Uuid::generateUUID();
        auditMessage->eventID = uuid.c_str();

        // TD8684: The event processing library is now notified of events. It in turn
        //         will ensure that the events are logged.
        Event auditEvent( new AuditEvent( *auditMessage ) );
        EventProcessingLibrary::instance()->processEvent( auditEvent );

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "A non-alarm event has been sent to EventProcessingLibrary, event id is %s", uuid.c_str());

        // Create Structured Event, inject message into Any field
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= auditMessage;
        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        messageType.getTypeKey(),
                                        &varHeader,
                                        filterableData );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);

        // return the message's uuid
        return uuid;
    }


    std::string AuditMessageSender::sendAuditMessage( const MessageType& messageType,
                                                      unsigned long entityKey,
                                                      const DescriptionParameters& description,
                                                      const std::string& details,
                                                      const std::string& sessionID,
                                                      const std::string& alarmID,
                                                      const std::string& incidentID,
                                                      const std::string& parentEventID,
                                                      const FilterData* filterableData /* = 0 */,
                                                      const timeb& timeStamp /* = 0 */)
    {
        // TODO: The asset name could be set to the entity name here, similar to how alarms
        // are handled.  But the entity name is not specified as an argument, and it is not
        // clear how we could efficiently get it without requiring a database access.

        return sendAuditMessageWithAsset( messageType,
                                          entityKey,
                                          description,
                                          details,
                                          sessionID,
                                          alarmID,
                                          incidentID,
                                          parentEventID,
                                          "",
                                          filterableData,
                                          timeStamp );
    }

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
	    timeb& timeStamp,
		std::string & descriptionStr)
	{
		// Collate Variable Header fields
		FilterData varHeader;

		// as priority is currently not used and MessageTypeTableAccessor is expensive, disable it and assign to 0;
		std::string priorityString( "0" );
		NameValuePair priorityField( CosNotification::Priority, priorityString );
		varHeader.push_back( &priorityField );

		// Create AuditMessageCorbaDef and fill in details
		AuditMessageCorbaDef* auditMessage = new AuditMessageCorbaDef;

		// If timestamp is specified, use it.
		if (timeStamp.time != 0)
		{
			auditMessage->createTime.time = timeStamp.time;
			auditMessage->createTime.millitm = timeStamp.millitm;
		}
		else
		{
			// else use the current time.
			timeb currentTime;
			ftime(&currentTime);
			auditMessage->createTime.time = currentTime.time;
			auditMessage->createTime.millitm = currentTime.millitm;
		}
		timeStamp.time = auditMessage->createTime.time;
		timeStamp.millitm = auditMessage->createTime.millitm;

		auditMessage->logTime.time = 0; // unknown
		auditMessage->logTime.millitm = 0;
		auditMessage->messageTypeKey = CORBA::string_dup( messageType.getTypeKey().c_str() );
		auditMessage->assocEntityKey = entityKey;

		// Create the message description
		std::string messageDescription;
		gGetStringFromParameters( description, messageDescription );
		auditMessage->messageParameterList = CORBA::string_dup( messageDescription.c_str() );
		descriptionStr = MessageTypeTableAccessor::getInstance().getExpandedDescription( messageType, description );
		auditMessage->messageDescription = CORBA::string_dup(descriptionStr.c_str() );
		

		auditMessage->messageDetails = CORBA::string_dup( details.c_str() );
		auditMessage->sessionID = CORBA::string_dup( sessionID.c_str() );
		auditMessage->assocAlarmID = CORBA::string_dup( alarmID.c_str() );
		auditMessage->assocIncidentID = CORBA::string_dup( incidentID.c_str() );
		auditMessage->parentEventID = CORBA::string_dup( parentEventID.c_str() );
		auditMessage->assetName = CORBA::string_dup( assetName.c_str() );
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
		auditMessage->locationKey = locationKey;

		// generate the unique identifier
		std::string uuid = TA_Base_Core::TA_Uuid::generateUUID();
		auditMessage->eventID = CORBA::string_dup( uuid.c_str() );

		Event auditEvent( new AuditEvent( *auditMessage, alarmDescription, eventLelOrSeverityKey, alarmvalue,
			alarmTypeKey, alarmComments, alarmMMSState, alarmDSSState, alarmAVLstate, alarmACKedBy, alarmStatus ,OMFlag ) );
		EventProcessingLibrary::instance()->processEvent( auditEvent );
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "An alarm-event has been sent to EventProcessingLibrary, event id is %s", uuid.c_str());

		// Create Structured Event, inject message into Any field
		CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
		event->remainder_of_body <<= auditMessage;
		gPopulateStructuredEventHeader( 
			*event, 
			messageType.getContextObj().getContextName(), 
			messageType.getTypeKey(), 
			&varHeader, 
			filterableData );

		TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
		m_supplier->publishEvent(*event);

		// return the message's uuid
		return uuid;
	}

	std::string AuditMessageSender::sendAuditMessageEx( const MessageType& messageType, 
		const unsigned long entityKey,
		unsigned long subsystemKey,                   //++, 
		long physicalSubsystemKey,           //++
		unsigned long entityLocationKey,              //++   // can this param be processed by source? see processing func : processLocKeyForAlarm
		const DescriptionParameters&description, 
		const std::string& details, 
		const std::string& sessionID, 
		unsigned long operatorKey,                       //++
		std::string operatorName,              //++
		std::string severityName,                 //++
		//unsigned long actionId,             //++
		unsigned long sessionLocationKey,        // no need for realtime event
		long profileID,                           // no need for realtime event 
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
		bool etyInfoValid ,
		bool sessionInfoValid) 
	{
		timeb createTime =  timeStamp;
		std::string descriptionStr = "";
		std::string uuid = sendAuditMessageExBeforeToAgent (messageType, entityKey, description, details, sessionID, alarmID,incidentID,
			parentEventID, assetName, alarmDescription, eventLelOrSeverityKey, alarmvalue, alarmTypeKey, alarmComments,
			alarmMMSState, alarmDSSState, alarmAVLstate, alarmACKedBy,alarmStatus, OMFlag, filterableData, createTime,descriptionStr);

		std::string messgaeTypeKey = messageType.getTypeKey();

		//will only send event to EventAgent when it's Loggable && isEnableRealTime
		if( MessageTypeTableAccessor::getInstance().isLoggable(messgaeTypeKey) 
			&& MessageTypeTableAccessor::getInstance().isRealTimeEvent(messgaeTypeKey))
		{
			unsigned long eventTypeKey =strtoul( messgaeTypeKey.c_str(), NULL, 10); 

			EventItemInfo * eventDetail = createEventData(etyInfoValid,sessionInfoValid,
				uuid, SOURCE_TABLE_EVENT, createTime, assetName, descriptionStr, alarmvalue , 
				entityKey, subsystemKey, physicalSubsystemKey, entityLocationKey,
				eventTypeKey, "", INVALID_KEY, INVALID_KEY, profileID, OMFlag, 0,sessionID,
				sessionLocationKey,operatorKey,operatorName,alarmID,eventLelOrSeverityKey,severityName,alarmTypeKey,alarmMMSState,
				alarmDSSState, alarmAVLstate,alarmComments,alarmACKedBy,alarmStatus,"");
			RealTimeEventSenderInstance::instance()->processEvent(eventDetail);
		}


		return uuid;

	}


    // CL-21508 begin
    std::string AuditMessageSender::sendAuditMessageEx( const MessageType& messageType, 
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
        const FilterData* filterableData/* = NULL*/,
        const timeb& timeStamp/* = TA_Base_Core::AlarmConstants::defaultTime*/ )
    {
		return sendAuditMessageEx(messageType,entityKey,0,0, 0,description,
			details,sessionID,0,"","",0,-1,alarmID, incidentID,parentEventID,assetName,
			alarmDescription,eventLelOrSeverityKey,alarmvalue,alarmTypeKey,alarmComments,alarmMMSState,alarmDSSState, alarmAVLstate,
			alarmACKedBy, alarmStatus,OMFlag, filterableData, timeStamp, false, false);

    }
    // CL-21508 end
}; // TA_Base_Core
