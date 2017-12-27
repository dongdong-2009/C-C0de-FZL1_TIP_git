#include "ComSimulator.h"

#include "ATSRealTimeObserverManager.h"
#include "EventHandlerManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include <comutil.h>
#include "ConstantItems.h"
#include "ATSComWrapper.h"

namespace TA_Base_App
{
	ComSimulator* ComSimulator::m_instance = NULL;

	ComSimulator * ComSimulator::getInstance()
	{
		if(m_instance == NULL)
		{
			m_instance = new ComSimulator();
		}
		return m_instance;
	}
	void ComSimulator::freeInstance()
	{
		if(m_instance!=NULL)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	ComSimulator::ComSimulator(void)
	{
		m_ATS_func_debug_level = true;
		m_pSubscriber = NULL;
		m_pCP = NULL;
	}

	ComSimulator::~ComSimulator(void)
	{
		if(m_pCP!=NULL)
		{
			m_pCP->Unadvise(m_dwAdvise);
		}

	}

	void ComSimulator::initATSSubscriber()
	{
		if(m_pSubscriber != NULL)
		{
			return;
		}
		m_pSubscriber.CoCreateInstance(CLSID_ATSEventSubscription, NULL, CLSCTX_INPROC_SERVER);


		CComPtr<IConnectionPointContainer> pCPC;

		HRESULT hr;

		hr = m_pSubscriber->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
		if(FAILED(hr))
		{
			m_pSubscriber = NULL;
			return;
		}

		hr = pCPC->FindConnectionPoint(DIID__IATSEventSubscriptionEvents,&m_pCP);

		if(FAILED(hr))
		{
			/*MessageBox(L"Unable to query IConnectionPoint", L"DBG");*/
			m_pSubscriber = NULL;
			return;
		}

		IUnknown *pSinkUnk;
		CMySink *pSink;
		pSink = new CMySink();

		pSinkUnk = pSink->GetInterface(&IID_IUnknown);

		hr = m_pCP->Advise(pSinkUnk,&m_dwAdvise);

	}

	ActionCommand ComSimulator::startATSSubscription(
		FilterCriteria filterCriteria,
		const unsigned int backloglength,				
		std::vector<TA_Base_Core::ICombinedEventData*>& events){


			ActionCommand actCommand = Reconnect;

			CComPtr<IATSFilterCriteria> pATSFilter;
			pATSFilter.CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER);

			transformToATSFilter(pATSFilter, filterCriteria );
			CComPtr<IATSEventList> pATSEvents;

			initATSSubscriber();

			if( m_pSubscriber != NULL )
			{
				LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");

				long atsSubErr = m_pSubscriber ->startSubscription( pATSFilter, backloglength ,& pATSEvents);

				LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");

				if(atsSubErr == 0 /*ATSSubscriptionError::ATS_SUBSCRIPTION_OK*/)
				{
					// subscribe successfully:
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has started a new subscription successfully, and returned total backlog events: %d ", pATSEvents->GetSize());

					// retrieve data
					try{
						transformToISCSEvents ( pATSEvents, events );
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

					// 					if (pATSEvents->GetSize()>0)
					// 					{
					// 						LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
					// 						//IATSEventsAPI::FreeATSEventList(atsEvents);  
					// 						LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
					// 					}

					actCommand = TA_Base_App::Reconnect;
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can't start ATS subscription due to getATSSubscriber unsuccessfully.");
				return TA_Base_App::Reconnect;
			}
			
			return actCommand;

	}
	void ComSimulator::setSystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList;
		pATSFilter->GetSystemNameEqualOr(&pTempStrList);
		pTempStrList->Clear();
		for(list<std::string>::iterator itr =filterCriteria.SystemNameEqualOr.begin();
			itr!=filterCriteria.SystemNameEqualOr.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str() );
			pTempStrList->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	void ComSimulator::setSubsystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList;
		pATSFilter->GetSubsystemNameEqualOr(&pTempStrList);
		pTempStrList->Clear();
		for(list<std::string>::iterator itr =filterCriteria.SubsystemNameEqualOr.begin();
			itr!=filterCriteria.SubsystemNameEqualOr.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str());
			pTempStrList->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	void ComSimulator::setOperatorToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList4;
		pATSFilter->GetOperatorEqualOr(&pTempStrList4);
		pTempStrList4->Clear();
		for(list<std::string>::iterator itr =filterCriteria.OperatorEqualOr.begin();
			itr!=filterCriteria.OperatorEqualOr.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str() );
			pTempStrList4->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	void ComSimulator::setLocationToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList3;
		pATSFilter->GetLocationEqualOr(&pTempStrList3);
		pTempStrList3->Clear();
		for(list<std::string>::iterator itr =filterCriteria.LocationEqualOr.begin();
			itr!=filterCriteria.LocationEqualOr.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str());
			pTempStrList3->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	void ComSimulator::setAssetToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList5;
		pATSFilter->GetAssetEqualOr(&pTempStrList5);
		pTempStrList5->Clear();
		for(list<std::string>::iterator itr =filterCriteria.Asset.begin();
			itr!=filterCriteria.Asset.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str());
			pTempStrList5->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	void ComSimulator::setAlarmSeverityToATSFilter(IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSIntList> pTempIntList;
		pATSFilter->GetAlarmSeverityEqualOr(&pTempIntList);
		pTempIntList->Clear();
		for(vector<int>::iterator itr2 =filterCriteria.severityIds.begin();
			itr2!=filterCriteria.severityIds.end(); itr2++)
		{
			pTempIntList->Add(*itr2);
		}
	}
	void ComSimulator::transformToATSFilter(IATSFilterCriteria * pATSFilter, FilterCriteria & filterCriteria )
	{
		pATSFilter->StartTimePeriodDefined = filterCriteria.StartTimePeriodDefined;
		pATSFilter->EndTimePeriodDefined = filterCriteria.EndTimePeriodDefined;

		setStartEndTimeToATSFilter (pATSFilter,filterCriteria);

		// adjust the star/end time for these situations:
		//adjustATSFilterTime(pATSFilter);

		//system , subsystem, location, operator, asset
		setSystemNameToATSFilter (pATSFilter,filterCriteria);

		setSubsystemNameToATSFilter(pATSFilter,filterCriteria);

		setLocationToATSFilter(pATSFilter,filterCriteria);

		setOperatorToATSFilter(pATSFilter,filterCriteria);

		setAssetToATSFilter(pATSFilter,filterCriteria);

		setTMTypeToATSFilter(pATSFilter,filterCriteria);

		setAlarmSeverityToATSFilter(pATSFilter,filterCriteria);

	}

	void  ComSimulator::setStartEndTimeToATSFilter ( ATSBridgeUtilLib::IATSFilterCriteria * pATSfilter, 
		FilterCriteria & filterCriteria)
	{

		if(pATSfilter->StartTimePeriodDefined == VARIANT_TRUE)
		{
			ISCSDateTime tempTime = filterCriteria.StartTimePeriod;
			CComPtr<IATSDateTime> pStartTime;
		    /*IATSDateTime* pStartTime = NULL;*/

			pATSfilter->GetStartTimePeriod( &pStartTime );
			pStartTime->SetDate(tempTime.Year, tempTime.Month, tempTime.Day, tempTime.Hour,tempTime.Minute, tempTime.Second, 0);
		}

		if(pATSfilter->EndTimePeriodDefined == VARIANT_TRUE)
		{
			ISCSDateTime tempTime = filterCriteria.EndTimePeriod;
			/*IATSDateTime* pEndTime = NULL;*/
			CComPtr<IATSDateTime> pEndTime;
			pATSfilter->GetEndTimePeriod( &pEndTime );
			pEndTime->SetDate(tempTime.Year, tempTime.Month, tempTime.Day, tempTime.Hour,tempTime.Minute, tempTime.Second, 0);
		}

	}

// 	void ComSimulator::adjustATSFilterTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter)
// 	{
// 
// 		const CTime currentTime = CTime::GetCurrentTime();
// 
// 		if(atsfilter ->StartTimePeriodDefined == false && atsfilter->EndTimePeriodDefined == false)
// 		{   // this is real time mode: [current-1day, current]
// 
// 			CTime startTime(currentTime.GetTime()-EventViewerConstants::TWENTY_FOUR_HOURS);
// 
// 			setATSFilterStartTimeByCTime(atsfilter, startTime);
// 
// 			setATSFilterEndTimeByCTime(atsfilter, currentTime);
// 
// 		}
// 		else if(atsfilter->StartTimePeriodDefined == true && atsfilter->EndTimePeriodDefined == false)
// 		{
// 			// adjust end time to current time or starttime+1day , which comes first
// 
// 			CComPtr<IATSDateTime> pStartTime;
// 			atsfilter->GetStartTimePeriod(&pStartTime);
// 			ISCSDateTime fromTime_ISCS(pStartTime->Year,pStartTime->Month, pStartTime->Day, 
// 				pStartTime->Hour, pStartTime->Minute,pStartTime->Second,pStartTime->Nanosecond);
// 			if (fromTime_ISCS.getTime_t() + EventViewerConstants::TWENTY_FOUR_HOURS < currentTime.GetTime() )
// 			{
// 				CTime fromTimeAdd1Day( fromTime_ISCS.getTime_t() + EventViewerConstants::TWENTY_FOUR_HOURS );
// 				setATSFilterEndTimeByCTime(atsfilter, fromTimeAdd1Day);
// 			}else
// 			{
// 				setATSFilterEndTimeByCTime(atsfilter, currentTime);
// 			}
// 			atsfilter->EndTimePeriodDefined = true;
// 
// 		}
// 		else if(atsfilter->StartTimePeriodDefined == false && atsfilter->EndTimePeriodDefined == true) 
// 		{
// 			// adjust start time as endtime- 1day
// 			CComPtr<IATSDateTime> pEndTime;
// 			atsfilter->GetEndTimePeriod(&pEndTime);
// 			ISCSDateTime endTime_ISCS(pEndTime->Year,pEndTime->Month, pEndTime->Day, 
// 				pEndTime->Hour, pEndTime->Minute,pEndTime->Second,pEndTime->Nanosecond);
// 
// 			CTime startTime( endTime_ISCS.getTime_t() - EventViewerConstants::TWENTY_FOUR_HOURS);
// 			setATSFilterStartTimeByCTime(atsfilter, startTime);
// 			atsfilter->StartTimePeriodDefined = true;
// 		}
// 	}

	void ComSimulator::setATSFilterStartTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time)
	{
		/*IATSDateTime* pStartTime = NULL;*/
		CComPtr<IATSDateTime> pStartTime;
		atsfilter->GetStartTimePeriod( &pStartTime );
		pStartTime->SetDate(time.GetYear(), time.GetMonth(), time.GetDay(), 
			time.GetHour(), time.GetMinute(), time.GetSecond(),0);

	}

	void ComSimulator::setATSFilterEndTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time)
	{
		/*IATSDateTime* pEndTime = NULL;*/
		CComPtr<IATSDateTime> pEndTime;
		atsfilter->GetEndTimePeriod( &pEndTime );
		pEndTime->SetDate(time.GetYear(), time.GetMonth(), time.GetDay(), 
			time.GetHour(), time.GetMinute(), time.GetSecond(),0);
	}

	void  ComSimulator:: setTMTypeToATSFilter(ATSBridgeUtilLib::IATSFilterCriteria * atsfilter, FilterCriteria & filterCriteria)
	{
		CComPtr<IATSIntList> pAlarmType;
		atsfilter->GetOperationalMaintenanceTypeEqualOr( &pAlarmType );
		pAlarmType->Clear();

		if(filterCriteria.TMTypeEqualOr.size()>0){

			string temp = filterCriteria.TMTypeEqualOr.front().c_str(); 
			if ( "O"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL);*/
				pAlarmType->Add(0);
			}else if( "M"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(1/ *ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE* /);*/
				pAlarmType->Add(1);
			}else if( "B"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(2/ *ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH* /);*/
				pAlarmType->Add(2);
			}
		}

		return;
	}

	void ComSimulator::transformToISCSEvents ( IATSEventList * pATSEvents, 
		std::vector<TA_Base_Core::ICombinedEventData*>& events , unsigned long lastKey)
	{
		if (pATSEvents->GetSize()==0) return;

		//if the destination is not empty, clean it first
		if (events.size() > 0)
		{
			for (int i=0; i< (int)events.size(); i++)
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
		events.reserve(pATSEvents->GetSize());

		for (int i=0; i<pATSEvents->GetSize(); i++)
		{
			CComPtr<IATSEvent> pATSEvent = NULL;
			pATSEvents->GetItem(i, &pATSEvent);
			if(pATSEvent!=NULL)
			{
				events.push_back( createCombinedEvent( pATSEvent, ++lastKey) );
			}

		}


		// 		//free atsEvents
		// 		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Comeing into IATSEventsAPI::FreeATSEventList().");
		// 	/*	IATSEventsAPI::FreeATSEventList(pATSEvents); */ 
		// 		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

	}

	ICombinedEventData * ComSimulator::createCombinedEvent ( IATSEvent* pATSEvent, unsigned long key)
	{
		try{
			// get the creatime (timeb)
			CComPtr<IATSDateTime> pDateTime;
			pATSEvent->GetTimeStamp(&pDateTime);
			ISCSDateTime iscsTime(pDateTime->Year,pDateTime->Month, pDateTime->Day, 
				pDateTime->Hour, pDateTime->Minute, pDateTime->Second, pDateTime->Nanosecond);
			timeb createtime;
			createtime.time = iscsTime.getTime_t();
			createtime.millitm = (unsigned short)(iscsTime.Nanosecond) / 1000000;
			struct _timeb timebuffer;
			_ftime( &timebuffer );
			createtime.timezone = timebuffer.timezone;
			createtime.dstflag = timebuffer.dstflag;


			// get alarmSeverityName
			char buff[7];
			//itoa (pATSEvent->AlarmSeverity, buff, 10);
			_itoa_s(pATSEvent->AlarmSeverity, buff, 10);
			std::string alarmSeverityName = buff;

			BSTR bstrAsset = pATSEvent->Asset;
			char* assetName = _com_util::ConvertBSTRToString(bstrAsset);
			SysFreeString(bstrAsset);

			BSTR bstrDesc = pATSEvent->Description;
			char* description = _com_util::ConvertBSTRToString(bstrDesc);
			SysFreeString(bstrDesc);

			BSTR bstrVal = pATSEvent->Value;
			char* value = _com_util::ConvertBSTRToString(bstrVal);
			SysFreeString(bstrVal);


			//MMS State
			std::string alarmMmsState = "";
			short mmsState = pATSEvent->AlarmMMSState;
			switch(mmsState)
			{

			case 1: //ATS_EVENTALARMMMSSTATE_SEMI
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_SEMI, buff, 10); // "S";
				alarmMmsState = _itoa_s(TA_Base_Core::IAlarmData::TYPE_SEMI, buff, 10); // "S";
				break;
			case 2: //ATS_EVENTALARMMMSSTATE_AUTO
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_AUTO, buff, 10); // "A";
				alarmMmsState = _itoa_s(TA_Base_Core::IAlarmData::TYPE_AUTO, buff, 10); // "A";
				break;
			case 3: //ATS_EVENTALARMMMSSTATE_JR_SENT
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_JR_SENT, buff, 10);; //"R";
				alarmMmsState = _itoa_s(TA_Base_Core::IAlarmData::TYPE_JR_SENT, buff, 10);; //"R";
				break;
			case 5: //ATS_EVENTALARMMMSSTATE_INHIBIT
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_INHIBIT, buff, 10);; //"I";
				alarmMmsState = _itoa_s(TA_Base_Core::IAlarmData::TYPE_INHIBIT, buff, 10);; //"I";
				break;
			default:
				alarmMmsState = "";
			}

			// No Dss state for ATS event
			std::string alarmDssState = TA_Base_Core::IAlarmData::NO_ASSOC_PLAN;

			// AVL state:
			std::string alarmAVlState = "";
			if( /*ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_CHILD*/ 1 == pATSEvent->AlarmAvalancheType)
			{
				//alarmAVlState = itoa(TA_Base_Core::IAlarmData::CHILD_AVL, buff, 10);
				alarmAVlState = _itoa_s(TA_Base_Core::IAlarmData::CHILD_AVL, buff, 10);
			}else if(/* ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_HEAD*/ 0 == pATSEvent->AlarmAvalancheType)
			{
				//alarmAVlState = itoa(TA_Base_Core::IAlarmData::HEAD_AVL, buff, 10);
				alarmAVlState = _itoa_s(TA_Base_Core::IAlarmData::HEAD_AVL, buff, 10);
			}
			else
			{
				//alarmAVlState = itoa(TA_Base_Core::IAlarmData::NO_ASSOC_AVL, buff, 10);
				alarmAVlState = _itoa_s(TA_Base_Core::IAlarmData::NO_ASSOC_AVL, buff, 10);
			}


			BSTR bstrOperator = pATSEvent->Operator;
			char* operatorName = _com_util::ConvertBSTRToString(bstrOperator);
			SysFreeString(bstrOperator);

			std::string alarmComment = "";

			BSTR bstrAlarmID = pATSEvent->AlarmID;
			char* alarmID = _com_util::ConvertBSTRToString(bstrAlarmID);
			SysFreeString(bstrAlarmID);

			std::string alarmAck = "";

			std::string alarmStatus = "";

			std::string alarmStateStr = "";    //  for column "Alarm State"

			short alarmState = pATSEvent->AlarmState;
			switch (alarmState)
			{
			case 0:  //ATS_EVENTALARMSTATE_CLOSED
				alarmStateStr = ALARM_CLOSED;  
				break;
			case 1:  //ATS_EVENTALARMSTATE_OPEN
				alarmStateStr = ALARM_OPENED;
				break;
			case 2:  //ATS_EVENTALARMSTATE_RETURNED
				alarmStateStr = ALARM_NORMALISED;
				break;
			case 3:  //ATS_EVENTALARMSTATE_ACKNOWLEDGED
				alarmStateStr = ALARM_ACKNOWLEDGED;
				break;
			case 4:  //ATS_EVENTALARMSTATE_INHIBITED
				alarmStateStr = ALARM_SUPPRESSED;
				break;
			case 5:  //ATS_EVENTALARMSTATE_NOALARM
				alarmStateStr = "";
				break;
			default:
				alarmStateStr = "";
			}


			std::string operation_mode = "";
			if( /*ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH*/ 2 == pATSEvent->OperationalMaintenanceType )
			{
				operation_mode = "B";
			}
			else if(/* ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE*/ 1 == pATSEvent->OperationalMaintenanceType )
			{
				operation_mode = "M";
			}else if(/*ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL*/ 0 == pATSEvent->OperationalMaintenanceType )
			{
				operation_mode = "O";
			}


			BSTR bstrEventID = pATSEvent->EventID;
			char* eventID = _com_util::ConvertBSTRToString(bstrEventID);
			SysFreeString(bstrEventID);

			// to do: change CombinedEventData to CombinedEventItem
			return new CombinedEventData( TA_Base_Core::E_AUDIT_EVENT, key, createtime, alarmSeverityName, assetName, description,
				value, alarmMmsState,alarmDssState, alarmAVlState, operatorName, alarmComment, alarmID, alarmAck, alarmStatus,
				alarmStateStr, operation_mode, TA_Base_Core::ATSCategory,eventID);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught in createCombinedEvent().");
		}

		return NULL;

	}

	ActionCommand ComSimulator::stopATSSubscription(void){

		ActionCommand actCommand = NoCommand;

		if(  m_pSubscriber != NULL )
		{
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::stopSubscription().");
			long atsSubErr = m_pSubscriber->stopSubscription();
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::stopSubscription().");

			if(atsSubErr == 0 /*ATSSubscriptionError::ATS_SUBSCRIPTION_OK*/){
				actCommand = TA_Base_App::NoCommand;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ATS Subscription stopped successfully ");
			}else if(atsSubErr == (-2) /*ATSSubscriptionError::ATS_SUBSCRIPTION_FAILED_COMMS_ERROR*/){
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSSubscriptionError: %d when try to stopSubscription ", atsSubErr);

			}
		}			

		return actCommand;
	}

	ActionCommand  ComSimulator::doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSDateTime> pStartTime;
		CComPtr<IATSDateTime> pEndTime;
		CComPtr<IATSEventList> pATSEvents;

		transformToATSFilter(pATSFilter, filterCriteria);
		pATSFilter->GetStartTimePeriod(&pStartTime);
		pATSFilter->GetEndTimePeriod(&pEndTime);
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoStartTimeEndTimeQuery().");
		err = pATSBridgeUtility->DoStartTimeEndTimeQuery(
			pATSFilter,
			pStartTime,
			pEndTime,
			&pATSEvents);
		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoStartTimeEndTimeQuery().");

		if( err == 0 /*ATS_QUERY_OK*/ )
		{
			// cut the length to be within atsMaxLength
			truncateATSEvents (pATSEvents, atsMaxLength, true);

			//query succesful:
			transformToISCSEvents ( pATSEvents, events ); 

			// 			// free the temporary cache 
			// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			// 			IATSEventsAPI::FreeATSEventList(pATSEvents);  
			// 			LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");

			actionCommand =  TA_Base_App::NoCommand;

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
			// 			if(pATSEvents->GetSize()>0)
			// 			{
			// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			// 				IATSEventsAPI::FreeATSEventList(pATSEvents);  
			// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
			// 			}
			actionCommand = TA_Base_App::Reconnect;
		}

		return actionCommand;

	}

	void ComSimulator::truncateATSEvents (IATSEventList * pATSEvents, unsigned long atsMaxLength, bool cutSmallTime)
	{
		if( (unsigned long)pATSEvents->GetSize() <= atsMaxLength ) 
		{
			return;
		}
		else 
		{
			// cut some events
			unsigned long cutNum = pATSEvents->GetSize() - atsMaxLength;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Some ATS events need to be cutted according to max length, cut Number %d", cutNum);

			if(cutSmallTime)
			{
				while(cutNum>0)
				{
					pATSEvents->Remove(0);
					--cutNum;
				}
			}
			else
			{
				while(cutNum>0)
				{
					pATSEvents->Remove( pATSEvents->GetSize() - 1);
					--cutNum;
				}
			}
		}
	}


	ActionCommand ComSimulator::doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSEventList> pATSEvents;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		transformToATSFilter(pATSFilter, filterCriteria);

		//query [start, end)
		BSTR bstr_ATSPkey = _com_util::ConvertStringToBSTR(filterCriteria.pkeyBoundary_ATS.c_str());

		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoPreviousPageQuery().");
		err = pATSBridgeUtility->DoPreviousPageQuery( pATSFilter, bstr_ATSPkey, atsMaxLength, &pATSEvents);
		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoPreviousPageQuery().");

		SysFreeString(bstr_ATSPkey);

		if( err == 0 /*ATS_QUERY_OK*/ )
		{
			//query succesful:
			transformToISCSEvents ( pATSEvents, events ); 
			actionCommand =  TA_Base_App::NoCommand;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
			actionCommand = TA_Base_App::Reconnect;
		}

		return actionCommand;

	}

	ActionCommand ComSimulator::doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSEventList> pATSEvents;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		transformToATSFilter(pATSFilter, filterCriteria);

		BSTR bstr_ATSPkey = _com_util::ConvertStringToBSTR(filterCriteria.pkeyBoundary_ATS.c_str());

		//query [start, end)

		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoForwardPageQuery().");
		err = pATSBridgeUtility ->DoForwardPageQuery(pATSFilter, bstr_ATSPkey, atsMaxLength, &pATSEvents);
		LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoForwardPageQuery().");

		SysFreeString(bstr_ATSPkey);

		if( err == 0 /*ATS_QUERY_OK*/ )
		{
			//query succesful:
			transformToISCSEvents ( pATSEvents, events ); 
			actionCommand =  TA_Base_App::NoCommand;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
			actionCommand = TA_Base_App::Reconnect;
		}

		return actionCommand;
	}


	void ComSimulator::restartATSSubscriptionForRefresh()
	{
		stopATSSubscription();

		initATSSubscriber();

		CComPtr<IATSFilterCriteria> pATSFilter;
		transformToATSFilter(pATSFilter, ATSRealTimeObserverManager::getInstance()->getFilterCriteria());
		CComPtr<IATSEventList> pATSEvents;

		if( m_pSubscriber!=NULL )
		{
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
			m_pSubscriber->startSubscription( pATSFilter, EventHandlerManager::getMaxATSEvents() , &pATSEvents);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");

			// 			if (pATSEvents->GetSize()>0)
			// 			{
			// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::FreeATSEventList().");
			// 				IATSEventsAPI::FreeATSEventList(pATSEvents);  
			// 				LOG_GENERIC(SourceInfo, ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::FreeATSEventList().");
			// 			}

		}
	}



};
