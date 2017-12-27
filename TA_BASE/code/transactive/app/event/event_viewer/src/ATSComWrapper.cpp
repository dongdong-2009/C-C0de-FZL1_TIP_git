#include "ATSComWrapper.h"


#include "ATSRealTimeObserverManager.h"
#include "EventHandlerManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/threads/src/Thread.h"
#include <comutil.h>
#include "ConstantItems.h"



namespace TA_Base_App
{
	ATSComWrapper* ATSComWrapper::m_instance_realTime = NULL;
	ATSComWrapper* ATSComWrapper::m_instance_hist = NULL;

	ATSComWrapper * ATSComWrapper::getInstanceRealTime()
	{
		if(m_instance_realTime == NULL)
		{
			m_instance_realTime = new ATSComWrapper();
			CoInitialize(0);
		}
		return m_instance_realTime;
	}
	void ATSComWrapper::freeInstanceRealTime()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATSComWrapper::freeInstanceRealTime() entry . ");
		if(m_instance_realTime!=NULL)
		{
			delete m_instance_realTime;
			m_instance_realTime = NULL;
			CoUninitialize();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATSComWrapper::freeInstanceRealTime() exit . ");
	}
	ATSComWrapper * ATSComWrapper::getInstanceHistorical()
	{
		if(m_instance_hist == NULL)
		{
			m_instance_hist = new ATSComWrapper();
			CoInitialize(0);
		}
		return m_instance_hist;
	}
	void ATSComWrapper::freeInstanceHistorical()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATSComWrapper::freeInstanceHistorical() entry . ");
		if(m_instance_hist!=NULL)
		{
			delete m_instance_hist;
			m_instance_hist = NULL;
			CoUninitialize();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATSComWrapper::freeInstanceHistorical() exit . ");
	}
	
	ATSComWrapper::ATSComWrapper(void)
	{
		m_ATS_func_debug_level = true;
		m_pSubscriber = NULL;
		m_pCP = NULL;
		m_pSink = NULL;
		m_pSinkUnk = NULL;

	}

	ATSComWrapper::~ATSComWrapper(void)
	{
		if(m_pCP!=NULL)
		{
			m_pCP->Unadvise(m_dwAdvise);
			m_pCP->Release();
			m_pCP = NULL;
		}

		if(m_pSink)
		{
			delete m_pSink;
			m_pSink = NULL;
		}
		if (m_pSubscriber)
		{
			m_pSubscriber.Release();
		}

	}

	int ATSComWrapper::initATSSubscriber()
	{
		if(m_pSubscriber != NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has been created. ");
			return 0;  // success
		}
		HRESULT hr = m_pSubscriber.CoCreateInstance(CLSID_ATSEventSubscription, NULL, CLSCTX_INPROC_SERVER);

		if(isFailed(hr))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Fail to create instance in initATSSubscriber().");
			return 1; // fail to create instance
		}
		
		CComPtr<IConnectionPointContainer> pCPC;

		hr = m_pSubscriber->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
		if(isFailed(hr))
		{
			m_pSubscriber = NULL;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Fail to QueryInterface in initATSSubscriber().");
			return 2; // fail to QueryInterface
		}

		hr = pCPC->FindConnectionPoint(DIID__IATSEventSubscriptionEvents,&m_pCP);

		if(isFailed(hr))
		{
			m_pSubscriber = NULL;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Fail to FindConnectionPoint in initATSSubscriber().");
			return 3; // fail to FindConnectionPoint
		}
		
		m_pSink = new CMySink();

		m_pSinkUnk = m_pSink->GetInterface((void*)&IID_IUnknown);

		hr = m_pCP->Advise(m_pSinkUnk,&m_dwAdvise);

		if(isFailed(hr))
		{
			m_pSubscriber = NULL;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Fail to Advise in initATSSubscriber().");
			return 4; // fail to Advise
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has been created. ");
		return 0; // success
	
	}

	ActionCommand ATSComWrapper::startATSSubscription(
		FilterCriteria filterCriteria,
		const unsigned int backloglength,				
		std::vector<TA_Base_Core::ICombinedEventData*>& events)
	{

			ActionCommand actCommand = Reconnect;

			CComPtr<IATSFilterCriteria> pATSFilter;
			HRESULT hr = pATSFilter.CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER);

			if(isFailed(hr))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtil.dll. ATSBridgeUtil may not be installed correctly.");
				return TA_Base_App::Reconnect;
			}
			transformToATSFilter(pATSFilter, filterCriteria );
			CComPtr<IATSEventList> pATSEvents;

			if(initATSSubscriber() != 0)
			{
				return TA_Base_App::Reconnect;
			}

			if( m_pSubscriber != NULL )
			{
				LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
				long atsSubErr = m_pSubscriber ->startSubscription( pATSFilter, backloglength ,& pATSEvents);
				LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");

				for (int i=0; atsSubErr!=0 && i<2 ; i++ )    // fail, try to startSubscription again (try total 3 times at most).
				{
					Thread::sleep(1000);
					LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
					atsSubErr = m_pSubscriber ->startSubscription( pATSFilter, backloglength ,& pATSEvents);
					LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");
				}

				if(atsSubErr == 0 /*ATSSubscriptionError::ATS_SUBSCRIPTION_OK*/)    // subscribe successfully:
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has started a new subscription successfully, and returned total backlog events: %d ", pATSEvents->GetSize());
					try
					{
						transformToISCSEvents ( pATSEvents, events );  
					}
					catch(...)
					{	
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when transfer ATS event to ISCS form.");
					}
					actCommand = NoCommand;
				}
				else /* atsSubErr != 0*/
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get Error when call ATS:startSubscription, ATSSubscriptionError: %d", atsSubErr);
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


	/* m_systemStructureVector will be like { { -1, name1, { (-1,subname1), (-2,subname2) }} ,
											  { -2, name1, { (-3,subname3), (-4,subname4), (-5,subname5) }} ,
											  ...
											}
    The simulated "key" for system and subsystem is created by us.
	*/ 
	int ATSComWrapper::loadATSSystemsInfo( )
	{
		// clear the m_systemStructureVector
		std::vector<ATSSystemItem>::iterator itr;
		for ( itr=m_systemStructureVector.begin(); itr!=m_systemStructureVector.end(); itr++)
		{
			itr->subsystemVector.clear();
		}
		m_systemStructureVector.clear();


		CComPtr<IATSEventSystemList> pEventSystems;
		CComPtr<IATSStringList> pEventTypes;
		CComPtr<IATSStringList> pAlarmTypes;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility;
		HRESULT hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);
		if (!isFailed(hr))
		{
			long err;
			err = pATSBridgeUtility->DoEventInfoQuery ( & pEventSystems, & pEventTypes, & pAlarmTypes );
			if(err == 0 /*ATS_QUERY_OK*/)
			{

				int sysKey = -1;
				int subsysKey = -1;   // use negative number to differentiate from ISCS sys/subsys keys.

				for(int i=0; i<pEventSystems->GetSize(); i++)
				{
					CComPtr<IATSEventSystem> pATSEventSystem;
					pEventSystems->GetItem(i,&pATSEventSystem);

					BSTR bstrName = pATSEventSystem->Name;
					char* name = _com_util::ConvertBSTRToString(bstrName);
					SysFreeString(bstrName);
					m_systemsNames.push_back(name);

					//std::string temp = name;
					vector<pair<int,std::string>> temp_subsysVector;

					CComPtr<IATSStringList> pSubSystemList;
					pATSEventSystem->GetSubSystemList( &pSubSystemList);
					for( int j=0; j<pSubSystemList->GetSize(); j++)
					{
						BSTR bstrSubName = pSubSystemList->GetItem(j);
						char* subName = _com_util::ConvertBSTRToString(bstrSubName);
						SysFreeString(bstrSubName);

						// 					std::pair<std::string, std::string> subsystem;
						// 					subsystem.first = subName;
						// 					subsystem.second = name;
						// 					m_subsystems.push_back(subsystem);

						pair<int,std::string>  subsystem_KeyNamePair;
						subsystem_KeyNamePair.first = subsysKey;
						subsystem_KeyNamePair.second = subName;
						temp_subsysVector.push_back(subsystem_KeyNamePair);
						subsysKey--;
						delete[] subName;
					}

					//m_systemStructureVector.insert(std::map<std::string, std::vector<std::string>>::value_type( name, temp_subsysVector ) );
					ATSSystemItem tempATSSystemItem(sysKey, name);
					delete[] name;
					tempATSSystemItem.subsystemVector = temp_subsysVector;
					m_systemStructureVector.push_back(tempATSSystemItem);
					sysKey--;

				}
			}
			return err;
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtil.dll. ATSBridgeUtil may not be installed correctly.");
			return -1;
		}
		
	}

	vector<std::string> ATSComWrapper::getATSSystemsNames()
	{
		return m_systemsNames;
	}

	vector<ATSSystemItem> ATSComWrapper::getATSSystemStructureVector()
	{
		return m_systemStructureVector;
	}


	void ATSComWrapper::setSystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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
	void ATSComWrapper::setSubsystemNameToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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
	void ATSComWrapper::setOperatorToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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
	void ATSComWrapper::setLocationToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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

	// change here: set the eventType strings to list<ATSEventAlarmState> AlarmStateEqualOr
	void ATSComWrapper::setAlarmStateToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSIntList> pTempIntList;		
		pATSFilter->GetAlarmStateEqualOr(&pTempIntList);   //need to test here!
		pTempIntList->Clear();

		for(list<std::string>::iterator itr =filterCriteria.EventTypeEqualOr.begin();
			itr!=filterCriteria.EventTypeEqualOr.end(); itr++)
		{
			string alarmState = *itr;
			if( ALARM_CLOSED == alarmState)
			{
				pTempIntList->Add(0);
			}else if( ALARM_OPENED == alarmState)
			{
				pTempIntList->Add(1);
			}else if( ALARM_NORMALISED == alarmState)
			{
				pTempIntList->Add(2);
			}else if( ALARM_ACKNOWLEDGED == alarmState)
			{
				pTempIntList->Add(3);
			}else if( ALARM_SUPPRESSED == alarmState)
			{
				pTempIntList->Add(4);
			}

		}
	}

	void ATSComWrapper::setAssetToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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

	void ATSComWrapper::setDescriptionToATSFilter (IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
	{
		CComPtr<IATSStringList> pTempStrList5;
		pATSFilter->GetDescriptionEqualOr(&pTempStrList5);
		pTempStrList5->Clear();
		for(list<std::string>::iterator itr =filterCriteria.Description.begin();
			itr!=filterCriteria.Description.end(); itr++)
		{
			BSTR bstrText = _com_util::ConvertStringToBSTR( (*itr).c_str());
			pTempStrList5->Add(bstrText);
			SysFreeString(bstrText);
		}
	}
	

	// has been confirmed: 0 is used to get Non-alarm events here
	// change -1( in filterCriteria means "Non-alarm") to 0 (in ATSFilter means "Non-alarm")
	void ATSComWrapper::setAlarmSeverityToATSFilter(IATSFilterCriteria *pATSFilter,FilterCriteria & filterCriteria)
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
	void ATSComWrapper::transformToATSFilter(IATSFilterCriteria * pATSFilter, FilterCriteria & filterCriteria )
	{
		pATSFilter->StartTimePeriodDefined = (filterCriteria.StartTimePeriodDefined)?VARIANT_TRUE:VARIANT_FALSE;
		pATSFilter->EndTimePeriodDefined = (filterCriteria.EndTimePeriodDefined)?VARIANT_TRUE:VARIANT_FALSE;

		setStartEndTimeToATSFilter (pATSFilter,filterCriteria);

// 		// adjust the star/end time for these situations:
// 		adjustATSFilterTime(pATSFilter);

		//system , subsystem, location, operator, asset
		setSystemNameToATSFilter (pATSFilter,filterCriteria);

		setSubsystemNameToATSFilter(pATSFilter,filterCriteria);

		setLocationToATSFilter(pATSFilter,filterCriteria);

		setOperatorToATSFilter(pATSFilter,filterCriteria);

		setAssetToATSFilter(pATSFilter,filterCriteria);

		setDescriptionToATSFilter(pATSFilter,filterCriteria);

		setTMTypeToATSFilter(pATSFilter,filterCriteria);

		setAlarmSeverityToATSFilter(pATSFilter,filterCriteria);

		setAlarmStateToATSFilter(pATSFilter,filterCriteria); // set FilterCriteria's EventTypeEqualOr To Alarm state

	}

	void  ATSComWrapper::setStartEndTimeToATSFilter ( ATSBridgeUtilLib::IATSFilterCriteria * pATSfilter, 
		FilterCriteria & filterCriteria)
	{
		if(pATSfilter->StartTimePeriodDefined == VARIANT_TRUE)
		{
			ISCSDateTime tempTime = filterCriteria.StartTimePeriod;
			CComPtr<IATSDateTime> pStartTime;
			pATSfilter->GetStartTimePeriod( &pStartTime );
			pStartTime->SetDate(tempTime.Year, tempTime.Month, tempTime.Day, 
				tempTime.Hour,tempTime.Minute, tempTime.Second, tempTime.Nanosecond);
		}
		
		if(pATSfilter->EndTimePeriodDefined == VARIANT_TRUE)
		{
			ISCSDateTime tempTime = filterCriteria.EndTimePeriod;
			CComPtr<IATSDateTime> pEndTime;
			pATSfilter->GetEndTimePeriod( &pEndTime );
			pEndTime->SetDate(tempTime.Year, tempTime.Month, tempTime.Day, 
				tempTime.Hour,tempTime.Minute, tempTime.Second, tempTime.Nanosecond);
		}
 		
	}

// 	void ATSComWrapper::adjustATSFilterTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter)
// 	{
// 
// 		const CTime currentTime = CTime::GetCurrentTime();
// 
// 		if(atsfilter ->StartTimePeriodDefined == VARIANT_FALSE && atsfilter->EndTimePeriodDefined == VARIANT_FALSE)
// 		{   // this is real time mode: [current-1day, current]
// 
// 			CTime startTime(currentTime.GetTime()-EventViewerConstants::TWENTY_FOUR_HOURS);
// 
// 			setATSFilterStartTimeByCTime(atsfilter, startTime);
// 
// 			setATSFilterEndTimeByCTime(atsfilter, currentTime);
// 
// 		}
// 		else if(atsfilter->StartTimePeriodDefined == VARIANT_TRUE && atsfilter->EndTimePeriodDefined == VARIANT_FALSE)
// 		{
// 			// adjust end time to current time or starttime+1day , whichever comes first
// 			
// 			CComPtr<IATSDateTime> pStartTime;
// 			atsfilter->GetStartTimePeriod(&pStartTime);
// 			//long nano = pStartTime->Nanosecond;
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
// 			atsfilter->EndTimePeriodDefined = VARIANT_TRUE;
// 
// 		}
// 		else if(atsfilter->StartTimePeriodDefined == VARIANT_FALSE && atsfilter->EndTimePeriodDefined == VARIANT_TRUE)  
// 		{
// 			// adjust start time as endtime- 1day
// 			CComPtr<IATSDateTime> pEndTime;
// 			atsfilter->GetEndTimePeriod(&pEndTime);
// 			ISCSDateTime endTime_ISCS(pEndTime->Year,pEndTime->Month, pEndTime->Day, 
// 				pEndTime->Hour, pEndTime->Minute,pEndTime->Second,pEndTime->Nanosecond);
// 
// 			CTime startTime( endTime_ISCS.getTime_t() - EventViewerConstants::TWENTY_FOUR_HOURS);
// 			setATSFilterStartTimeByCTime(atsfilter, startTime);
// 			atsfilter->StartTimePeriodDefined = VARIANT_TRUE;
// 		}
// 	}

	void ATSComWrapper::setATSFilterStartTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time)
	{
		/*IATSDateTime* pStartTime = NULL;*/
		CComPtr<IATSDateTime> pStartTime;
		atsfilter->GetStartTimePeriod( &pStartTime );
		pStartTime->SetDate(time.GetYear(), time.GetMonth(), time.GetDay(), 
			time.GetHour(), time.GetMinute(), time.GetSecond(),0);

	}

	void ATSComWrapper::setATSFilterEndTimeByCTime(ATSBridgeUtilLib::IATSFilterCriteria *  atsfilter, CTime time)
	{
		/*IATSDateTime* pEndTime = NULL;*/
		CComPtr<IATSDateTime> pEndTime;
		atsfilter->GetEndTimePeriod( &pEndTime );
		pEndTime->SetDate(time.GetYear(), time.GetMonth(), time.GetDay(), 
			time.GetHour(), time.GetMinute(), time.GetSecond(),0);
	}

	void  ATSComWrapper:: setTMTypeToATSFilter(ATSBridgeUtilLib::IATSFilterCriteria * atsfilter, FilterCriteria & filterCriteria)
	{
		CComPtr<IATSIntList> pOMType;
		atsfilter->GetOperationalMaintenanceTypeEqualOr( &pOMType );
		pOMType->Clear();

		if(filterCriteria.TMTypeEqualOr.size()>0){

			string temp = filterCriteria.TMTypeEqualOr.front().c_str(); 
			if ( "O"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL);*/
				pOMType->Add(0);
			}else if( "M"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(1/ *ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE* /);*/
				pOMType->Add(1);
			}else if( "B"== temp ){
				/*atsfilter.AlarmTypeEqualOr.push_back(2/ *ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH* /);*/
				pOMType->Add(2);
			}
		}

		return;
	}

	void ATSComWrapper::transformToISCSEvents ( IATSEventList * pATSEvents, 
		std::vector<TA_Base_Core::ICombinedEventData*>& events , unsigned long lastKey)
	{
		if (pATSEvents == NULL || pATSEvents->GetSize()==0) return;

		//if the destination is not empty, clean it first
		if (events.size() > 0)
		{
			for (int i=0; i<(int)events.size(); i++)
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

	}

	ICombinedEventData * ATSComWrapper::createCombinedEvent ( IATSEvent* pATSEvent, unsigned long key)
	{
		try{
			// get the creatime (timeb)
			CComPtr<IATSDateTime> pDateTime;
			pATSEvent->GetTimeStamp(&pDateTime);
			ISCSDateTime iscsTime(pDateTime->Year,pDateTime->Month, pDateTime->Day, 
				pDateTime->Hour, pDateTime->Minute, pDateTime->Second, pDateTime->Nanosecond);
			timeb createtime;
			createtime.time = iscsTime.getTime_t();
			createtime.millitm = (unsigned short) ((iscsTime.Nanosecond) / 1000000);
			struct _timeb timebuffer;
			_ftime( &timebuffer );
			createtime.timezone = timebuffer.timezone;
			createtime.dstflag = timebuffer.dstflag;


			// get alarmSeverityName
			char buff[7];
			//itoa (pATSEvent->AlarmSeverity, buff, 10);
			_itoa_s(pATSEvent->AlarmSeverity,buff, 10);
			std::string alarmSeverityName = buff;

			BSTR bstrAsset = pATSEvent->Asset;
			char* assetName = _com_util::ConvertBSTRToString(bstrAsset);
			SysFreeString(bstrAsset);
			string assetName_str = assetName;
			delete[] assetName;

			BSTR bstrDesc = pATSEvent->Description;
			char* description = _com_util::ConvertBSTRToString(bstrDesc);
			SysFreeString(bstrDesc);
			string description_str = description;
			delete[] description;

			BSTR bstrVal = pATSEvent->Value;
			char* value = _com_util::ConvertBSTRToString(bstrVal);
			SysFreeString(bstrVal);
			string value_str = value;
			delete[] value;


			// MMS type:
			std::string alarmMmsState = "";
			short mmsState = pATSEvent->AlarmMMSState;
			switch(mmsState)
			{

			case 1: //ATS_EVENTALARMMMSSTATE_SEMI
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_SEMI, buff, 10); // "S";
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_SEMI, buff, 10); // "S";
				alarmMmsState = buff;
				break;
			case 2: //ATS_EVENTALARMMMSSTATE_AUTO
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_AUTO, buff, 10); // "A";
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_AUTO, buff, 10); // "A";
				alarmMmsState = buff;
				break;
			case 3: //ATS_EVENTALARMMMSSTATE_JR_SENT
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_JR_SENT, buff, 10);; //"R";
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_JR_SENT, buff, 10);; //"R";
				alarmMmsState = buff;
				break;
			case 4: //ATS_EVENTALARMMMSSTATE_ALM_SENT
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_ALM_SENT, buff, 10);; //"A";
				alarmMmsState = buff;
				break;
			case 5: //ATS_EVENTALARMMMSSTATE_INHIBIT
				//alarmMmsState = itoa(TA_Base_Core::IAlarmData::TYPE_INHIBIT, buff, 10);; //"I";
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_INHIBIT, buff, 10);; //"I";
				alarmMmsState = buff;
				break;
			case 6: //ATS_EVENTALARMMMSSTATE_FAILED
				_itoa_s(TA_Base_Core::IAlarmData::TYPE_FAILED, buff, 10);; //"D";
				alarmMmsState = buff;
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
				_itoa_s(TA_Base_Core::IAlarmData::CHILD_AVL, buff, 10);
				alarmAVlState = buff;
			}else if(/* ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_HEAD*/ 0 == pATSEvent->AlarmAvalancheType)
			{
				//alarmAVlState = itoa(TA_Base_Core::IAlarmData::HEAD_AVL, buff, 10);
				_itoa_s(TA_Base_Core::IAlarmData::HEAD_AVL, buff, 10);	
				alarmAVlState = buff;
			}
			else
			{
				//alarmAVlState = itoa(TA_Base_Core::IAlarmData::NO_ASSOC_AVL, buff, 10);
				_itoa_s(TA_Base_Core::IAlarmData::NO_ASSOC_AVL, buff, 10);
				alarmAVlState = buff;
			}


			BSTR bstrOperator = pATSEvent->Operator;
			char* operatorName = _com_util::ConvertBSTRToString(bstrOperator);
			SysFreeString(bstrOperator);
			string operatorName_str = operatorName;
			delete[] operatorName;

			std::string alarmComment = "";

			BSTR bstrAlarmID = pATSEvent->AlarmID;
			char* alarmID = _com_util::ConvertBSTRToString(bstrAlarmID);
			SysFreeString(bstrAlarmID);
			string alarmID_str = alarmID;
			delete[] alarmID;

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
			string eventID_str = eventID;
			delete[] eventID;


			// to do: change CombinedEventData to CombinedEventItem
			return new CombinedEventData( TA_Base_Core::E_AUDIT_EVENT, key, createtime, alarmSeverityName, assetName_str, description_str,
				value_str, alarmMmsState,alarmDssState, alarmAVlState, operatorName_str, alarmComment, alarmID_str, alarmAck, alarmStatus,
				alarmStateStr, operation_mode, TA_Base_Core::ATSCategory,eventID_str);


			

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught in createCombinedEvent().");
		}

		return NULL; //todo check

	}

	ActionCommand ATSComWrapper::stopATSSubscription(void){

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
				actCommand = TA_Base_App::Reconnect;

			}
		}			

		return actCommand;
	}

	ActionCommand  ATSComWrapper::doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSDateTime> pStartTime;
		CComPtr<IATSDateTime> pEndTime;
		CComPtr<IATSEventList> pATSEvents;

		HRESULT hr = pATSFilter.CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER);

		if(isFailed(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtil.dll. ATSBridgeUtil may not be installed correctly.");
			return TA_Base_App::Reconnect;
		}

		transformToATSFilter(pATSFilter, filterCriteria);
		pATSFilter->GetStartTimePeriod(&pStartTime);
		pATSFilter->GetEndTimePeriod(&pEndTime);
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);

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

			actionCommand =  TA_Base_App::NoCommand;

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSQueryError: %d", err);
			actionCommand = TA_Base_App::Reconnect;
		}

		return actionCommand;

	}

	void ATSComWrapper::truncateATSEvents (IATSEventList * pATSEvents, unsigned long atsMaxLength, bool cutSmallTime)
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


	ActionCommand ATSComWrapper::doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSEventList> pATSEvents;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		HRESULT hr = pATSFilter.CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER);
		if(isFailed(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtil.dll. ATSBridgeUtil may not be installed correctly.");
			return TA_Base_App::Reconnect;
		}

		hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);

		transformToATSFilter(pATSFilter, filterCriteria);
		
		if(filterCriteria.pkeyBoundary_ATS != "")
		{
			BSTR bstr_ATSPkey = _com_util::ConvertStringToBSTR(filterCriteria.pkeyBoundary_ATS.c_str());

			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoPreviousPageQuery().");
			err = pATSBridgeUtility->DoPreviousPageQuery( pATSFilter, bstr_ATSPkey, atsMaxLength, &pATSEvents);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoPreviousPageQuery().");

			SysFreeString(bstr_ATSPkey);
		}
		else
		{
			CComPtr<IATSDateTime> pEndTime;
			pATSFilter->GetEndTimePeriod(&pEndTime);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoPreviousPageQuery().");
			err = pATSBridgeUtility->DoPreviousPageQueryDate( pATSFilter, pEndTime, atsMaxLength, &pATSEvents);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoPreviousPageQuery().");

		}


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

	ActionCommand ATSComWrapper::doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
		FilterCriteria filterCriteria, unsigned long atsMaxLength)
	{
		ActionCommand actionCommand = TA_Base_App::Reconnect;
		long err;
		CComPtr<IATSFilterCriteria> pATSFilter;
		CComPtr<IATSEventList> pATSEvents;
		CComPtr<IATSBridgeUtility> pATSBridgeUtility; 

		HRESULT hr = pATSFilter.CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER);
		if(isFailed(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtil.dll. ATSBridgeUtil may not be installed correctly.");
			return TA_Base_App::Reconnect;
		}

		hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);
		transformToATSFilter(pATSFilter, filterCriteria);

		if(filterCriteria.pkeyBoundary_ATS != "")
		{
			BSTR bstr_ATSPkey = _com_util::ConvertStringToBSTR(filterCriteria.pkeyBoundary_ATS.c_str());

			//query [start, end)

			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoForwardPageQuery_byID.");
			err = pATSBridgeUtility ->DoForwardPageQuery(pATSFilter, bstr_ATSPkey, atsMaxLength, &pATSEvents);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoForwardPageQuery_byID.");

			SysFreeString(bstr_ATSPkey);

		}else
		{
			CComPtr<IATSDateTime> pStartTime;
			pATSFilter->GetStartTimePeriod(&pStartTime);

			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::DoForwardPageQuery_byTime.");
			err = pATSBridgeUtility ->DoForwardPageQueryDate(pATSFilter, pStartTime, atsMaxLength, &pATSEvents);
			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::DoForwardPageQuery_byTime.");

		}

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


// 	void ATSComWrapper::restartATSSubscriptionForRefresh()
// 	{
// 		stopATSSubscription();
// 
// 		initATSSubscriber();
// 
// 		CComPtr<IATSFilterCriteria> pATSFilter;
// 		transformToATSFilter(pATSFilter, ATSRealTimeObserverManager::getInstance()->getFilterCriteria());
// 		CComPtr<IATSEventList> pATSEvents;
// 
// 		if( m_pSubscriber!=NULL )
// 		{
// 			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Coming into IATSEventsAPI::startSubscription().");
// 			m_pSubscriber->startSubscription( pATSFilter, EventHandlerManager::getMaxATSEvents() , &pATSEvents);
// 			LOG_GENERIC(SourceInfo, m_ATS_func_debug_level? DebugUtil::DebugDebug : DebugUtil::DebugInfo , "Come out from IATSEventsAPI::startSubscription().");
// 
// 		}
// 	}

	bool ATSComWrapper::isFailed(HRESULT hres)
	{
		if(hres < 0)
			return true;
		else
			return false;
	}

};
