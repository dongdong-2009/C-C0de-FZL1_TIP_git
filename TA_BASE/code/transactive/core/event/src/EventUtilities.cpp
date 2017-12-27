/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventUtilities.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventUtilities.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/event/src/EventDeSerialize.h"
#include "core/event/src/EventZlibInflate.h"

namespace TA_Base_Core
{

	/**
	 * EventCorbaToEventItem
	 */
	TA_Base_Core::EventItem* EventUtilities::EventCorbaToEventItem(const TA_Base_Core::EventDetailCorbaDef& evDetailCorba)
	{
		FUNCTION_ENTRY("EventUtilities::EventCorbaToEventItem");

		TA_Base_Core::EventItem* pRetVal = NULL;

		try
		{
			pRetVal = new TA_Base_Core::EventItem(
				evDetailCorba.eventID.in(),
				evDetailCorba.sourceTable.in(),
				TA_Base_Core::MillisecondTimeEx(evDetailCorba.createTime.time, 
				evDetailCorba.createTime.millitm),
				evDetailCorba.subsystemKey,
				evDetailCorba.physicalSubsystemKey,
				evDetailCorba.locationKey,
				evDetailCorba.severityKey,
				evDetailCorba.severityname.in(),
				evDetailCorba.assetname.in(),
				evDetailCorba.description.in(),
				evDetailCorba.eventTypeKey,
				evDetailCorba.eventTypeName.in(),
				evDetailCorba.alarmID.in(),
				evDetailCorba.alarmTypeKey,
				evDetailCorba.value.in(),
				evDetailCorba.mms.in(),
				evDetailCorba.decisionSupportState.in(),
				evDetailCorba.avalanceState.in(),
				evDetailCorba.operatorKey,
				evDetailCorba.operatorName.in(),
				evDetailCorba.alarmComment.in(),
				evDetailCorba.eventLevel,
				evDetailCorba.alarmAckedBy.in(),
				evDetailCorba.alarmStatus.in(),
				evDetailCorba.sessionID.in(),
				evDetailCorba.sessionLocation,
				evDetailCorba.profileID,
				evDetailCorba.actionID,
				evDetailCorba.operatorMode.in(),
				evDetailCorba.entitykey,
				evDetailCorba.avlheadID.in(),
				evDetailCorba.systemKey,
				TA_Base_Core::EvMgsSequenceNumber(evDetailCorba.eventSequence.ulHighOrder, 
				evDetailCorba.eventSequence.ulLowOrder));
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert corba type to event data type.");
		}

		return pRetVal;
		FUNCTION_EXIT;
	}

	/**
	 * EventItemToEventCorba
	 */
	void EventUtilities::EventItemToEventCorba(TA_Base_Core::EventItem* eventItem, TA_Base_Core::EventDetailCorbaDef& refRetVal)
	{
		FUNCTION_ENTRY("EventUtilities::EventItemToEventCorba");

		try
		{
			refRetVal.eventID = CORBA::string_dup(eventItem->eventID);
			refRetVal.sourceTable = CORBA::string_dup(eventItem->sourceTable);
			refRetVal.createTime.time = eventItem->createTime.time;
			refRetVal.createTime.millitm = eventItem->createTime.millitm;
			refRetVal.subsystemKey = eventItem->subsystemKey; 
			refRetVal.physicalSubsystemKey = eventItem->physicalSubsystemKey;
			refRetVal.locationKey = eventItem->locationKey;
			refRetVal.severityKey = eventItem->severityKey;
			refRetVal.severityname = CORBA::string_dup(eventItem->severityname);
			refRetVal.assetname = CORBA::string_dup(eventItem->assetname);
			refRetVal.description = CORBA::string_dup(eventItem->description);
			refRetVal.eventTypeKey = eventItem->eventTypeKey;
			refRetVal.eventTypeName = CORBA::string_dup(eventItem->eventTypeName);
			refRetVal.alarmID = CORBA::string_dup(eventItem->alarmID);
			refRetVal.alarmTypeKey = eventItem->alarmTypeKey;
			refRetVal.value = CORBA::string_dup(eventItem->value);
			refRetVal.mms = CORBA::string_dup(eventItem->mms);
			refRetVal.decisionSupportState = CORBA::string_dup(eventItem->decisionSupportState);
			refRetVal.avalanceState = CORBA::string_dup(eventItem->avalanceState);
			refRetVal.operatorKey = eventItem->operatorKey;
			refRetVal.operatorName = CORBA::string_dup(eventItem->operatorName);
			refRetVal.alarmComment = CORBA::string_dup(eventItem->alarmComment);
			refRetVal.eventLevel = eventItem->eventLevel;
			refRetVal.alarmAckedBy = CORBA::string_dup(eventItem->alarmAckedBy);
			refRetVal.alarmStatus = CORBA::string_dup(eventItem->alarmStatus);
			refRetVal.sessionID = CORBA::string_dup(eventItem->sessionID);
			refRetVal.sessionLocation = eventItem->sessionLocation;
			refRetVal.profileID = eventItem->profileID;
			refRetVal.actionID = eventItem->actionID;
			refRetVal.operatorMode = CORBA::string_dup(eventItem->operatorMode);
			refRetVal.entitykey = eventItem->entitykey;
			refRetVal.avlheadID = CORBA::string_dup(eventItem->avlheadID);
			refRetVal.systemKey = eventItem->systemKey;
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert Event Item to Event Corba type.");
		}

		FUNCTION_EXIT;
	}

	/**
      * EventInfoToCorbaDef
      */
	void EventUtilities::EventInfoToCorbaDef(EventItemInfo* event, EventInfoCorbaDef& refCorbaObj)
	{
		FUNCTION_ENTRY("EventUtilities::EventInfoToCorbaDef");

		try
		{			
			refCorbaObj.isEntityInfoValid = event->isEntityInfoValid;
			refCorbaObj.isSessionInfoValid = event->isSessionInfoValid;

			refCorbaObj.eventDetail.eventID = CORBA::string_dup(event->eventItemDetail->eventID);
			refCorbaObj.eventDetail.sourceTable = CORBA::string_dup(event->eventItemDetail->sourceTable);
			refCorbaObj.eventDetail.createTime.time = event->eventItemDetail->createTime.time;
			refCorbaObj.eventDetail.createTime.millitm = event->eventItemDetail->createTime.millitm;
			refCorbaObj.eventDetail.subsystemKey = event->eventItemDetail->subsystemKey; 
			refCorbaObj.eventDetail.physicalSubsystemKey = event->eventItemDetail->physicalSubsystemKey;
			refCorbaObj.eventDetail.locationKey = event->eventItemDetail->locationKey;
			refCorbaObj.eventDetail.severityKey = event->eventItemDetail->severityKey;
			refCorbaObj.eventDetail.severityname = CORBA::string_dup(event->eventItemDetail->severityname);
			refCorbaObj.eventDetail.assetname = CORBA::string_dup(event->eventItemDetail->assetname);
			refCorbaObj.eventDetail.description = CORBA::string_dup(event->eventItemDetail->description);
			refCorbaObj.eventDetail.eventTypeKey = event->eventItemDetail->eventTypeKey;
			refCorbaObj.eventDetail.eventTypeName = CORBA::string_dup(event->eventItemDetail->eventTypeName);
			refCorbaObj.eventDetail.alarmID = CORBA::string_dup(event->eventItemDetail->alarmID);
			refCorbaObj.eventDetail.alarmTypeKey = event->eventItemDetail->alarmTypeKey;
			refCorbaObj.eventDetail.value = CORBA::string_dup(event->eventItemDetail->value);
			refCorbaObj.eventDetail.mms = CORBA::string_dup(event->eventItemDetail->mms);
			refCorbaObj.eventDetail.decisionSupportState = CORBA::string_dup(event->eventItemDetail->decisionSupportState);
			refCorbaObj.eventDetail.avalanceState = CORBA::string_dup(event->eventItemDetail->avalanceState);
			refCorbaObj.eventDetail.operatorKey = event->eventItemDetail->operatorKey;
			refCorbaObj.eventDetail.operatorName = CORBA::string_dup(event->eventItemDetail->operatorName);
			refCorbaObj.eventDetail.alarmComment = CORBA::string_dup(event->eventItemDetail->alarmComment);
			refCorbaObj.eventDetail.eventLevel = event->eventItemDetail->eventLevel;
			refCorbaObj.eventDetail.alarmAckedBy = CORBA::string_dup(event->eventItemDetail->alarmAckedBy);
			refCorbaObj.eventDetail.alarmStatus = CORBA::string_dup(event->eventItemDetail->alarmStatus);
			refCorbaObj.eventDetail.sessionID = CORBA::string_dup(event->eventItemDetail->sessionID);
			refCorbaObj.eventDetail.sessionLocation = event->eventItemDetail->sessionLocation;
			refCorbaObj.eventDetail.profileID = event->eventItemDetail->profileID;
			refCorbaObj.eventDetail.actionID = event->eventItemDetail->actionID;
			refCorbaObj.eventDetail.operatorMode = CORBA::string_dup(event->eventItemDetail->operatorMode);
			refCorbaObj.eventDetail.entitykey = event->eventItemDetail->entitykey;
			refCorbaObj.eventDetail.avlheadID = CORBA::string_dup(event->eventItemDetail->avlheadID);
			refCorbaObj.eventDetail.systemKey = event->eventItemDetail->systemKey;
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert Event Info to Event Corba type.");
		}

		FUNCTION_EXIT;
	}

	/**
      * UnpackPackage
      */
	void EventUtilities::UnpackPackage(const TA_Base_Core::EvDataPkgCorbaDef* pEventPkg, TA_Base_Core::T_EventItemList& eventList)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start Unpacking Data");
		std::auto_ptr<EventZlibInflate> pObjZlibInflate (new EventZlibInflate((char*)pEventPkg->eventDetail.get_buffer(), pEventPkg->eventDetail.length(), pEventPkg->ulDataSize));
		std::auto_ptr<EventDeSerialize> pDeserialize(new EventDeSerialize(pObjZlibInflate.get(), eventList));

		pDeserialize->processEvent(); // Process data

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End Unpacking Data.");
	}

}
