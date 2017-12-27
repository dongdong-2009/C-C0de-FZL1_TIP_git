/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventDeSerialize.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventDeSerialize.h"

namespace TA_Base_Core
{
	//Constructor
	EventDeSerialize::EventDeSerialize(char* _pszData, T_EventItemList& _pEventList)
		: m_pEventList(_pEventList)
		, m_pszData(_pszData)
		, m_pObjEvProcessor(0)
	{
	}

	EventDeSerialize::EventDeSerialize(IEventDataProcessor* _pObjEvProcessor, T_EventItemList& _pEventList)
		: m_pEventList(_pEventList)
		, m_pszData(0)
		, m_pObjEvProcessor(_pObjEvProcessor)
	{
	}

	//Destructor
	EventDeSerialize::~EventDeSerialize()
	{
		if (0!=m_pszData)
		{
			delete [] m_pszData;
		}
	}

	//Inherited function
	char* EventDeSerialize::processEvent()
	{
		if (NULL != m_pObjEvProcessor)
		{
			m_pszData = m_pObjEvProcessor->processEvent();
		}
		
		EvItemlistInfo objItemLstInfo;
		const char* rPos = m_pszData;
		memcpy(&objItemLstInfo, rPos, EV_SIZEOF_ITEM_LIST_INFO);
		rPos+=EV_SIZEOF_ITEM_LIST_INFO;

		// Reserve the total list
		//m_pEventList.reserve(objItemLstInfo.ulListSize);

		for(unsigned long ulIdx=0; ulIdx < objItemLstInfo.ulListSize; ulIdx++)
		{
			EventItem* pEvItem = new EventItem;

			rPos = deSerializeEvent(rPos, pEvItem);

			m_pEventList.push_back(pEvItem);
		}

		return NULL;
	}

	const char* EventDeSerialize::deSerializeEvent(const char* pszData, EventItem* pEventData)
	{
		const char* rPos = pszData;
		EvSzItemInfo pEvSzItemInfo;
		//Read Header Info
		memcpy(&pEvSzItemInfo, rPos, EV_SIZEOF_ITEM_INFO); rPos+=EV_SIZEOF_ITEM_INFO;

		//Read data content
		memcpy(pEventData->eventID, rPos, EV_EVENT_ID_SIZE); rPos+=EV_EVENT_ID_SIZE;
		memcpy(pEventData->sourceTable, rPos, EV_SOURCE_TABLE_SIZE); rPos+=EV_SOURCE_TABLE_SIZE;
		memcpy(pEventData->alarmStatus, rPos, EV_ALARM_STATUS_SIZE); rPos+=EV_ALARM_STATUS_SIZE;
		memcpy(pEventData->operatorMode, rPos, EV_OPERATION_MODE_SIZE); rPos+=EV_OPERATION_MODE_SIZE;
		memcpy(&pEventData->createTime.time, rPos, EV_CREATETIME_TIME_SIZE);  rPos+=EV_CREATETIME_TIME_SIZE;
		memcpy(&pEventData->createTime.millitm, rPos, EV_CREATETIME_MILLI_SIZE);  rPos+=EV_CREATETIME_MILLI_SIZE;
		memcpy(&pEventData->subsystemKey, rPos, EV_SUBSYSTEM_KEY_SIZE); rPos+=EV_SUBSYSTEM_KEY_SIZE;
		memcpy(&pEventData->physicalSubsystemKey, rPos, EV_PHYSICAL_SUBSYSTEM_KEY_SIZE); rPos+=EV_PHYSICAL_SUBSYSTEM_KEY_SIZE;
		memcpy(&pEventData->locationKey, rPos, EV_LOCATION_KEY_SIZE); rPos+=EV_LOCATION_KEY_SIZE;
		memcpy(&pEventData->severityKey, rPos, EV_SEVERITY_KEY_SIZE); rPos+=EV_SEVERITY_KEY_SIZE;
		memcpy(&pEventData->eventTypeKey, rPos, EV_EVENT_TYPE_KEY_SIZE); rPos+=EV_EVENT_TYPE_KEY_SIZE;
		memcpy(&pEventData->alarmTypeKey, rPos, EV_ALARM_TYPE_KEY_SIZE); rPos+=EV_ALARM_TYPE_KEY_SIZE;
		memcpy(&pEventData->operatorKey, rPos, EV_OPERATOR_KEY_SIZE); rPos+=EV_OPERATOR_KEY_SIZE;
		memcpy(&pEventData->eventLevel, rPos, EV_EVENT_LEVEL_SIZE); rPos+=EV_EVENT_LEVEL_SIZE;
		memcpy(&pEventData->sessionLocation, rPos, EV_SESSION_LOCATION_SIZE); rPos+=EV_SESSION_LOCATION_SIZE;
		memcpy(&pEventData->profileID, rPos, EV_PROFILE_ID_SIZE); rPos+=EV_PROFILE_ID_SIZE;
		memcpy(&pEventData->actionID, rPos, EV_ACTION_ID_SIZE); rPos+=EV_ACTION_ID_SIZE;
		memcpy(&pEventData->entitykey, rPos, EV_ENTITY_KEY_SIZE); rPos+=EV_ENTITY_KEY_SIZE;
		memcpy(&pEventData->systemKey, rPos, EV_SYSTEM_KEY_SIZE); rPos+=EV_SYSTEM_KEY_SIZE;
		memcpy(&pEventData->eventSequence.ulHighOrder, rPos, EV_SEQ_HIGH_ORDER_SIZE); rPos+=EV_SEQ_HIGH_ORDER_SIZE;
		memcpy(&pEventData->eventSequence.ulLowOrder, rPos, EV_SEQ_LOW_ORDER_SIZE); rPos+=EV_SEQ_LOW_ORDER_SIZE;
		memcpy(pEventData->severityname, rPos, EV_SEVERITY_NAME_SIZE); rPos+=EV_SEVERITY_NAME_SIZE;
		memcpy(pEventData->alarmID, rPos, EV_ALARM_ID_SIZE); rPos+=EV_ALARM_ID_SIZE;
		memcpy(pEventData->mms, rPos, EV_MMS_STATE_SIZE); rPos+=EV_MMS_STATE_SIZE;
		memcpy(pEventData->decisionSupportState, rPos, EV_DSS_STATE_SIZE); rPos+=EV_DSS_STATE_SIZE;
		memcpy(pEventData->avalanceState, rPos, EV_AVL_STATE_SIZE); rPos+=EV_AVL_STATE_SIZE;
		memcpy(pEventData->alarmAckedBy, rPos, EV_ALARM_ACK_SIZE); rPos+=EV_ALARM_ACK_SIZE;
		memcpy(pEventData->sessionID, rPos, EV_SESSION_KEY_SIZE); rPos+=EV_SESSION_KEY_SIZE;
		memcpy(pEventData->avlheadID, rPos, EV_AVLALARMHEADID_SIZE); rPos+=EV_AVLALARMHEADID_SIZE;

		//Asset Name
		memcpy(pEventData->assetname, rPos, pEvSzItemInfo.ulAssetNamelen); rPos+=pEvSzItemInfo.ulAssetNamelen;

		// Description
		memcpy(pEventData->description, rPos, pEvSzItemInfo.ulDescriptionlen); rPos+=pEvSzItemInfo.ulDescriptionlen;

		// Even Type Name
		memcpy(pEventData->eventTypeName, rPos, pEvSzItemInfo.ulEvTypeNamelen); rPos+=pEvSzItemInfo.ulEvTypeNamelen;

		// Value
		memcpy(pEventData->value, rPos, pEvSzItemInfo.ulValuelen); rPos+=pEvSzItemInfo.ulValuelen;

		// Operator Name
		memcpy(pEventData->operatorName, rPos, pEvSzItemInfo.ulOperatorNamelen); rPos+=pEvSzItemInfo.ulOperatorNamelen;

		// Alarm Comment
		memcpy(pEventData->alarmComment, rPos, pEvSzItemInfo.ulAlarmCommentlen); rPos+=pEvSzItemInfo.ulAlarmCommentlen;

		return rPos;
	}
}
