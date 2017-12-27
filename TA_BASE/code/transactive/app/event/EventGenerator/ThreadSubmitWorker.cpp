//Author: Noel R. Tajanlangit
#include "stdafx.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/uuid/src/TAuuid.h"
#include "app/event/EventGenerator/ThreadSubmitMgr.h"
#include "app/event/EventGenerator/ThreadSubmitWorker.h"
#include "app/event/EventGenerator/CommonDef.h"
#include "app/event/EventGenerator/DataGenerator.h"

#include "core/event/src/EventHelper.h"


using namespace TA_Base_Core;


ThreadSubmitWorker::ThreadSubmitWorker(
	ThreadSubmitMgr* pMyManager, 
	int nSubmitInterval,
	int nSubmitLocation)
	: m_pMyManager(pMyManager)
	, m_nSubmitInterval(nSubmitInterval) 
	, m_nSubmitLocation(nSubmitLocation)
	, m_bTerminate(false)
{
	// Set Event Agent 
	TA_Base_Core::EventHelper::getInstance().init(m_nSubmitLocation);
}

ThreadSubmitWorker::~ThreadSubmitWorker()
{
	terminateAndWait();
}

void ThreadSubmitWorker::run()
{
	T_EventItemInfoList eventsUnsubmitted;
	bool bSuccessful = true;

	while(!m_bTerminate)
	{
		T_EventItemInfoList events;
		if (bSuccessful)
			_getEventsToSubmit(events);
		else
			events = eventsUnsubmitted;

		bSuccessful = false;
		try
		{
			if (events.size()>0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Submitting %d events to location %d", events.size(), m_nSubmitLocation);
				//Submit these events to central Event Agent
// 				CORBA_CALL(m_eventAgentNameObj, 
// 					submitEvents, 
// 					(events));

				TA_Base_Core::EventHelper::getInstance().submitEvents(events);
				_updateCounters(events.size());
			}
			else
				break; //Terminate

			bSuccessful = true;
		}
		catch( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
				TA_Base_Core::DebugUtil::DebugError, "caught unknown exception");
		}	
		
		if (!bSuccessful) 
		{
			eventsUnsubmitted = events;
			AfxGetApp()->GetMainWnd()->PostMessage(WMU_UPDATE_ERROR_COUNT, 0, 0);
			if(m_bTerminate)
				break;
			TA_Base_Core::Thread::sleep(3000); //Sleep 3 seconds more before trying again
		}
		else
		{
			T_EventItemInfoList::iterator itr = events.begin();
			for (itr; itr != events.end(); itr++)
			{
				delete (*itr);
			}
			events.clear();
		}

		if(m_bTerminate)
			break;

		TA_Base_Core::Thread::sleep(m_nSubmitInterval);
	}
}

void ThreadSubmitWorker::terminate()
{
	m_bTerminate = true;
}

void ThreadSubmitWorker::_getEventsToSubmit(TA_Base_Core::T_EventItemInfoList& events)
{
	int nSubmitSize = m_pMyManager->getEventSize();
	
	if (nSubmitSize==0)
		return;

	for (int ctr=0; ctr<nSubmitSize; ctr++)
	{
// 		TA_Base_Core::EventItem* eventData = new TA_Base_Core::EventItem(
// 			TA_Base_Core::TA_Uuid::generateUUID().c_str()
// 			,"Test Table"
// 			,TA_Base_Core::MillisecondTimeEx( time(0), 0)
// 			,16
// 			,10
// 			,1
// 			,3
// 			,"Test Severity"
// 			,"Test Asset"
// 			,"Test Description"
// 			,1
// 			,"Test"
// 			,TA_Base_Core::TA_Uuid::generateUUID().c_str()
// 			,10050
// 			,"Test"
// 			,"S"
// 			,"D"
// 			,"Test"
// 			,622
// 			,"Test Operator"
// 			,"No Comment"
// 			,1
// 			,"Noel"
// 			,"1"
// 			,TA_Base_Core::TA_Uuid::generateUUID().c_str()
// 			,1
// 			,0
// 			,999
// 			,"1"
// 			,12312312
// 			,"1"
// 			,99
// 			,TA_Base_Core::EvMgsSequenceNumber(0 , 0));

		TA_Base_Core::EventItem* eventData = CDataGenerator::getInstance().createRandomEvent();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Generate event with ID %s", eventData->eventID);
		
		TA_Base_Core::EventItemInfo* pEventInfo = new TA_Base_Core::EventItemInfo;
		pEventInfo->isEntityInfoValid = false;
		pEventInfo->isSessionInfoValid = false;
		pEventInfo->eventItemDetail = eventData;

		events.push_back(pEventInfo);
	}

}


void ThreadSubmitWorker::_updateCounters(int nSubmitSize)
{
	//AfxGetMainWnd()->PostMessage(WMU_UPDATE_COUNTERS, (WPARAM)nSubmitSize, 0);
	AfxGetApp()->GetMainWnd()->PostMessage(WMU_UPDATE_COUNTERS, (WPARAM)nSubmitSize, 0);
}