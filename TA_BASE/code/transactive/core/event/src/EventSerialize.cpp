/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventSerialize.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventSerialize.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <vector>

namespace TA_Base_Core
{
	//Constructor
	EventSerialize::EventSerialize(const T_EventItemList& pEventList)
		: m_pEventList(pEventList)
	{
	}

	//Destructor
	EventSerialize::~EventSerialize(){}

	//Inherited function
	char* EventSerialize::processEvent()
	{
		T_EvSzItemInfoList evIteminfoList;

		unsigned long ulSize = calculateDataLen(evIteminfoList);
		char* pszRetData = new char[ulSize];
		memset(pszRetData, 0, ulSize);
		
		EvItemlistInfo objItemLstInfo;
		objItemLstInfo.ulListSize = m_pEventList.size();

		char* wPos = pszRetData;
		memcpy(wPos, &objItemLstInfo, EV_SIZEOF_ITEM_LIST_INFO);
		wPos += EV_SIZEOF_ITEM_LIST_INFO;

		unsigned long ulCtr = 0;
		T_EventItemList::const_iterator itr = m_pEventList.begin();
		for (itr; itr != m_pEventList.end(); itr++)
		{
			wPos = serializeEvent(wPos, evIteminfoList[ulCtr], (*itr));
			ulCtr++;
		}
		
		return pszRetData;
	}

	char* EventSerialize::serializeEvent(char* pWPos, EvSzItemInfo& pEvSzItemInfo, EventItem* pEventData)
	{
		char* wPos = pWPos;
		//Write Header Info
		memcpy(wPos, &pEvSzItemInfo, EV_SIZEOF_ITEM_INFO); wPos+=EV_SIZEOF_ITEM_INFO;

		//Write data content
		memcpy(wPos, pEventData->eventID, EV_EVENT_ID_SIZE); wPos+=EV_EVENT_ID_SIZE;
		memcpy(wPos, pEventData->sourceTable, EV_SOURCE_TABLE_SIZE); wPos+=EV_SOURCE_TABLE_SIZE;
		memcpy(wPos, pEventData->alarmStatus, EV_ALARM_STATUS_SIZE); wPos+=EV_ALARM_STATUS_SIZE;
		memcpy(wPos, pEventData->operatorMode, EV_OPERATION_MODE_SIZE); wPos+=EV_OPERATION_MODE_SIZE;
		memcpy(wPos, &pEventData->createTime.time, EV_CREATETIME_TIME_SIZE);  wPos+=EV_CREATETIME_TIME_SIZE;
		memcpy(wPos, &pEventData->createTime.millitm, EV_CREATETIME_MILLI_SIZE);  wPos+=EV_CREATETIME_MILLI_SIZE;
		memcpy(wPos, &pEventData->subsystemKey, EV_SUBSYSTEM_KEY_SIZE); wPos+=EV_SUBSYSTEM_KEY_SIZE;
		memcpy(wPos, &pEventData->physicalSubsystemKey, EV_PHYSICAL_SUBSYSTEM_KEY_SIZE); wPos+=EV_PHYSICAL_SUBSYSTEM_KEY_SIZE;
		memcpy(wPos, &pEventData->locationKey, EV_LOCATION_KEY_SIZE); wPos+=EV_LOCATION_KEY_SIZE;
		memcpy(wPos, &pEventData->severityKey, EV_SEVERITY_KEY_SIZE); wPos+=EV_SEVERITY_KEY_SIZE;
		memcpy(wPos, &pEventData->eventTypeKey, EV_EVENT_TYPE_KEY_SIZE); wPos+=EV_EVENT_TYPE_KEY_SIZE;
		memcpy(wPos, &pEventData->alarmTypeKey, EV_ALARM_TYPE_KEY_SIZE); wPos+=EV_ALARM_TYPE_KEY_SIZE;
		memcpy(wPos, &pEventData->operatorKey, EV_OPERATOR_KEY_SIZE); wPos+=EV_OPERATOR_KEY_SIZE;
		memcpy(wPos, &pEventData->eventLevel, EV_EVENT_LEVEL_SIZE); wPos+=EV_EVENT_LEVEL_SIZE;
		memcpy(wPos, &pEventData->sessionLocation, EV_SESSION_LOCATION_SIZE); wPos+=EV_SESSION_LOCATION_SIZE;
		memcpy(wPos, &pEventData->profileID, EV_PROFILE_ID_SIZE); wPos+=EV_PROFILE_ID_SIZE;
		memcpy(wPos, &pEventData->actionID, EV_ACTION_ID_SIZE); wPos+=EV_ACTION_ID_SIZE;
		memcpy(wPos, &pEventData->entitykey, EV_ENTITY_KEY_SIZE); wPos+=EV_ENTITY_KEY_SIZE;
		memcpy(wPos, &pEventData->systemKey, EV_SYSTEM_KEY_SIZE); wPos+=EV_SYSTEM_KEY_SIZE;
		memcpy(wPos, &pEventData->eventSequence.ulHighOrder, EV_SEQ_HIGH_ORDER_SIZE); wPos+=EV_SEQ_HIGH_ORDER_SIZE;
		memcpy(wPos, &pEventData->eventSequence.ulLowOrder, EV_SEQ_LOW_ORDER_SIZE); wPos+=EV_SEQ_LOW_ORDER_SIZE;
		memcpy(wPos, pEventData->severityname, EV_SEVERITY_NAME_SIZE); wPos+=EV_SEVERITY_NAME_SIZE;
		memcpy(wPos, pEventData->alarmID, EV_ALARM_ID_SIZE); wPos+=EV_ALARM_ID_SIZE;
		memcpy(wPos, pEventData->mms, EV_MMS_STATE_SIZE); wPos+=EV_MMS_STATE_SIZE;
		memcpy(wPos, pEventData->decisionSupportState, EV_DSS_STATE_SIZE); wPos+=EV_DSS_STATE_SIZE;
		memcpy(wPos, pEventData->avalanceState, EV_AVL_STATE_SIZE); wPos+=EV_AVL_STATE_SIZE;
		memcpy(wPos, pEventData->alarmAckedBy, EV_ALARM_ACK_SIZE); wPos+=EV_ALARM_ACK_SIZE;
		memcpy(wPos, pEventData->sessionID, EV_SESSION_KEY_SIZE); wPos+=EV_SESSION_KEY_SIZE;
		memcpy(wPos, pEventData->avlheadID, EV_AVLALARMHEADID_SIZE); wPos+=EV_AVLALARMHEADID_SIZE;
		
		//Asset Name
		memcpy(wPos, pEventData->assetname, pEvSzItemInfo.ulAssetNamelen); wPos+=pEvSzItemInfo.ulAssetNamelen;
		
		// Description
		memcpy(wPos, pEventData->description, pEvSzItemInfo.ulDescriptionlen); wPos+=pEvSzItemInfo.ulDescriptionlen;

		// Even Type Name
		memcpy(wPos, pEventData->eventTypeName, pEvSzItemInfo.ulEvTypeNamelen); wPos+=pEvSzItemInfo.ulEvTypeNamelen;

		// Value
		memcpy(wPos, pEventData->value, pEvSzItemInfo.ulValuelen); wPos+=pEvSzItemInfo.ulValuelen;
		
		// Operator Name
		memcpy(wPos, pEventData->operatorName, pEvSzItemInfo.ulOperatorNamelen); wPos+=pEvSzItemInfo.ulOperatorNamelen;

		// Alarm Comment
		memcpy(wPos, pEventData->alarmComment, pEvSzItemInfo.ulAlarmCommentlen); wPos+=pEvSzItemInfo.ulAlarmCommentlen;

		return wPos;
	}


	EvDataPkg* EventSerialize::processEventEx()
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "EventSerialize::processEventEx Start"); 

		T_EvSzItemInfoList evIteminfoList;
		EvDataPkg* pEvPkgRetVal = new EvDataPkg;
		pEvPkgRetVal->ulDataLen = calculateDataLen(evIteminfoList) + sizeof(EvItemlistInfo);
		pEvPkgRetVal->pszData = new char[pEvPkgRetVal->ulDataLen];
		memset(pEvPkgRetVal->pszData, 0, pEvPkgRetVal->ulDataLen);

		EvItemlistInfo objItemLstInfo;
		objItemLstInfo.ulListSize = m_pEventList.size();

		char* wPos = pEvPkgRetVal->pszData;
		memcpy(wPos, &objItemLstInfo, sizeof(EvItemlistInfo));
		wPos += sizeof(EvItemlistInfo);

		unsigned long ulCtr = 0;
		T_EventItemList::const_iterator itr = m_pEventList.begin();
		for (itr; itr != m_pEventList.end(); itr++)
		{
			wPos = serializeEvent(wPos, evIteminfoList[ulCtr], (*itr));
			ulCtr++;
		}

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "EventSerialize::processEventEx End"); 
		return pEvPkgRetVal;
	}

	unsigned long EventSerialize::calculateDataLen(T_EvSzItemInfoList & evIteminfoList)
	{
		unsigned long ulRetVal = 0;
		evIteminfoList.reserve(m_pEventList.size());

		T_EventItemList::const_iterator itr = m_pEventList.begin();
		for (itr; itr != m_pEventList.end(); itr++)
		{
			EvSzItemInfo objEvItemInfo;
			objEvItemInfo.ulAssetNamelen = strlen((*itr)->assetname) + 1;
			objEvItemInfo.ulDescriptionlen = strlen((*itr)->description) + 1;
			objEvItemInfo.ulEvTypeNamelen = strlen((*itr)->eventTypeName) + 1;
			objEvItemInfo.ulValuelen = strlen((*itr)->value) + 1;
			objEvItemInfo.ulOperatorNamelen = strlen((*itr)->operatorName) + 1;
			objEvItemInfo.ulAlarmCommentlen = strlen((*itr)->alarmComment) + 1;

			objEvItemInfo.ulTotalDatalen = EV_DATA_FIXED_SIZE + 
				objEvItemInfo.ulAssetNamelen + objEvItemInfo.ulDescriptionlen +
				objEvItemInfo.ulEvTypeNamelen + objEvItemInfo.ulValuelen + 
				objEvItemInfo.ulOperatorNamelen + objEvItemInfo.ulAlarmCommentlen + 
				sizeof(EvSzItemInfo);

			ulRetVal +=objEvItemInfo.ulTotalDatalen;

			evIteminfoList.push_back(objEvItemInfo);
		}
		
		return ulRetVal;
	}
}