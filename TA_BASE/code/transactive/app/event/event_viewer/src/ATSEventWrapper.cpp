/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSEventBridge.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSEventWrapper is ...
  *
  */
#include <vector>
#include <sys/timeb.h>
#include <atltime.h>

#include "FilterCriteria.h"
#include "ATSEventWrapper.h"
#include "ATSRealTimeObserverManager.h"
#include "ATSRealTimeWorker.h"
#include "ConstantItems.h"
#include "EventHandlerManager.h"

#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/utilities/src/DebugUtil.h"


using namespace TA_Base_Core;
using namespace IATSEventsAPI;
namespace TA_Base_App{

	IATSEventsAPI::ATSEventSubscription* ATSEventWrapper::atsSubscriber = NULL;
	bool ATSEventWrapper::ATS_func_debug_level = true;

	ATSEventWrapper::ATSEventWrapper(){

	}

	ATSEventWrapper::~ATSEventWrapper(){

	}

	ATSEventSubscription* ATSEventWrapper::getATSSubscriber(){
		if ( atsSubscriber == NULL )
		{
			try{
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Comeing into IATSEventsAPI::ATSEventSubscription::CreateATSEventSubscription().");
				atsSubscriber = IATSEventsAPI::ATSEventSubscription::CreateATSEventSubscription();
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::ATSEventSubscription::CreateATSEventSubscription().");
				
				if(atsSubscriber != NULL)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New ATS event subscription created.");
			}

			catch (...)
			{				
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when call ATSEventSubscription::CreateATSEventSubscription().");
			}

		}
		return atsSubscriber;
	}

	void ATSEventWrapper::adjustATSFilterTime(IATSEventsAPI::ATSFilterCriteria & atsfilter)
	{

		const CTime currentTime = CTime::GetCurrentTime();

		if(atsfilter.StartTimePeriodDefined == false && atsfilter.EndTimePeriodDefined == false)
		{   // this is real time mode: [current-1day, current]
			
			CTime startTime(currentTime.GetTime()-EventViewerConstants::TWENTY_FOUR_HOURS);
			ATSDateTime ATSStart(startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(), 
				startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond(),0);
			ATSDateTime ATSEnd (currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), 
				currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond(),0);
			atsfilter.StartTimePeriod = ATSStart;
			atsfilter.EndTimePeriod = ATSEnd;
		}
		else if(atsfilter.StartTimePeriodDefined == true && atsfilter.EndTimePeriodDefined == false)
		{
			// adjust end time to current time or starttime+1day , which comes first
			ISCSDateTime fromTime_ISCS;
			fromTime_ISCS.setByATSDateTime(atsfilter.StartTimePeriod);
			if (fromTime_ISCS.getTime_t() + EventViewerConstants::TWENTY_FOUR_HOURS < currentTime.GetTime() )
			{
				CTime fromTimeAdd1Day( fromTime_ISCS.getTime_t() + EventViewerConstants::TWENTY_FOUR_HOURS );
				atsfilter.EndTimePeriod = ATSDateTime(fromTimeAdd1Day.GetYear(), fromTimeAdd1Day.GetMonth(),fromTimeAdd1Day.GetDay(), 
					fromTimeAdd1Day.GetHour(), fromTimeAdd1Day.GetMinute(), fromTimeAdd1Day.GetSecond(),0);
			}else
			{
				atsfilter.EndTimePeriod = ATSDateTime(currentTime.GetYear(), currentTime.GetMonth(),currentTime.GetDay(), 
					currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond(),0);
			}
			atsfilter.EndTimePeriodDefined = true;

		}
		else if(atsfilter.StartTimePeriodDefined == false && atsfilter.EndTimePeriodDefined == true) 
		{
			// adjust start time as endtime- 1day
			ISCSDateTime endTime_ISCS;
			endTime_ISCS.setByATSDateTime(atsfilter.EndTimePeriod);

			CTime startTime( endTime_ISCS.getTime_t() - EventViewerConstants::TWENTY_FOUR_HOURS);
			atsfilter.StartTimePeriod = ATSDateTime(startTime.GetYear(), startTime.GetMonth(),startTime.GetDay(), 
				startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond(),0);
			atsfilter.StartTimePeriodDefined = true;
		}
	}

	void ATSEventWrapper::transformToATSFilter(ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria){
		
		atsfilter.StartTimePeriodDefined = filterCriteria.StartTimePeriodDefined;
		atsfilter.EndTimePeriodDefined = filterCriteria.EndTimePeriodDefined;

		setStartEndTimeToATSFilter (atsfilter,filterCriteria);

		// adjust the star/end time for these situations:
		adjustATSFilterTime(atsfilter);

		//system , subsystem, location, operator, asset
		atsfilter.SystemNameEqualOr.clear();
		atsfilter.SubsystemNameEqualOr.clear();
		atsfilter.LocationEqualOr.clear();
		atsfilter.OperatorEqualOr.clear();
		atsfilter.AssetEqualOr.clear();

		atsfilter.SystemNameEqualOr.assign(filterCriteria.SystemNameEqualOr.begin(),filterCriteria.SystemNameEqualOr.end());
		atsfilter.SubsystemNameEqualOr.assign(filterCriteria.SubsystemNameEqualOr.begin(),filterCriteria.SubsystemNameEqualOr.end());
		atsfilter.LocationEqualOr.assign(filterCriteria.LocationEqualOr.begin(),filterCriteria.LocationEqualOr.end());
		atsfilter.OperatorEqualOr.assign(filterCriteria.OperatorEqualOr.begin(),filterCriteria.OperatorEqualOr.end());

		if (filterCriteria.Asset.size()>0 && ""!= filterCriteria.Asset.front())
		{
			atsfilter.AssetEqualOr.push_back(filterCriteria.Asset.front().c_str());
		}

		setTMTypeToATSFilter(atsfilter,filterCriteria);

		// set alarm severity:
		std::vector<int> ulVector  = filterCriteria.severityIds;
		if ( !ulVector.empty() )
		{
			atsfilter.AlarmSeverityEqualOr.assign(ulVector.begin(),ulVector.end());  
		}
		// has been confirmed: 0 is used to get Non-alarm events here
		// change -1( in filterCriteria means "Non-alarm") to 0 (in ATSFilter means "Non-alarm")
		atsfilter.AlarmAvalancheTypeEqualOr.clear();
	}

	void ATSEventWrapper::transformToISCSEvents ( std::list<ATSEvent*>& atsEvents, 
		std::vector<TA_Base_Core::ICombinedEventData*>& events , unsigned long lastKey)
 	{
		if (atsEvents.size()==0) return;

		//if the destination is not empty, clean it first
		if (events.size() > 0)
		{
			for (int i=0; i<events.size(); i++)
			{
				if (events[i] != NULL)
				{
					delete events[i];
					events[i]=NULL;
				}
			}
			events.clear();
		}
		
		// reserveMemory first:
		events.reserve(atsEvents.size());

		list<ATSEvent*>::iterator it ;
		for( it = atsEvents.begin();it!=atsEvents.end();it++)
		{
			if(*it != NULL)
			{
				events.push_back( createCombinedEvent( *it, ++lastKey) );
			}
			
		}

		//free atsEvents
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Comeing into IATSEventsAPI::FreeATSEventList().");
		IATSEventsAPI::FreeATSEventList(atsEvents);  
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

	}

	ICombinedEventData * ATSEventWrapper::createCombinedEvent ( ATSEvent* atsEvent, unsigned long key)
	{
		try{
			// get the creatime (timeb)
			ATSDateTime atsTime = atsEvent->Timestamp;
			ISCSDateTime iscsTime;
			iscsTime.setByATSDateTime( atsTime);
			timeb createtime;
			createtime.time = iscsTime.getTime_t();
			createtime.millitm = (iscsTime.Nanosecond) / 1000000;
			struct _timeb timebuffer;
			_ftime( &timebuffer );
			createtime.timezone = timebuffer.timezone;
			createtime.dstflag = timebuffer.dstflag;


			// get alarmSeverityName
			char buff[7];
			itoa (atsEvent->AlarmSeverity, buff, 10);
			std::string alarmSeverityName = buff;

			std::string assetName = atsEvent->Asset.c_str();

			std::string description = atsEvent->Description.c_str();

			std::string value = atsEvent->Value.c_str();

			// MMS type:
			std::string alarmMmsType = "";
			ATSEventAlarmMMSType mmsType = atsEvent->AlarmMMSType;
			// 			TYPE_NONE = 0,
			// 				TYPE_SEMI,
			// 				TYPE_AUTO,
			// 				TYPE_JR_SENT,
			// 				TYPE_ALM_SENT,
			// 				TYPE_INHIBIT,
			// 				TYPE_FAILED
			if( ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_AUTO == mmsType)
			{
				alarmMmsType = itoa(TA_Base_Core::IAlarmData::TYPE_AUTO, buff, 10);
			}
			else if( ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_NOALARM == mmsType)
			{
				alarmMmsType = itoa(TA_Base_Core::IAlarmData::TYPE_NONE,  buff, 10); // to be confirmed
			}
			else if( ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_NONE == mmsType)
			{
				alarmMmsType = itoa(TA_Base_Core::IAlarmData::TYPE_INHIBIT, buff, 10); // to be confirmed
			}
			else if (ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_SEMIAUTO == mmsType)
			{
				alarmMmsType = itoa(TA_Base_Core::IAlarmData::TYPE_SEMI, buff, 10);
			}

			// No Dss state for ATS event
			std::string alarmDssState = TA_Base_Core::IAlarmData::NO_ASSOC_PLAN;

			// AVL state:
			std::string alarmAVlState = "";
			if( ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_CHILD == atsEvent->AlarmAvalancheType)
			{
				alarmAVlState = itoa(TA_Base_Core::IAlarmData::CHILD_AVL, buff, 10);
			}else if( ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_HEAD == atsEvent->AlarmAvalancheType)
			{
				alarmAVlState = itoa(TA_Base_Core::IAlarmData::HEAD_AVL, buff, 10);
			}
			else
			{
				alarmAVlState = itoa(TA_Base_Core::IAlarmData::NO_ASSOC_AVL, buff, 10);
			}


			std::string operatorName = atsEvent->Operator;

			std::string alarmComment = "";

			std::string alarmID = atsEvent->AlarmID;

			std::string alarmAck = "";

			std::string alarmStatus = "";

			std::string alarmStatusEventType = "";    //  for column "Alarm Status"
			std::string eventTypeName = atsEvent->EventType;
			if(eventTypeName.empty() == false && 
				(eventTypeName.find("AlarmAcknowledged") != std::string::npos
				|| eventTypeName.find("AlarmClosed") != std::string::npos
				|| eventTypeName.find("AlarmSubmitted") != std::string::npos
				|| eventTypeName.find("AlarmSupressed") != std::string::npos))
			{
				alarmStatusEventType = eventTypeName;
				if(eventTypeName.find("AlarmClosed")!=std::string::npos)
				{
					alarmStatusEventType = "AlarmNormalized";
				}
			}

			std::string operation_mode = "";
			if( ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH == atsEvent->AlarmType )
			{
				operation_mode = "B";
			}
			else if( ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE == atsEvent->AlarmType )
			{
				operation_mode = "M";
			}else if(ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL == atsEvent->AlarmType )
			{
				operation_mode = "O";
			}

			return new CombinedEventData( ECombinedEventType::E_AUDIT_EVENT, key, createtime, alarmSeverityName, assetName, description,
				value, alarmMmsType,alarmDssState, alarmAVlState, operatorName, alarmComment, alarmID, alarmAck, alarmStatus,
				alarmStatusEventType, operation_mode, TA_Base_Core::EventCategory::ATSCategory,atsEvent->EventID);


		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught in createCombinedEvent().");
		}
		

		// create combinedeventData
		// 		CombinedEventData( const ECombinedEventType type, -- NA
		// 			const unsigned long key,-- NA
		// 			const timeb& createTime, --
		// 			const std::string& alarmSeverityName, --
		// 			const std::string& assetName, --
		// 			const std::string& description, --
		// 			const std::string& value, --
		// 			const std::string& alarmMmsState, --
		// 			const std::string& alarmDssState, -- NULL
		// 			const std::string& alarmAvlState, --
		// 			const std::string& operatorName, --
		// 			const std::string& alarmComment, -- NULL
		// 			const std::string& alarmId, --
		// 			const std::string& alarmAck, -- NULL
		// 			const std::string& alarmStatus, -- NULL
		// 			const std::string& alarmStatusEventType, --
		// 			const std::string& operationMode,
		// 			const EventCategory categoryType = EventCategory::ISCSCategory)

	}

	void eventSubscriptionCallback (const ATSSubscriptionError error, std::list<ATSEvent*>& atsEvents){

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Come into function eventSubscriptionCallback(). ");

		if(error == ATS_SUBSCRIPTION_OK){

			try{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get  %d ATS refreshing data.", atsEvents.size());
			std::vector<TA_Base_Core::ICombinedEventData*> events;	

			ATSEventWrapper::transformToISCSEvents ( atsEvents, events, ATSRealTimeWorker::getCurrentMaxKey() );

			ATSRealTimeWorker::setCurrentMaxKey( ATSRealTimeWorker::getCurrentMaxKey() + events.size());

			ATSRealTimeObserverManager::getInstance()->notifyObserverList(ActionCommand::NoCommand, events, ATSRealTimeObserverManager::getInstance()->getTaskID());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught in ATS callback eventSubscriptionCallback().");
			}
			
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSSubscriptionError:%d in ATSEventWrapper::eventSubscriptionCallback.", error);

			//restart ATS subscription
			ATSEventWrapper::restartATSSubscriptionForRefresh();

			
		}
	}

	void ATSEventWrapper::restartATSSubscriptionForRefresh()
	{
		ATSEventWrapper::stopATSSubscription();
	
		ATSEventSubscription* subscriber = getATSSubscriber();

		ATSFilterCriteria atsFilter;
		transformToATSFilter(atsFilter, ATSRealTimeObserverManager::getInstance()->getFilterCriteria());
		std::list<IATSEventsAPI::ATSEvent*> atsEvents;

		if( subscriber!=NULL )
		{
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
			subscriber ->startSubscription( &atsFilter, EventHandlerManager::getMaxATSEvents() ,&TA_Base_App::eventSubscriptionCallback,  atsEvents);
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");

			if (atsEvents.size()>0)
			{
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
				IATSEventsAPI::FreeATSEventList(atsEvents);  
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
			}

		}
	}


	ActionCommand ATSEventWrapper::startATSSubscription(
		FilterCriteria filterCriteria,
		const unsigned int backloglength,				
		std::vector<TA_Base_Core::ICombinedEventData*>& events){


			ActionCommand actCommand = Reconnect;
			ATSFilterCriteria atsFilter;

			transformToATSFilter(atsFilter, filterCriteria );
			std::list<IATSEventsAPI::ATSEvent*> atsEvents;
			
			
				ATSEventSubscription* subscriber = getATSSubscriber();
				if( subscriber!=NULL )
				{
					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
					ATSSubscriptionError atsSubErr = subscriber ->startSubscription( &atsFilter, backloglength ,&TA_Base_App::eventSubscriptionCallback,  atsEvents);
					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");
					
					if(atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_OK)
					{
						// subscribe successfully:
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has started a new subscription successfully, and returned total backlog events: %d ", atsEvents.size());

						// retrieve data
						try{
							transformToISCSEvents ( atsEvents, events );
						}
						catch(...)
						{	
							LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when transfer ATS event to ISCS form.");

						}
						 

						actCommand = NoCommand;
					}
					else 
					{
						// get a communication error
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get Error when call ATS:startSubscription, ATSSubscriptionError:");

						if (atsEvents.size()>0)
						{
							LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
							IATSEventsAPI::FreeATSEventList(atsEvents);  
							LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
						}

						actCommand = ActionCommand::Reconnect;
					}
				
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can't start ATS subscription due to getATSSubscriber unsuccessfully.");
					return ActionCommand::Reconnect;
				}
				

			return actCommand;

	}

	bool ATSEventWrapper::ATSDateTimeCompare(ATSDateTime time1 , ATSDateTime time2)
	{
		bool result = true; // true means time1<time2

		ISCSDateTime t1;
		t1.setByATSDateTime(time1);
		ISCSDateTime t2;
		t2.setByATSDateTime(time2);

		return static_cast<unsigned long>(t1.getTime_t()) < static_cast<unsigned long>(t2.getTime_t());
	}

	// sort -> cut if length beyond ATS max size & free memory
	void ATSEventWrapper::truncateATSEvents (std::list<ATSEvent*>&atsEvents, unsigned long atsMaxLength, bool cutSmallTime)
	{
		if( atsEvents.size()<=atsMaxLength ) 
		{
			return;
		}
		else 
		{
			// cut some events
			unsigned long cutNum = atsEvents.size() - atsMaxLength;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Some ATS events need to be cutted according to max length, cut Number %d", cutNum);
			
			std::list<ATSEvent*> tempList;
	        // the list has already been sorted in order (by timestamp asc)
			ATSEvent * temp=NULL;
			std::list<ATSEvent*>::iterator itr;
			if(cutSmallTime)
			{

				while(cutNum>0)
				{
					temp = atsEvents.front();
					atsEvents.front() = NULL;
					tempList.push_back( temp );

					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
					IATSEventsAPI::FreeATSEventList(tempList);  
					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

					tempList.clear();
					atsEvents.pop_front();
					--cutNum;
				}

			}
			else
			{
				while(cutNum>0)
				{
					temp = atsEvents.back();
					tempList.push_back( temp );
					atsEvents.back() = NULL;

					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
					IATSEventsAPI::FreeATSEventList(tempList);  
					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

					tempList.clear();
					atsEvents.pop_back();
					--cutNum;
				}

			}

		}
	}

	ActionCommand  ATSEventWrapper::doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = ActionCommand::Reconnect;
		ATSQueryError err;
		ATSFilterCriteria atsFilter;
		std::list<ATSEvent*> atsEvents;

		transformToATSFilter(atsFilter, filterCriteria);

		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoStartTimeEndTimeQuery().");
		err = DoStartTimeEndTimeQuery(
			&atsFilter,
			atsFilter.StartTimePeriod,
			atsFilter.EndTimePeriod,
			atsEvents);
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoStartTimeEndTimeQuery().");

		if( err == ATS_QUERY_OK )
		{
			// cut the length to be within atsMaxLength
			truncateATSEvents (atsEvents, atsMaxLength, true);

			//query succesful:
			transformToISCSEvents ( atsEvents, events ); 

			// free the temporary cache 
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			IATSEventsAPI::FreeATSEventList(atsEvents);  
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

			actionCommand =  ActionCommand::NoCommand;

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
			if(atsEvents.size()>0)
			{
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
				IATSEventsAPI::FreeATSEventList(atsEvents);  
				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
			}
			actionCommand = ActionCommand::Reconnect;
		}

		return actionCommand;
		
	}

// 	ActionCommand ATSEventWrapper::doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
// 		FilterCriteria filterCriteria, unsigned long atsMaxLength)
// 	{
// 		ActionCommand actionCommand = ActionCommand::Reconnect;
// 		ATSQueryError err;
// 		ATSFilterCriteria atsFilter;
// 		std::list<ATSEvent*> atsEvents1;
// 		std::list<ATSEvent*> atsEvents2;
// 
// 
// 		transformToATSFilter(atsFilter, filterCriteria);
// 
// 		//query [start, end)
// 		err = DoStartTimeEndTimeQuery(
// 			&atsFilter,
// 			atsFilter.StartTimePeriod,
// 			atsFilter.EndTimePeriod,
// 			atsEvents1);
// 
// 		if( err == ATS_QUERY_OK )
// 		{
// 			// go on query [end, end+1s)
// 			atsFilter.StartTimePeriod = atsFilter.EndTimePeriod;
// 			ISCSDateTime tempTime;
// 			tempTime.setByATSDateTime(atsFilter.StartTimePeriod);
// 			tempTime.addSec(1);
// 			atsFilter.EndTimePeriod = ATSDateTime(tempTime.Year, tempTime.Month, tempTime.Day, tempTime.Hour, tempTime.Minute, tempTime.Second, tempTime.Nanosecond);
// 
// 			err = DoStartTimeEndTimeQuery(
// 				&atsFilter,
// 				atsFilter.StartTimePeriod,
// 				atsFilter.EndTimePeriod,
// 				atsEvents2);
// 
// 			cutIfIDEqualOrBiggerThan ( filterCriteria.pkeyBoundary_ATS, atsEvents2 );
// 
// 			// atsEvents1 = atsEvents1 + atsEvents2
// 			mergeATSEvnts(atsEvents1, atsEvents2);
// 
// 			// cut the length to be within atsMaxLength
// 			truncateATSEvents (atsEvents1, atsMaxLength, true);
// 
// 			//query succesful:
// 			transformToISCSEvents ( atsEvents1, events ); 
// 
// 			// free the temporary cache 
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 			IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 
// 			actionCommand =  ActionCommand::NoCommand;
// 
// 		}
// 		else
// 		{
// 			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
// 			if(atsEvents1.size()>0)
// 			{
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 				IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 			}
// 
// 			actionCommand = ActionCommand::Reconnect;
// 		}
// 
// 		return actionCommand;
// 
// 	}


	ActionCommand ATSEventWrapper::doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = ActionCommand::Reconnect;
		ATSQueryError err;
		ATSFilterCriteria atsFilter;
		std::list<ATSEvent*> atsEvents;

		transformToATSFilter(atsFilter, filterCriteria);

		//query [start, end)
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoPreviousPageQuery().");
		err = IATSEventsAPI::DoPreviousPageQuery(&atsFilter, filterCriteria.pkeyBoundary_ATS, atsMaxLength, atsEvents);
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoPreviousPageQuery().");

		if( err == ATS_QUERY_OK )
		{
			//query succesful:
			transformToISCSEvents ( atsEvents, events ); 

			actionCommand =  ActionCommand::NoCommand;

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);

			actionCommand = ActionCommand::Reconnect;
		}

		if(atsEvents.size()>0)
		{
			// free the temporary cache 
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			IATSEventsAPI::FreeATSEventList(atsEvents);  
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
		}

		return actionCommand;

	}


	bool ATSEventWrapper::ATSFilterTimeSpanLessThan1Sec(ATSFilterCriteria atsFilter)
	{

		ISCSDateTime startTimeAdd1Sec;
		startTimeAdd1Sec.setByATSDateTime(atsFilter.StartTimePeriod);
		startTimeAdd1Sec.addSec(1);

		ISCSDateTime endTimeTemp;
		endTimeTemp.setByATSDateTime(atsFilter.EndTimePeriod);

		if(startTimeAdd1Sec.getTime_t()> endTimeTemp.getTime_t())
		{
			return true;
		}
		else
		{   return false;
		}
	}

	void ATSEventWrapper::add1SecToATSFilterStartTime(ATSFilterCriteria & atsFilter)
	{
		ISCSDateTime tempTime;
		tempTime.setByATSDateTime(atsFilter.StartTimePeriod);
		tempTime.addSec(1);

		atsFilter.StartTimePeriod = ATSDateTime(tempTime.Year, tempTime.Month, tempTime.Day, 
			tempTime.Hour, tempTime.Minute, tempTime.Second, tempTime.Nanosecond);

	}

// 	ActionCommand ATSEventWrapper::doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
// 		FilterCriteria filterCriteria, unsigned long atsMaxLength)
// 	{
// 		ATSQueryError err;
// 		ATSFilterCriteria atsFilter;
// 		std::list<ATSEvent*> atsEvents1;
// 		std::list<ATSEvent*> atsEvents2;
// 
// 		transformToATSFilter(atsFilter, filterCriteria);
// 
// 		if( ATSFilterTimeSpanLessThan1Sec(atsFilter))
// 		{
// 			// if the timespan is <1sec, query [start, end)
// 			err = DoStartTimeEndTimeQuery(
// 				&atsFilter,
// 				atsFilter.StartTimePeriod,
// 				atsFilter.EndTimePeriod,
// 				atsEvents1);
// 			if(err!=ATS_QUERY_OK)
// 			{
// 				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
// 				if(atsEvents1.size()>0)
// 				{
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 					IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 				}
// 				return  ActionCommand::Reconnect;
// 			}
// 
// 		}
// 		else
// 		{   // if the timespan is >=1sec, startTime= startTime+1sec, query [start+1s, end);
// 			add1SecToATSFilterStartTime(atsFilter);
// 
// 			err = DoStartTimeEndTimeQuery(
// 				&atsFilter,
// 				atsFilter.StartTimePeriod,
// 				atsFilter.EndTimePeriod,
// 				atsEvents1);
// 
// 			if(err != ATS_QUERY_OK)
// 			{
// 				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
// 				if(atsEvents1.size()>0)
// 				{
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 					IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 				}
// 				return  ActionCommand::Reconnect;
// 			}
//             
// 			// then, query [ startTime, starTime+1s):
// 			atsFilter.EndTimePeriod = atsFilter.StartTimePeriod;
// 			atsFilter.StartTimePeriod = ATSDateTime( filterCriteria.StartTimePeriod.Year, filterCriteria.StartTimePeriod.Month, filterCriteria.StartTimePeriod.Day,
// 				filterCriteria.StartTimePeriod.Hour, filterCriteria.StartTimePeriod.Minute, filterCriteria.StartTimePeriod.Second, filterCriteria.StartTimePeriod.Nanosecond);
// 
// 			// [start, start+1s);
// 			err = DoStartTimeEndTimeQuery(
// 				&atsFilter,
// 				atsFilter.StartTimePeriod,
// 				atsFilter.EndTimePeriod,
// 				atsEvents2);
// 			if(err!=ATS_QUERY_OK)
// 			{
// 				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
// 				if(atsEvents1.size()>0)
// 				{
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 					IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 					LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 				}
// 				return  ActionCommand::Reconnect;
// 			}
// 
// 			cutIfIDEqualOrSmallerThan ( filterCriteria.pkeyBoundary_ATS, atsEvents2 );
// 			// atsEvents1 = atsEvents2 + atsEvents1
// 			mergeATSEvnts(atsEvents1, atsEvents2);
// 
// 		}
// 		// cut the length to be within atsMaxLength
// 		truncateATSEvents (atsEvents1, atsMaxLength, true);
// 
// 		//query succesful:
// 		transformToISCSEvents ( atsEvents1, events ); 
// 
// 		// free the temporary cache 
// 		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 		IATSEventsAPI::FreeATSEventList(atsEvents1);  
// 		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 
// 		return  ActionCommand::NoCommand;
// 		
// 	}



	ActionCommand ATSEventWrapper::doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = ActionCommand::Reconnect;
		ATSQueryError err;
		ATSFilterCriteria atsFilter;
		std::list<ATSEvent*> atsEvents;

		transformToATSFilter(atsFilter, filterCriteria);

		//query [start, end)
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoForwardPageQuery().");
		err = IATSEventsAPI::DoForwardPageQuery(&atsFilter, filterCriteria.pkeyBoundary_ATS, atsMaxLength, atsEvents);
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoForwardPageQuery().");

		if( err == ATS_QUERY_OK )
		{
			//query succesful:
			transformToISCSEvents ( atsEvents, events ); 

			actionCommand =  ActionCommand::NoCommand;

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);

			actionCommand = ActionCommand::Reconnect;
		}

		if(atsEvents.size()>0)
		{
			// free the temporary cache 
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			IATSEventsAPI::FreeATSEventList(atsEvents);  
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
		}

		return actionCommand;

	}



// 	// no need?
// 	ActionCommand ATSEventWrapper::doEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
// 		FilterCriteria filterCriteria, unsigned long atsMaxLength)
// 	{
// 		ActionCommand actionCommand = ActionCommand::Reconnect;
// 		ATSQueryError err;
// 		ATSFilterCriteria atsFilter;
// 		std::list<ATSEvent*> atsEvents;
// 
// 		transformToATSFilter(atsFilter, filterCriteria);
// 
// 		err = DoPreviousPageQuery(
// 			&atsFilter,
// 			atsFilter.EndTimePeriod,
// 			atsMaxLength,
// 			atsEvents);
// 
// 		if( err == ATS_QUERY_OK )
// 		{
// 			//query succesful:
// 			transformToISCSEvents ( atsEvents, events ); 
// 
// 			// free the temporary cache 
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 			IATSEventsAPI::FreeATSEventList(atsEvents);  
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 
// 			actionCommand =  ActionCommand::NoCommand;
// 
// 		}
// 		else
// 		{
// 			if(atsEvents.size()>0)
// 			{
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 				IATSEventsAPI::FreeATSEventList(atsEvents);  
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 			}
// 
// 			actionCommand = ActionCommand::Reconnect;
// 		}
// 
// 		return actionCommand;
// 	}
// 
// 	// no need?
// 	ActionCommand ATSEventWrapper::doStartTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
// 		FilterCriteria filterCriteria, unsigned long atsMaxLength)
// 	{
// 		ActionCommand actionCommand = ActionCommand::Reconnect;
// 		ATSQueryError err;
// 		ATSFilterCriteria atsFilter;
// 		std::list<ATSEvent*> atsEvents;
// 
// 		transformToATSFilter(atsFilter, filterCriteria);
// 
// 		err = DoForwardPageQuery(
// 			&atsFilter,
// 			atsFilter.StartTimePeriod,
// 			atsMaxLength,
// 			atsEvents);
// 
// 		if( err == ATS_QUERY_OK )
// 		{
// 			//query succesful:
// 			transformToISCSEvents ( atsEvents, events ); 
// 
// 			// free the temporary cache 
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 			IATSEventsAPI::FreeATSEventList(atsEvents);  
// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 
// 			actionCommand =  ActionCommand::NoCommand;
// 
// 		}
// 		else
// 		{
// 			if(atsEvents.size()>0)
// 			{
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 				IATSEventsAPI::FreeATSEventList(atsEvents);  
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 			}
// 			actionCommand = ActionCommand::Reconnect;
// 		}
// 
// 		return actionCommand;
// 	}

	ActionCommand ATSEventWrapper::stopATSSubscription(void){

		ActionCommand actCommand = NoCommand;

		ATSEventSubscription* subscriber = getATSSubscriber();

		if(  subscriber != NULL )
		{
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::stopSubscription().");
			ATSSubscriptionError atsSubErr= subscriber ->stopSubscription();
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::stopSubscription().");

			if(atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_OK){
				actCommand = ActionCommand::NoCommand;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ATS Subscription stopped successfully ");
			}else if(atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_FAILED_COMMS_ERROR){
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSSubscriptionError: %d when try to stopSubscription ", atsSubErr);

			}
		}			

		return actCommand;
	}

	void  ATSEventWrapper:: setTMTypeToATSFilter(ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria){

		atsfilter.AlarmTypeEqualOr.clear();

		if(filterCriteria.TMTypeEqualOr.size()>0){

			string temp = filterCriteria.TMTypeEqualOr.front().c_str(); 
			if ( "O"== temp ){
				atsfilter.AlarmTypeEqualOr.push_back(ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL);
			}else if( "M"== temp ){
				atsfilter.AlarmTypeEqualOr.push_back(ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE);
			}else if( "B"== temp ){
				atsfilter.AlarmTypeEqualOr.push_back(ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH);
			}
						
		}

		return;
	}

	void  ATSEventWrapper:: setStartEndTimeToATSFilter (ATSFilterCriteria & atsfilter, FilterCriteria & filterCriteria){
		ISCSDateTime tempTime = filterCriteria.StartTimePeriod;
		atsfilter.StartTimePeriod.Year = tempTime.Year;
		atsfilter.StartTimePeriod.Month = tempTime.Month;
		atsfilter.StartTimePeriod.Day = tempTime.Day;
		atsfilter.StartTimePeriod.Hour = tempTime.Hour;
		atsfilter.StartTimePeriod.Minute = tempTime.Minute;
		atsfilter.StartTimePeriod.Second = tempTime.Second;

		tempTime = filterCriteria.EndTimePeriod;
		atsfilter.EndTimePeriod.Year = tempTime.Year;
		atsfilter.EndTimePeriod.Month = tempTime.Month;
		atsfilter.EndTimePeriod.Day = tempTime.Day;
		atsfilter.EndTimePeriod.Hour = tempTime.Hour;
		atsfilter.EndTimePeriod.Minute = tempTime.Minute;
		atsfilter.EndTimePeriod.Second = tempTime.Second;

	}

	// if pkey can be found, cut the record at it and before it
	// if pkey can't be found, do nothing
	void ATSEventWrapper:: cutIfIDEqualOrSmallerThan ( std::string pkeyBoundary, std::list<ATSEvent*> & atsEvents )
	{
		if(atsEvents.size()==0) return;
		std::list<ATSEvent*>::iterator itr = atsEvents.begin();
       /* std::list<ATSEvent*>::iterator foundPkey_itr = atsEvents.end();*/
		int reach_pos = 0;
		for ( ; itr!= atsEvents.end(); itr++)
		{
			if( (*itr)->EventID == pkeyBoundary)  // when it's the lase one: search_pos = size-1
			{
				break;
			}
			reach_pos ++;
		}

		if(reach_pos != atsEvents.size())  // has found the key
		{
			std::list<ATSEvent*> toBeDelPart;
			int num=1;
			ATSEvent * temp = NULL;
			while ( num <= reach_pos+1)
			{
				itr = atsEvents.begin();
				temp = *itr;
				toBeDelPart.push_back(temp);
				atsEvents.front() = NULL;
				atsEvents.pop_front();
				num++;
			}

			
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			FreeATSEventList(toBeDelPart);
			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
		}

	}

	// if pkey can be found, cut the record at it and before it
	// if pkey can't be found, do nothing
	void ATSEventWrapper:: cutIfIDEqualOrBiggerThan ( std::string pkeyBoundary, std::list<ATSEvent*> & atsEvents )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug , "cutIfIDEqualOrBiggerThan() come in.");

		if(atsEvents.size()==0) return;

		std::list<ATSEvent*>::iterator itr = atsEvents.end();
		std::list<ATSEvent*>::iterator foundPkey_itr = atsEvents.begin();
		for ( --itr; itr!= atsEvents.begin(); itr--)
		{
			if( (*itr)->EventID == pkeyBoundary)
			{
				foundPkey_itr = itr;
				break;
			}
		}
	
		if (foundPkey_itr == atsEvents.begin() && atsEvents.front()->EventID== pkeyBoundary)
		{
			foundPkey_itr = atsEvents.begin();  // found pkey at the beginning position
		}else if( foundPkey_itr == atsEvents.begin())
		{
			return;    //  pkey not found
		}

		// pkey found
// 			std::list<ATSEvent*> toBeDelPart;
// 			itr = atsEvents.end();
// 			itr --;
// 			ATSEvent * temp=NULL;
// 			while ( itr!= foundPkey_itr)
// 			{
// 				temp = *itr;
// 				toBeDelPart.push_back( temp );
// 				atsEvents.back() = NULL;
// 
// 				// free the memory of the cutted part:
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
// 				FreeATSEventList(toBeDelPart);
// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
// 
// 				toBeDelPart.clear();
// 
// 				atsEvents.pop_back();
// 				itr = atsEvents.end();
// 				itr --;
// 			}


		std::list<ATSEvent*> toBeDelPart;
		toBeDelPart.assign(foundPkey_itr,atsEvents.end());

		for(itr = foundPkey_itr; itr!=atsEvents.end(); itr++)
		{
			*itr = NULL;
		}

		// free the memory of the cutted part:
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
		FreeATSEventList(toBeDelPart);
		LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

		atsEvents.erase(foundPkey_itr, atsEvents.end());




// 		for(itr = foundPkey_itr; itr!=atsEvents.end(); itr++)
// 		{
// 			delete *itr;
// 			*itr = NULL;
// 		}
// 
// 		atsEvents.erase(foundPkey_itr, atsEvents.end());
// 
// 		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug , "cutIfIDEqualOrBiggerThan() come out.");

	}

	// put the 2nd list after the 1st if the timestamp is bigger, in the same order as it was in the 2nd.
	// or put the 2nd list before the 1st if the timestamp is smaller, in the same order as it was in the 2nd.
	void ATSEventWrapper::mergeATSEvnts(std::list<ATSEvent*> & atsEvents1, std::list<ATSEvent*> & atsEvents2)
	{
		if (atsEvents1.size()>0 && atsEvents2.size()>0)
		{
			ATSDateTime time1 = atsEvents1.front()->Timestamp;
			ATSDateTime time2 = atsEvents2.front()->Timestamp;
			if( ATSDateTimeCompare(time1, time2) )  // time1<time2
			{
				// link atsEvent2 at the back of atsEvents1
				for ( std::list<ATSEvent*>::iterator itr = atsEvents2.begin(); itr!= atsEvents2.end(); itr++)
				{
					atsEvents1.push_back(*itr);
				}
			}
			else 
			{
				std::list<ATSEvent*>::iterator itr = atsEvents2.end();
				itr--;
				// link atsEvent2 at the back of atsEvents1
				for ( ; itr!= atsEvents2.begin(); itr--)
				{
					atsEvents1.push_front(*itr);
				}
				atsEvents1.push_front(atsEvents2.front());
			}
			atsEvents2.clear();
		}
		else if( atsEvents1.size()==0 )
		{
			atsEvents1 = atsEvents2;
		}
	}
};
