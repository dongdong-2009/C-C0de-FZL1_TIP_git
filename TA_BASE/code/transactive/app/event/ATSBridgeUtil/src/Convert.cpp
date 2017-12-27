#include "StdAfx.h"
#include "Convert.h"

using namespace IATSEventsAPI;

LoggerPtr CConvert::m_pLogger(Logger::getLogger("CConvert"));

CConvert::CConvert(void)
{
}

CConvert::~CConvert(void)
{
}

HRESULT CConvert::ATSType2IATSType(IATSEventsAPI::ATSDateTime atsDateTime, IATSDateTime** pIatsSDateTime)
{
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSDateTime->IATSDateTime) entry");

	if(pIatsSDateTime == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsSDateTime == NULL");
		return E_POINTER;
	}
	if(*pIatsSDateTime == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSDateTime, NULL, CLSCTX_INPROC_SERVER, IID_IATSDateTime, (void**)pIatsSDateTime);
		if(FAILED(hr)) 
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: " << hr);
			return hr;
		}
	}

	(*pIatsSDateTime)->put_Day(atsDateTime.Day);
	(*pIatsSDateTime)->put_Hour(atsDateTime.Hour);
	(*pIatsSDateTime)->put_Minute(atsDateTime.Minute);
	(*pIatsSDateTime)->put_Month(atsDateTime.Month);
	(*pIatsSDateTime)->put_Nanosecond(atsDateTime.Nanosecond);
	(*pIatsSDateTime)->put_Second(atsDateTime.Second);
	(*pIatsSDateTime)->put_Year(atsDateTime.Year);

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSDateTime->IATSDateTime) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(IATSEventsAPI::ATSEventSystem atsEventSystem, IATSEventSystem** pIatsEventSystem)
{
	USES_CONVERSION;
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSEventSystem->IATSEventSystem)entry");

	if(pIatsEventSystem == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsEventSystem == NULL");
		return E_POINTER;
	}

	if(*pIatsEventSystem == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSEventSystem, NULL, CLSCTX_INPROC_SERVER, IID_IATSEventSystem, (void**)pIatsEventSystem);
		if(FAILED(hr)) 
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: " << hr);
			return hr;
		}
	}

	CComBSTR bstrData = A2OLE(atsEventSystem.Name.c_str());
	IATSStringList *pStrList = NULL;

	(*pIatsEventSystem)->put_Name(bstrData);
	(*pIatsEventSystem)->GetSubSystemList(&pStrList);

	ATSType2IATSType(atsEventSystem.SubsystemNames, &pStrList);

	pStrList->Release();

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSDateTime->IATSDateTime) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(std::list<IATSEventsAPI::ATSEventSystem> atsEventSystems, IATSEventSystemList** pIatsEventSystems)
{
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<ATSEventSystem>->IATSEventSystemList) entry");

	if(pIatsEventSystems == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsEventSystems == NULL");
		return E_POINTER;
	}


	if(*pIatsEventSystems == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSEventSystemList, NULL, CLSCTX_INPROC_SERVER, IID_IATSEventSystemList, (void**)pIatsEventSystems);
		if(FAILED(hr)) 
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: %ld" << hr);
			return hr;
		}
	}
	std::list<IATSEventsAPI::ATSEventSystem>::iterator it;
	for(it = atsEventSystems.begin(); it != atsEventSystems.end(); it++)
	{
		IATSEventSystem* pEventSystem = NULL;
		ATSType2IATSType((*it), &pEventSystem);
		(*pIatsEventSystems)->Add(pEventSystem);
		pEventSystem->Release();
	}

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<ATSEventSystem>->IATSEventSystemList) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(IATSEventsAPI::ATSEvent atsEvent, IATSEvent** pIatsEvent)
{
	USES_CONVERSION;
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSEvent->IATSEvent) entry");

	if(pIatsEvent == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsEvent == NULL");
		return E_POINTER;
	}
		
	if((*pIatsEvent) == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSEvent, NULL, CLSCTX_INPROC_SERVER, IID_IATSEvent, (void**)pIatsEvent);
		if(FAILED(hr)) 
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: " << hr);
			return hr;
		}
	}
	IATSDateTime *pTimeStamp = NULL;

	(*pIatsEvent)->put_AlarmAvalancheType((SHORT) atsEvent.AlarmAvalancheType);
	(*pIatsEvent)->put_AlarmID(A2OLE(atsEvent.AlarmID.c_str()));
	(*pIatsEvent)->put_AlarmMMSState((SHORT) atsEvent.AlarmMMSState);
	(*pIatsEvent)->put_AlarmSeverity((SHORT) atsEvent.Severity); // change from AlarmSevirity -> Severity
	(*pIatsEvent)->put_OperationalMaintenanceType((SHORT) atsEvent.OperationalMaintenanceType); // change from AlarmType -> OperationalMaintenanceType
	(*pIatsEvent)->put_AlarmType(A2OLE(atsEvent.AlarmType.c_str())); //++ new attribute 
	(*pIatsEvent)->put_Asset(A2OLE(atsEvent.Asset.c_str()));
	(*pIatsEvent)->put_Description(A2OLE(atsEvent.Description.c_str()));
	(*pIatsEvent)->put_EventID(A2OLE(atsEvent.EventID.c_str()));
	(*pIatsEvent)->put_EventType(A2OLE(atsEvent.EventType.c_str()));
	(*pIatsEvent)->put_Location(A2OLE(atsEvent.Location.c_str()));
	(*pIatsEvent)->put_Operator(A2OLE(atsEvent.Operator.c_str()));
	(*pIatsEvent)->put_SubsystemName(A2OLE(atsEvent.SubsystemName.c_str()));
	(*pIatsEvent)->put_SystemName(A2OLE(atsEvent.SystemName.c_str()));
	(*pIatsEvent)->put_Value(A2OLE(atsEvent.Value.c_str()));
	(*pIatsEvent)->put_AlarmState((SHORT)atsEvent.AlarmState); //++ new attribute // added in ver 2

	(*pIatsEvent)->GetTimeStamp(&pTimeStamp);
	
	ATSType2IATSType(atsEvent.Timestamp, &pTimeStamp);
	
	pTimeStamp->Release();

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSEvent->IATSEvent) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(std::list<IATSEventsAPI::ATSEvent*> atsEvents, IATSEventList** pIatsEvents)
{
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<ATSEvent*>->IATSEventList) entry");

	if(pIatsEvents == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsEvents == NULL");
		return E_POINTER;
	}

	if(*pIatsEvents == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSEventList, NULL, CLSCTX_INPROC_SERVER, IID_IATSEventList, (void**)pIatsEvents);
		if(FAILED(hr))
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: " << hr);
			return hr;
		}
	}

	std::list<IATSEventsAPI::ATSEvent*>::iterator it;

	for(it = atsEvents.begin(); it != atsEvents.end(); it++)
	{
		IATSEvent *pEvent = NULL;
		IATSEventsAPI::ATSEvent *atsEvent = NULL;
		
		atsEvent = (*it);

		ATSType2IATSType((*atsEvent), &pEvent);
		(*pIatsEvents)->Add(pEvent);
		pEvent->Release();
	}

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<ATSEvent*>->IATSEventList) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(IATSEventsAPI::ATSFilterCriteria atsFilterCriteria, IATSFilterCriteria** pIatsFilterCriteria)
{
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSFilterCriteria->IATSFilterCriteria) entry");

	if(pIatsFilterCriteria == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsFilterCriteria == NULL");
		return E_POINTER;
	}	
	if(*pIatsFilterCriteria == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSFilterCriteria, NULL, CLSCTX_INPROC_SERVER, IID_IATSFilterCriteria, (void**)pIatsFilterCriteria);
		if(FAILED(hr))
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: " << hr);
			return hr;
		}
	}
	IATSIntList* pTempIntList = NULL;
	IATSStringList * pTempStrList = NULL;
	IATSDateTime * pTempDate = NULL;
	
	//1. EndTimePeriodDefined
	(*pIatsFilterCriteria)->put_EndTimePeriodDefined(atsFilterCriteria.EndTimePeriodDefined? VARIANT_TRUE : VARIANT_FALSE);

	//2. StartTimePeriodDefined
	(*pIatsFilterCriteria)->put_StartTimePeriodDefined(atsFilterCriteria.StartTimePeriodDefined? VARIANT_TRUE : VARIANT_FALSE);
	
	//3. AlarmAvalancheTypeEqualOr
	(*pIatsFilterCriteria)->GetAlarmAvalancheTypeEqualOr(&pTempIntList);
	std::list<IATSEventsAPI::ATSEventAlarmAvalancheType>::iterator it;
	for(it = atsFilterCriteria.AlarmAvalancheTypeEqualOr.begin(); it != atsFilterCriteria.AlarmAvalancheTypeEqualOr.end(); it++)
	{
		pTempIntList->Add((LONG) *it);
	}
	pTempIntList->Release();
	
	//4. AlarmIDEqualOr
	(*pIatsFilterCriteria)->GetAlarmIDEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.AlarmIDEqualOr, &pTempStrList);
	pTempStrList->Release();

	//5. ATSEventAlarmStateEqualOr //++ ver 2 change
	(*pIatsFilterCriteria)->GetAlarmStateEqualOr(&pTempIntList);
	std::list<IATSEventsAPI::ATSEventAlarmState>::iterator it1;
	for(it1 = atsFilterCriteria.AlarmStateEqualOr.begin(); it1 != atsFilterCriteria.AlarmStateEqualOr.end(); it1++)
	{
		pTempIntList->Add((LONG) *it1);
	}
	pTempIntList->Release();

	//6. AlarmSeverityEqualOr // -> SeverityEqualOr
	(*pIatsFilterCriteria)->GetAlarmSeverityEqualOr(&pTempIntList);
	ATSType2IATSType(atsFilterCriteria.SeverityEqualOr, &pTempIntList);
	pTempIntList->Release();

	//7. AlarmTypeEqualOr // -> ATSOperationalMaintenanceType
	(*pIatsFilterCriteria)->GetOperationalMaintenanceTypeEqualOr(&pTempIntList);
	std::list<IATSEventsAPI::ATSOperationalMaintenanceType>::iterator it2;
	for(it2 = atsFilterCriteria.OperationalMaintenanceTypeEqualOr.begin(); it2 != atsFilterCriteria.OperationalMaintenanceTypeEqualOr.end(); it2++)
	{
		pTempIntList->Add((LONG) *it2);
	}
	pTempIntList->Release();

	//8. AssetEqualOr // -> AssetLikeOr
	(*pIatsFilterCriteria)->GetAssetEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.AssetLikeOr, &pTempStrList);
	pTempStrList->Release();

	//9. DescriptionEqualOr // -> DescriptionLikeOr
	(*pIatsFilterCriteria)->GetDescriptionEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.DescriptionLikeOr, &pTempStrList);
	pTempStrList->Release();

	//10. EndTimePeriod
	(*pIatsFilterCriteria)->GetEndTimePeriod(&pTempDate);
	ATSType2IATSType(atsFilterCriteria.EndTimePeriod, &pTempDate);
	pTempDate->Release();

	//11. StartTimePeriod
	(*pIatsFilterCriteria)->GetStartTimePeriod(&pTempDate);
	ATSType2IATSType(atsFilterCriteria.StartTimePeriod, &pTempDate);
	pTempDate->Release();

	//12. EventTypeEqualOr
	(*pIatsFilterCriteria)->GetEventTypeEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.EventTypeEqualOr, &pTempStrList);
	pTempStrList->Release();

	//13. LocationEqualOr
	(*pIatsFilterCriteria)->GetLocationEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.LocationEqualOr, &pTempStrList);
	pTempStrList->Release();

	//14. OperatorEqualOr
	(*pIatsFilterCriteria)->GetOperatorEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.OperatorEqualOr, &pTempStrList);
	pTempStrList->Release();

	//15. SubsystemNameEqualOr
	(*pIatsFilterCriteria)->GetSubsystemNameEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.SubsystemNameEqualOr, &pTempStrList);
	pTempStrList->Release();

	//16. SystemNameEqualOr
	(*pIatsFilterCriteria)->GetSystemNameEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.SystemNameEqualOr, &pTempStrList);
	pTempStrList->Release();

	//17. ValueEqualOr
	(*pIatsFilterCriteria)->GetValueEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.ValueEqualOr, &pTempStrList);
	pTempStrList->Release();

	//18. AlarmTypeEqualOr
	(*pIatsFilterCriteria)->GetAlarmTypeEqualOr(&pTempStrList);
	ATSType2IATSType(atsFilterCriteria.AlarmTypeEqualOr, &pTempStrList);
	pTempStrList->Release();


	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(ATSFilterCriteria->IATSFilterCriteria) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(std::list<std::string> strList, IATSStringList** pIatsStringList)
{
	USES_CONVERSION;
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<std::string>->IATSStringList) entry");

	if(pIatsStringList == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsStringList == NULL");
		return E_POINTER;
	}	
	if(*pIatsStringList == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSStringList, NULL, CLSCTX_INPROC_SERVER, IID_IATSStringList, (void**)pIatsStringList);
		if(FAILED(hr))
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: %d" << hr);
			return hr;
		}
	}

	std::list<std::string>::iterator it;

	for(it = strList.begin(); it != strList.end(); it++)
	{
		CComBSTR data = A2OLE((*it).c_str());
		(*pIatsStringList)->Add(data);
	}

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<std::string>->IATSStringList) exit");
	return S_OK;
}

HRESULT CConvert::ATSType2IATSType(std::list<short int> intList, IATSIntList** pIatsIntList)
{
	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<short int>->IATSIntList) entry");

	if((pIatsIntList) == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_POINTER | pIatsIntList == NULL");
		return E_POINTER;
	}
	if(*pIatsIntList == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSIntList, NULL, CLSCTX_INPROC_SERVER, IID_IATSIntList, (void**)pIatsIntList);
		if(FAILED(hr))
		{
			LOG4CXX_ERROR(m_pLogger, "Failed CoCreateInstance | HRESULT: %d" << hr);
			return hr;
		}
	}
	std::list<short int>::iterator it;

	for(it = intList.begin(); it != intList.end(); it++)
	{
		(*pIatsIntList)->Add((LONG)*it);
	}

	LOG4CXX_TRACE(m_pLogger, "ATSType2IATSType(std::list<short int>->IATSIntList) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSEventList* pIatsEventList, std::list<IATSEventsAPI::ATSEvent*>& atsEvents)
{
	LONG lMAX = 0;
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventList->std::list<ATSEvent*>) entry");
	
	pIatsEventList->GetSize(&lMAX);

	for(int nIndex = 0; nIndex < lMAX; nIndex++)
	{
		IATSEvent* patsTempEvent = NULL;
		IATSEventsAPI::ATSEvent* atsTempEvent;
		atsTempEvent = new IATSEventsAPI::ATSEvent;

		pIatsEventList->GetItem(nIndex, &patsTempEvent);
		IATSType2ATSType(patsTempEvent, *atsTempEvent);
		atsEvents.push_back(atsTempEvent);

		patsTempEvent->Release();
	}

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventList->std::list<ATSEvent*>) exit");

	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSDateTime* pIatsDateTime, IATSEventsAPI::ATSDateTime& atsDateTime)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSDateTime->ATSDateTime) entry");

	if(pIatsDateTime == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_INVALIDARG | pIatsDateTime == NULL");
		return E_INVALIDARG;
	}

	pIatsDateTime->get_Day(&atsDateTime.Day);
	pIatsDateTime->get_Hour(&atsDateTime.Hour);
	pIatsDateTime->get_Minute(&atsDateTime.Minute);
	pIatsDateTime->get_Month(&atsDateTime.Month);
	pIatsDateTime->get_Nanosecond(&atsDateTime.Nanosecond);
	pIatsDateTime->get_Second(&atsDateTime.Second);
	pIatsDateTime->get_Year(&atsDateTime.Year);

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSDateTime->ATSDateTime) exit");

	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSEventSystem* pIatsEventSystem, IATSEventsAPI::ATSEventSystem& atsEventSystem)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventSystem->ATSEventSystem) entry");

	if(pIatsEventSystem == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_INVALIDARG | pIatsEventSystem == NULL");
		return E_INVALIDARG;
	}

	CComBSTR bstrTemp;
	IATSStringList *pTempStrList = NULL;

	pIatsEventSystem->get_Name(&bstrTemp);
	CW2A pszName(bstrTemp);
	atsEventSystem.Name = pszName;

	pIatsEventSystem->GetSubSystemList(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsEventSystem.SubsystemNames);
	pTempStrList->Release();

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventSystem->ATSEventSystem) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSEvent* pIatsEvent, IATSEventsAPI::ATSEvent& atsEvent)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEvent->ATSEvent) entry");

	if(pIatsEvent == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_INVALIDARG | pIatsEvent == NULL");
		return E_INVALIDARG;
	}
	SHORT nTemp = 0;
	CComBSTR bstrTemp;

	//1. AlarmAvalancheType
	pIatsEvent->get_AlarmAvalancheType(&nTemp);
	atsEvent.AlarmAvalancheType = (ATSEventAlarmAvalancheType)nTemp;
	//2. AlarmID
	pIatsEvent->get_AlarmID(&bstrTemp);
	CW2A pszAlarmID(bstrTemp);
	atsEvent.AlarmID = pszAlarmID;
	//3. AlarmMMSState - modified from AlarmMMSType
	pIatsEvent->get_AlarmMMSState(&nTemp);
	atsEvent.AlarmMMSState = (ATSEventAlarmMMSState)nTemp;
	//4. Alarm Severity
	pIatsEvent->get_AlarmSeverity(&atsEvent.Severity); // change to severity
	//5. AlarmType => OperationalMaintenanceType
	pIatsEvent->get_OperationalMaintenanceType(&nTemp);
	atsEvent.OperationalMaintenanceType = (ATSOperationalMaintenanceType )nTemp; // change to OperationalMaintenanceType
	//6. Asset
	pIatsEvent->get_Asset(&bstrTemp);
	CW2A pszAsset(bstrTemp);
	atsEvent.Asset = pszAsset;
	//7. Description
	pIatsEvent->get_Description(&bstrTemp);
	CW2A pszDescription(bstrTemp);
	atsEvent.Description = pszDescription;
	//8. EventID
	pIatsEvent->get_EventID(&bstrTemp);
	CW2A pszEventID(bstrTemp);
	atsEvent.EventID = pszEventID;
	//9. EventType
	pIatsEvent->get_EventType(&bstrTemp);
	CW2A pszEventType(bstrTemp);
	atsEvent.EventType = pszEventType;
	//10. Location
	pIatsEvent->get_Location(&bstrTemp);
	CW2A pszLocation(bstrTemp);
	atsEvent.Location = pszLocation;
	//11. Operator
	pIatsEvent->get_Operator(&bstrTemp);
	CW2A pszOperator(bstrTemp);
	atsEvent.Operator = pszOperator;
	//12. SubsystemName
	pIatsEvent->get_SubsystemName(&bstrTemp);
	CW2A pszSubsystemName(bstrTemp);
	atsEvent.SubsystemName = pszSubsystemName;
	//13. SystemName
	pIatsEvent->get_SystemName(&bstrTemp);
	CW2A pszSystemName(bstrTemp);
	atsEvent.SystemName = pszSystemName;
	//14. Value
	pIatsEvent->get_Value(&bstrTemp);
	CW2A pszValue(bstrTemp);
	atsEvent.Value = pszValue;
	//15. TimeStamp
	IATSDateTime * pITempDate;
	pIatsEvent->GetTimeStamp(&pITempDate);
	IATSType2ATSType(pITempDate, atsEvent.Timestamp);
	pITempDate->Release();
	
	//16. support std::string AlarmType // new attribute
	pIatsEvent->get_AlarmType(&bstrTemp);
	CW2A pszAlarmType(bstrTemp);
	atsEvent.AlarmType = pszAlarmType;

	//17. support AlarmState // new attribute -> ver 2
	pIatsEvent->get_AlarmState(&nTemp);
	atsEvent.AlarmState = (ATSEventAlarmState)nTemp;

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEvent->ATSEvent) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSFilterCriteria* pIatsFilterCriteria, IATSEventsAPI::ATSFilterCriteria& atsFilterCriteria)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSFilterCriteria->ATSFilterCriteria) entry");

	if(pIatsFilterCriteria == NULL)
	{
		LOG4CXX_TRACE(m_pLogger, "E_INVALIDARG | pIatsFilterCriteria == NULL");
		return E_INVALIDARG;
	}
	IATSIntList * pTempIntList = NULL;
	IATSStringList *pTempStrList = NULL;
	IATSDateTime *pTempDate = NULL;
	VARIANT_BOOL bTempBOOL = VARIANT_FALSE;
	LONG nMAX;
	
	//1. AlarmAvalancheTypeEqualOr
	atsFilterCriteria.AlarmAvalancheTypeEqualOr.clear();
	pIatsFilterCriteria->GetAlarmAvalancheTypeEqualOr(&pTempIntList);
	pTempIntList->GetSize(&nMAX);
	for(int nIndex = 0; nIndex < nMAX; nIndex++)
	{
		LONG nTemp = 0;
		pTempIntList->GetItem(nIndex, &nTemp);
		atsFilterCriteria.AlarmAvalancheTypeEqualOr.push_back((const IATSEventsAPI::ATSEventAlarmAvalancheType)nTemp);
	}
	pTempIntList->Release();

	//2. AlarmIDEqualOr
	atsFilterCriteria.AlarmIDEqualOr.clear();
	pIatsFilterCriteria->GetAlarmIDEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.AlarmIDEqualOr);
	pTempStrList->Release();

	//3. AlarmStateEqualOr
	atsFilterCriteria.AlarmStateEqualOr.clear();
	pIatsFilterCriteria->GetAlarmStateEqualOr(&pTempIntList);
	pTempIntList->GetSize(&nMAX);
	for(int nIndex = 0; nIndex < nMAX; nIndex++)
	{
		LONG nTemp = 0;
		pTempIntList->GetItem(nIndex, &nTemp);
		atsFilterCriteria.AlarmStateEqualOr.push_back((const IATSEventsAPI::ATSEventAlarmState)nTemp);
	}
//	IATSType2ATSType(pTempIntList, atsFilterCriteria.AlarmStateEqualOr); 
	pTempIntList->Release();

	//4. AlarmSeverityEqualOr // -> SeverityEqualOr
	atsFilterCriteria.SeverityEqualOr.clear();
	pIatsFilterCriteria->GetAlarmSeverityEqualOr(&pTempIntList);
	IATSType2ATSType(pTempIntList, atsFilterCriteria.SeverityEqualOr); 
	pTempIntList->Release();

	//5. AlarmTypeEqualOr // OperationalMaintenanceTypeEqualOr
	atsFilterCriteria.OperationalMaintenanceTypeEqualOr.clear();
	pIatsFilterCriteria->GetOperationalMaintenanceTypeEqualOr(&pTempIntList);
	pTempIntList->GetSize(&nMAX);
	for(int nIndex = 0; nIndex < nMAX; nIndex++)
	{
		LONG nTemp = 0;
		pTempIntList->GetItem(nIndex, &nTemp);
		atsFilterCriteria.OperationalMaintenanceTypeEqualOr.push_back((const IATSEventsAPI::ATSOperationalMaintenanceType)nTemp);
	}
//	IATSType2ATSType(pTempIntList, atsFilterCriteria.AlarmTypeEqualOr); 
	pTempIntList->Release();

	//6. AssetEqualOr // -> AssetLikeOr
	atsFilterCriteria.AssetLikeOr.clear();
	pIatsFilterCriteria->GetAssetEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.AssetLikeOr);
	pTempStrList->Release();

	//7. EndTimePeriod
	pIatsFilterCriteria->GetEndTimePeriod(&pTempDate);
	IATSType2ATSType(pTempDate, atsFilterCriteria.EndTimePeriod);
	pTempDate->Release();

	//8. EndTimePeriodDefined
	pIatsFilterCriteria->get_EndTimePeriodDefined(&bTempBOOL);
	atsFilterCriteria.EndTimePeriodDefined = (bTempBOOL == VARIANT_TRUE)? true : false ;

	//9. EventTypeEqualOr
	atsFilterCriteria.EventTypeEqualOr.clear();
	pIatsFilterCriteria->GetEventTypeEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.EventTypeEqualOr);
	pTempStrList->Release();
	
	//10. LocationEqualOr
	atsFilterCriteria.LocationEqualOr.clear();
	pIatsFilterCriteria->GetLocationEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.LocationEqualOr);
	pTempStrList->Release();

	//11. OperatorEqualOr
	atsFilterCriteria.OperatorEqualOr.clear();
	pIatsFilterCriteria->GetOperatorEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.OperatorEqualOr);
	pTempStrList->Release();

	//12. StartTimePeriod
	pIatsFilterCriteria->GetStartTimePeriod(&pTempDate);
	IATSType2ATSType(pTempDate, atsFilterCriteria.StartTimePeriod);
	pTempDate->Release();

	//13. StartTimePeriodDefined
	pIatsFilterCriteria->get_StartTimePeriodDefined(&bTempBOOL);
	atsFilterCriteria.StartTimePeriodDefined = (bTempBOOL == VARIANT_TRUE)? true : false;

	//14. SubsystemNameEqualOr
	atsFilterCriteria.SubsystemNameEqualOr.clear();
	pIatsFilterCriteria->GetSubsystemNameEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.SubsystemNameEqualOr);
	pTempStrList->Release();

	//15. SystemNameEqualOr
	atsFilterCriteria.SystemNameEqualOr.clear();
	pIatsFilterCriteria->GetSystemNameEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.SystemNameEqualOr);
	pTempStrList->Release();

	//16. ValueEqualOr
	atsFilterCriteria.ValueEqualOr.clear();
	pIatsFilterCriteria->GetValueEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.ValueEqualOr);
	pTempStrList->Release();

	//17. DescriptionEqualOr // -> DescriptionLikeOr
	pIatsFilterCriteria->GetDescriptionEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.DescriptionLikeOr);
	pTempStrList->Release();

	//18. AlarmTypeEqualOr
	pIatsFilterCriteria->GetAlarmTypeEqualOr(&pTempStrList);
	IATSType2ATSType(pTempStrList, atsFilterCriteria.AlarmTypeEqualOr);
	pTempStrList->Release();

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSFilterCriteria->ATSFilterCriteria) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSEventSystemList* pIatsEventSystems, std::list<IATSEventsAPI::ATSEventSystem>& atsEventSystems)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventSystemList->std::list<ATSEventSystem>) entry");

	if(pIatsEventSystems == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_INVALIDARG | pIatsEventSystems == NULL");
		return E_INVALIDARG;
	}

	LONG lMax;
	pIatsEventSystems->GetSize(&lMax);

	for(int nIndex = 0; nIndex < lMax; nIndex++)
	{
		IATSEventsAPI::ATSEventSystem atsEventSystem;
		IATSEventSystem * pEventSystem = NULL;

		pIatsEventSystems->GetItem(nIndex, &pEventSystem);
		IATSType2ATSType(pEventSystem, atsEventSystem);

		atsEventSystems.push_back(atsEventSystem);
		pEventSystem->Release();
	}

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSEventSystemList->std::list<ATSEventSystem>) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSStringList* pIatsStringList, std::list<std::string>& strList)
{
	LOG4CXX_TRACE(m_pLogger,  "IATSType2ATSType(IATSStringList->std::list<std::string>) entry");

	if(pIatsStringList == NULL)
	{
		LOG4CXX_ERROR(m_pLogger, "E_INVALIDARG | pIatsStringList == NULL");
		return E_INVALIDARG;
	}

	LONG lMax;
	pIatsStringList->GetSize(&lMax);

	for(int nIndex = 0; nIndex < lMax; nIndex ++)
	{
		CComBSTR bstrTemp;
		std::string strTemp;

		pIatsStringList->GetItem(nIndex, &bstrTemp);
		CW2A pszConverted(bstrTemp);
		strTemp =  pszConverted;
		strList.push_back(strTemp);
	}

	LOG4CXX_TRACE(m_pLogger,  "IATSType2ATSType(IATSStringList->std::list<std::string>) exit");
	return S_OK;
}

HRESULT CConvert::IATSType2ATSType(IATSIntList* pIatsIntList, std::list<short int>& intList)
{
	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSIntList->std::list<short int>) entry");

	if(pIatsIntList == NULL)
	{
		LOG4CXX_ERROR(m_pLogger,  "E_INVALIDARG | pIatsIntList == NULL");
		return E_INVALIDARG;
	}

	LONG lMax;
	pIatsIntList->GetSize(&lMax);

	for(int nIndex = 0; nIndex < lMax; nIndex++)
	{
		LONG ldata;
		pIatsIntList->GetItem(nIndex, &ldata);
		intList.push_back((short int)ldata);
	}

	LOG4CXX_TRACE(m_pLogger, "IATSType2ATSType(IATSIntList->std::list<short int>) exit");
	return S_OK;
}


// Converting Object to String //

std::string CConvert::ObjectToString(IATSEventList* pAtsEventList)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEventList entry");
	std::string strResult = "";
	if(NULL == pAtsEventList)
	{
		return strResult;
	}

	LONG lSize = 0;
	pAtsEventList->GetSize(&lSize);

	for(int nIndex = 0; nIndex < lSize; nIndex++)
	{
		IATSEvent* patsTempEvent = NULL;
		pAtsEventList->GetItem(nIndex, &patsTempEvent);
		
		std::string strTemp = ObjectToString(patsTempEvent);
		
		if (!strTemp.empty())
		{
			strResult += strTemp;
			strResult += "\n";
		}
		patsTempEvent->Release();
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEventList exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSDateTime* pIatsDateTime)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSDateTime entry");
	std::string strResult = "";
	if (NULL == pIatsDateTime)
	{
		return strResult;
	}

	SHORT day;
	SHORT mon;
	SHORT year;
	SHORT hour;
	SHORT min;
	SHORT sec;
	LONG nanosec;

	pIatsDateTime->get_Day(&day);
	pIatsDateTime->get_Month(&mon);
	pIatsDateTime->get_Year(&year);
	pIatsDateTime->get_Hour(&hour);
	pIatsDateTime->get_Minute(&min);
	pIatsDateTime->get_Second(&sec);
	pIatsDateTime->get_Nanosecond(&nanosec);
	
	char pszData[256];	
	sprintf(pszData, "Date: %d-%d-%d || Time: %d:%d:%d:%ld", day, mon, year, hour, min, sec, nanosec);
	
	strResult = pszData;
	
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSDateTime exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEventSystem* pIatsEventSystem)
{
	LOG4CXX_TRACE(m_pLogger,  "ObjectToString -> IATSEventSystem entry");
	std::string strResult = "";
	if (NULL == pIatsEventSystem)
	{
		return strResult;
	}

	CComBSTR bstrVal;
	IATSStringList* pAtsStringList = NULL;

	pIatsEventSystem->get_Name(&bstrVal);
	pIatsEventSystem->GetSubSystemList(&pAtsStringList);

	LONG lSize;
	pAtsStringList->GetSize(&lSize);
	CW2A pszConverted(bstrVal);

	strResult = "EventSystem Name: " + std::string(pszConverted);

	for (int index = 0; index < lSize; index ++)
	{
		CComBSTR bstrTemp;
		pAtsStringList->GetItem(index, &bstrTemp);

		if (bstrTemp.Length() != 0)
		{
			strResult += "| SubSystem: ";
			CW2A pszConverted(bstrTemp);
			strResult += pszConverted;
		}

		bstrTemp.Empty();
	}

	LOG4CXX_TRACE(m_pLogger,  "ObjectToString -> IATSEventSystem exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEvent* pIatsEvent)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEvent entry");

	CComBSTR bstrTemp;
	SHORT shortTemp;
	IATSDateTime * pAtsTempDate = NULL;

	std::string strResult = "";

	if (pIatsEvent == NULL)
	{
		return strResult;
	}

	pIatsEvent->get_EventID(&bstrTemp);
	strResult = "EventID: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_EventType(&bstrTemp);
	strResult = "EventType: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->GetTimeStamp(&pAtsTempDate);
	strResult += std::string("DateTime: ") + ObjectToString(pAtsTempDate);
	pAtsTempDate->Release();

	pIatsEvent->get_Asset(&bstrTemp);
	strResult += std::string("Asset: " + std::string(CW2A(bstrTemp)));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_Description(&bstrTemp);
	strResult += std::string("Description: " + std::string(CW2A(bstrTemp)));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_Operator(&bstrTemp);
	strResult += std::string("Operator: " + std::string(CW2A(bstrTemp)));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_Location(&bstrTemp);
	strResult = "get_Location: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_Value(&bstrTemp);
	strResult = "Value: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_SystemName(&bstrTemp);
	strResult = "SystemName: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_SubsystemName(&bstrTemp);
	strResult = "SubsystemName: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	pIatsEvent->get_AlarmID(&bstrTemp);
	strResult = "AlarmID: " + std::string(CW2A(bstrTemp));
	strResult += " | ";
	bstrTemp.Empty();

	//	pIatsEvent->get_AlarmType(&shortTemp);
	pIatsEvent->get_OperationalMaintenanceType(&shortTemp);
	strResult += std::string("OperationalMaintenanceType: " + ObjectToString(shortTemp));
	strResult += " | ";
	shortTemp = 0;

	pIatsEvent->get_AlarmMMSState(&shortTemp);
	strResult += std::string("AlarmMMSState: " + ObjectToString(shortTemp));
	strResult += " | ";
	shortTemp = 0;

	pIatsEvent->get_AlarmAvalancheType(&shortTemp);
	strResult += std::string("AlarmAvalancheType: " + ObjectToString(shortTemp));
	strResult += " | ";
	shortTemp = 0;

	pIatsEvent->get_AlarmSeverity(&shortTemp);
	strResult += std::string("AlarmSeverity: " + ObjectToString(shortTemp));
	shortTemp = 0;

	//++ new attribute
	pIatsEvent->get_AlarmType(&bstrTemp);
	strResult += std::string("AlarmType: " + std::string(CW2A(bstrTemp)));
	strResult += " | ";
	bstrTemp.Empty();

	//++ new attribute
	pIatsEvent->get_AlarmState(&shortTemp);
	strResult += std::string("AlarmState: " + ObjectToString(shortTemp));
	shortTemp = 0;

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEvent exit");
	return strResult;
}  

std::string CConvert::ObjectToString(IATSFilterCriteria* pAtsfilterCriteria)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSFilterCriteria entry");
	std::string strResult = "";
	if (pAtsfilterCriteria == NULL)
	{
		return strResult;
	}
	VARIANT_BOOL bTempBool;
	IATSDateTime * pAtsDateTime = NULL;
	IATSStringList * pAtsStringList = NULL;
	IATSIntList * pAtsIntList = NULL;

	pAtsfilterCriteria->get_StartTimePeriodDefined(&bTempBool);
	strResult += std::string(" StartTimePeriodDefined: ");
	strResult += std::string((bTempBool==VARIANT_TRUE)?"TRUE \n":"FALSE \n");
	if (bTempBool == VARIANT_TRUE)
	{
		pAtsfilterCriteria->GetStartTimePeriod(&pAtsDateTime);
		if (pAtsDateTime != NULL)
		{
			strResult += (std::string("StartTimePeriod: ") + ObjectToString(pAtsDateTime) + std::string("\n"));
			pAtsDateTime->Release();
			pAtsDateTime = NULL;
		}
	}
	bTempBool = VARIANT_FALSE;

	pAtsfilterCriteria->get_EndTimePeriodDefined(&bTempBool);
	strResult += (std::string("EndTimePeriodDefined: ") + std::string(bTempBool==VARIANT_TRUE?"TRUE \n":"FALSE \n"));
	if (bTempBool == VARIANT_TRUE)
	{
		pAtsfilterCriteria->GetEndTimePeriod(&pAtsDateTime);
		if (NULL != pAtsDateTime)
		{
			strResult += (std::string(" EndTimePeriod: ") + ObjectToString(pAtsDateTime) + std::string("\n"));
			pAtsDateTime->Release();
			pAtsDateTime = NULL;
		}
	}
	bTempBool = VARIANT_FALSE;

	// EventTypeEqualOr
	pAtsfilterCriteria->GetEventTypeEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("EventTypeEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// AssetLikeOr
	pAtsfilterCriteria->GetAssetEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("AssetLikeOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// DescriptionEqualOr
	pAtsfilterCriteria->GetDescriptionEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string(" DescriptionEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// OperatorEqualOr
	pAtsfilterCriteria->GetOperatorEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("OperatorEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// LocationEqualOr
	pAtsfilterCriteria->GetLocationEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("LocationEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// ValueEqualOr
	pAtsfilterCriteria->GetValueEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("ValueEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// SystemNameEqualOr
	pAtsfilterCriteria->GetSystemNameEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("SystemNameEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// SubsystemNameEqualOr
	pAtsfilterCriteria->GetSubsystemNameEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("SubsystemNameEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// AlarmIDEqualOr
	pAtsfilterCriteria->GetAlarmIDEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("AlarmIDEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	// OperationalMaintenanceType
	pAtsfilterCriteria->GetOperationalMaintenanceTypeEqualOr(&pAtsIntList);
	if (pAtsIntList)
	{
		strResult += (std::string("OperationalMaintenanceType: ") + ObjectToString(pAtsIntList) + std::string("\n"));
		pAtsIntList->Release();
		pAtsIntList = NULL;
	}

	// AlarmStateEqualOr
	pAtsfilterCriteria->GetAlarmStateEqualOr(&pAtsIntList);
	if (pAtsIntList)
	{
		strResult += (std::string("AlarmStateEqualOr: ") + ObjectToString(pAtsIntList) + std::string("\n"));
		pAtsIntList->Release();
		pAtsIntList = NULL;
	}

	// AlarmAvalancheTypeEqualOr
	pAtsfilterCriteria->GetAlarmAvalancheTypeEqualOr(&pAtsIntList);
	if (pAtsIntList)
	{
		strResult += (std::string("AlarmAvalancheTypeEqualOr: ") + ObjectToString(pAtsIntList) + std::string("\n"));
		pAtsIntList->Release();
		pAtsIntList = NULL;
	}

	// AlarmSeverityEqualOr
	pAtsfilterCriteria->GetAlarmSeverityEqualOr(&pAtsIntList);
	if (pAtsIntList)
	{
		strResult += (std::string("AlarmSeverityEqualOr: ") + ObjectToString(pAtsIntList) + std::string("\n"));
		pAtsIntList->Release();
		pAtsIntList = NULL;
	}

	// AlarmTypeEqualOr
	pAtsfilterCriteria->GetAlarmTypeEqualOr(&pAtsStringList);
	if (pAtsStringList)
	{
		strResult += (std::string("AlarmTypeEqualOr: ") + ObjectToString(pAtsStringList) + std::string("\n"));
		pAtsStringList->Release();
		pAtsStringList = NULL;
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSFilterCriteria exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEventSystemList* pIatsEventSystems)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEventSystemList entry");
	std::string strResult = "";
	LONG lSize;

	if (pIatsEventSystems == NULL)
	{
		return strResult;
	}

	pIatsEventSystems->GetSize(&lSize);

	strResult = "EventSystems : ";
	for (int nIndex =0; nIndex < lSize; nIndex++)
	{
		IATSEventSystem * pAtsEventSystem = NULL;
		pIatsEventSystems->GetItem(nIndex, &pAtsEventSystem);

		if (pAtsEventSystem)
		{
			strResult += ObjectToString(pAtsEventSystem);
			pAtsEventSystem->Release();
		}
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSEventSystemList exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSStringList* pIatsStringList)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSStringList entry");
	std::string strResult = "";
	if(pIatsStringList == NULL)
	{
		return strResult;
	}

	LONG lSize = 0;
	pIatsStringList->GetSize(&lSize);

	for (int nIndex = 0; nIndex < lSize; nIndex++)
	{
		CComBSTR bstrTemp;
		pIatsStringList->GetItem(nIndex, &bstrTemp);
		strResult += std::string( CW2A(bstrTemp) );
		strResult += " | ";
		bstrTemp.Empty();
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSStringList exit");	
	return strResult;
}

std::string CConvert::ObjectToString(IATSIntList* pIatsIntList)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSIntList entry");

	std::string strResult = "";
	if (pIatsIntList == NULL)
	{
		return strResult;
	}

	LONG lSize = 0;
	pIatsIntList->GetSize(&lSize);
	
	for (int nIndex = 0; nIndex < lSize; nIndex++)
	{
		LONG lTemp;
		pIatsIntList->GetItem(nIndex, &lTemp);
		strResult += (ObjectToString(lTemp) + std::string(" | "));
		lTemp = 0;
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> IATSIntList exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEventsAPI::ATSDateTime& atsDateTime)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString->ATSDateTime entry");
	std::string strResult = "";
	char szData[50];

	sprintf(szData, "Date: %d-%d-%d | Time: %d:%d:%d:%ld", atsDateTime.Month, atsDateTime.Day, atsDateTime.Year, atsDateTime.Hour, atsDateTime.Minute, atsDateTime.Second, atsDateTime.Nanosecond);
	strResult = szData;

	LOG4CXX_TRACE(m_pLogger, "ObjectToString->ATSDateTime exit");
	return strResult;
}
std::string CConvert::ObjectToString(IATSEventsAPI::ATSEventSystem& atsEventSystem)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSEventSystem entry");
	std::string strResult = "";

	strResult += (std::string("EventSystem Name: ") + atsEventSystem.Name + std::string(" "));
	strResult += (std::string("SubSystemNames: ") + ObjectToString(atsEventSystem.SubsystemNames));

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSEventSystem exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEventsAPI::ATSEvent& atsEvent)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSEvent entry");
	std::string strResult = "";
	
	strResult+= (std::string("EventID: ") + atsEvent.EventID);

	/// The ATS event type (eg System Message, Alarm Acknowlege). Max 25 chars.
	strResult+= (std::string(" EventType: ") + atsEvent.EventType);

	/// Event generation time
	/// Time is local time.
	strResult+= (std::string(" Timestamp: ") + ObjectToString(atsEvent.Timestamp));

	/// The asset name for the equipment associated with the event, if any.  
	/// Max 28 chars.
	strResult+= (std::string(" Asset: ") + atsEvent.Asset);

	/// Event description. Max 70 chars.
	strResult+= (std::string(" Description: ") + atsEvent.Description);

	/// The operator ID responsible for the event, if any. Max 10 chars.
	strResult+= (std::string(" Operator: ") + atsEvent.Operator);

	/// The location where the event was generated.  Max 6 chars.
	strResult+= (std::string(" Location: ") + atsEvent.Location);

	/// The value associated with the event, if any.  Max 10 chars.
	strResult+= (std::string(" Value: ") + atsEvent.Value);

	/// The system associated with the event.  Max 4 chars.
	strResult+= (std::string(" SystemName: ") + atsEvent.SystemName);

	/// The subsystem associated with the event.  Max 6 chars.
	strResult+= (std::string(" SubsystemName: ") + atsEvent.SubsystemName);

	/// If event is associated with alarm, the alarm ID
	strResult+= (std::string(" AlarmID: ") + atsEvent.AlarmID);

	/// If event is associated with alarm, the alarm type
	strResult+= (std::string(" OperationalMaintenanceType: ") + ObjectToString((int)atsEvent.OperationalMaintenanceType)); // change to 

	/// If event is associated with alarm, the alarm's MMS type
	strResult+= (std::string(" AlarmMMSState: ") + ObjectToString((int)atsEvent.AlarmMMSState));

	/// If event is associated with alarm, the alarm's avalanche setting
	strResult+= (std::string(" AlarmAvalancheType: ") + ObjectToString((int)atsEvent.AlarmAvalancheType));

	/// If event is associated with alarm, the alarm severity
	strResult+= (std::string(" AlarmSeverity: ") + ObjectToString((int)atsEvent.Severity)); // change to severity

	//If event is associated with alarm, the alarm type
	strResult+= (std::string(" AlarmType: ") + atsEvent.AlarmType);

	//++new attribute
	strResult+= (std::string(" AlarmState: ") + ObjectToString((int)atsEvent.AlarmState));

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSEvent exit");
	return strResult;
}

std::string CConvert::ObjectToString(IATSEventsAPI::ATSFilterCriteria& atsFilterCriteria)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSFilterCriteria entry ");
	std::string strResult = "";

	strResult += (std::string("StartTimePeriodDefined: ") + std::string(atsFilterCriteria.StartTimePeriodDefined?"TRUE \n":"FALSE \n"));
	strResult += (std::string("StartTimePeriod: ") + ObjectToString(atsFilterCriteria.StartTimePeriod) +  std::string("\n"));

	strResult += (std::string("EndTimePeriodDefined: ") + std::string(atsFilterCriteria.EndTimePeriodDefined?"TRUE \n":"FALSE \n"));
	strResult += (std::string("EndTimePeriod: ") + ObjectToString(atsFilterCriteria.EndTimePeriod) +  std::string("\n"));

	strResult += (std::string("EventTypeEqualOr: ") + ObjectToString(atsFilterCriteria.EventTypeEqualOr) + std::string("\n"));
	strResult += (std::string("AssetLikeOr: ") + ObjectToString(atsFilterCriteria.AssetLikeOr) + std::string("\n"));
	strResult += (std::string("DescriptionLikeOr: ") + ObjectToString(atsFilterCriteria.DescriptionLikeOr) + std::string("\n"));
	strResult += (std::string("OperatorEqualOr: ") + ObjectToString(atsFilterCriteria.OperatorEqualOr) + std::string("\n"));
	strResult += (std::string("LocationEqualOr: ") + ObjectToString(atsFilterCriteria.LocationEqualOr) + std::string("\n"));
	strResult += (std::string("ValueEqualOr: ") + ObjectToString(atsFilterCriteria.ValueEqualOr) + std::string("\n"));
	strResult += (std::string("SystemNameEqualOr: ") + ObjectToString(atsFilterCriteria.SystemNameEqualOr) + std::string("\n"));
	strResult += (std::string("SubsystemNameEqualOr: ") + ObjectToString(atsFilterCriteria.SubsystemNameEqualOr) + std::string("\n"));
	strResult += (std::string("AlarmIDEqualOr: ") + ObjectToString(atsFilterCriteria.AlarmIDEqualOr) + std::string("\n"));
	
	/// Filter by OperationalMaintenanceTypeEqualOr
	strResult += std::string("OperationalMaintenanceTypeEqualOr: ");
	std::list<ATSOperationalMaintenanceType>::iterator it; //AlarmTypeEqualOr;
	for (it = atsFilterCriteria.OperationalMaintenanceTypeEqualOr.begin(); it != atsFilterCriteria.OperationalMaintenanceTypeEqualOr.end(); it++)
	{
		char szData[10];
		sprintf(szData, "%d | ", (int)(*it));
		strResult += std::string(szData);
	}
	strResult += std::string("\n");

	/// Filter by AlarmState //++ ver 2 change
	strResult += std::string("AlarmStateEqualOr: ");
	std::list<ATSEventAlarmState>::iterator it1; //AlarmStateEqualOr;
	for (it1 = atsFilterCriteria.AlarmStateEqualOr.begin(); it1 != atsFilterCriteria.AlarmStateEqualOr.end(); it1++)
	{
		char szData[10];
		sprintf(szData, "%d | ", (int)(*it1));
		strResult += std::string(szData);
	}
	strResult += std::string("\n");

	/// Filter by AlarmAvalancheType
	strResult += std::string("AlarmAvalancheTypeEqualOr: ");
	std::list<ATSEventAlarmAvalancheType>::iterator it2; //AlarmTypeEqualOr;
	for (it2 = atsFilterCriteria.AlarmAvalancheTypeEqualOr.begin(); it2 != atsFilterCriteria.AlarmAvalancheTypeEqualOr.end(); it2++)
	{
		char szData[10];
		sprintf(szData, "%d | ", (int)(*it2));
		strResult += std::string(szData);
	}
	strResult += std::string("\n");

	strResult += ((std::string("SeverityEqualOr: ") + ObjectToString(atsFilterCriteria.SeverityEqualOr) + std::string("\n")));
	strResult += (std::string("AlarmTypeEqualOr: ") + ObjectToString(atsFilterCriteria.AlarmTypeEqualOr) + std::string("\n"));

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> ATSFilterCriteria exit ");
	return strResult;
}

std::string CConvert::ObjectToString(std::list<IATSEventsAPI::ATSEvent*>& atsEvents)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<IATSEventsAPI::ATSEvent*> entry ");
	std::string strResult = "";
	std::list<IATSEventsAPI::ATSEvent*>::iterator it;

	for (it = atsEvents.begin(); it != atsEvents.end(); it++)
	{
		IATSEventsAPI::ATSEvent* atsTemp = NULL;
		atsTemp = (*it);
		strResult += (std::string("Event: ") + ObjectToString(*atsTemp) + std::string("\n"));
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<IATSEventsAPI::ATSEvent*> exit");
	return strResult;
}

std::string CConvert::ObjectToString(std::list<IATSEventsAPI::ATSEventSystem>& atsEventSystems)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<IATSEventsAPI::ATSEventSystem> entry");
	std::string strResult = "";
	std::list<IATSEventsAPI::ATSEventSystem>::iterator it;

	for (it = atsEventSystems.begin(); it != atsEventSystems.end(); it++)
	{
		strResult += (std::string("Name: ") + (*it).Name);
		strResult += (ObjectToString((*it).SubsystemNames) + std::string("\n"));;
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<IATSEventsAPI::ATSEventSystem> exit");
	return strResult;
}

std::string CConvert::ObjectToString(std::list<std::string>& strList)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<std::string> entry");

	std::string strResult = "";
	std::list<std::string>::iterator it;

	for (it = strList.begin(); it != strList.end(); it++)
	{
		strResult += ((*it) + std::string(" | "));
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<std::string> exit");
	return strResult;
}

std::string CConvert::ObjectToString(std::list<short int>& intList)
{
	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<short int> entry");
	std::string strResult = "";
	
	std::list<short int>::iterator it;
	for (it = intList.begin(); it != intList.end(); it++)
	{
		char szData[10];
		sprintf(szData, "%d |", (*it));
		strResult += std::string(szData);
	}

	LOG4CXX_TRACE(m_pLogger, "ObjectToString -> std::list<short int> exit");
	return strResult;
}
