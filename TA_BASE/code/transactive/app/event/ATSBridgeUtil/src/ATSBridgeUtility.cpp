// ATSBridgeUtility.cpp : Implementation of CATSBridgeUtility

#include "stdafx.h"
#include "ATSBridgeUtility.h"

using namespace IATSEventsAPI;

typedef std::list<IATSEventsAPI::ATSEvent*> STD_ATSEvents;

// CATSBridgeUtility
LoggerPtr CATSBridgeUtility::m_logger(Logger::getLogger("CATSBridgeUtility"));

STDMETHODIMP CATSBridgeUtility::DoForwardPageQuery(IATSFilterCriteria* filterCriteria, BSTR atsEventID, LONG length, IATSEventList** pAtsEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	USES_CONVERSION;
	LOG4CXX_INFO(m_logger, "DoForwardPageQuery entry");

	if(filterCriteria == NULL || pAtsEvents == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | filterCriteria == NULL || pAtsEvents == NULL");
		return E_POINTER;
	}

	IATSEventsAPI::ATSQueryError atsQError;
	IATSEventsAPI::ATSFilterCriteria atsFilterCritera;
	STD_ATSEvents stdEventList;

	try {

		m_objConvert.IATSType2ATSType(filterCriteria, atsFilterCritera);
		
		std::string strEventID;
		CW2A pszConvert(atsEventID);
		strEventID = pszConvert;
		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(filterCriteria));
		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCritera));
		LOG4CXX_INFO(m_logger, "EventID = " << strEventID << " | Lenght = " << length);
		
		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoForwardPageQuery");
		
		atsQError = IATSEventsAPI::DoForwardPageQuery(&atsFilterCritera, strEventID, (unsigned int)length, stdEventList);
		
		*pRetVal = (LONG)atsQError;

		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoForwardPageQuery | Result: " << (int)atsQError);
		}
		else 
		{
			LOG4CXX_TRACE(m_logger, "Query Results: \n" << m_objConvert.ObjectToString(stdEventList));
			
			m_objConvert.ATSType2IATSType(stdEventList, pAtsEvents);

			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoForwardPageQuery | Result: " << (int)atsQError <<" | EventList size: " << stdEventList.size());
		}

		if(stdEventList.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(stdEventList);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger,  "Exception Error Ocured!");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "DoForwardPageQuery exit");
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoForwardPageQueryDate(IATSFilterCriteria* filterCriteria, IATSDateTime* startTime, LONG length, IATSEventList** atsEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	USES_CONVERSION;
	LOG4CXX_INFO(m_logger, "DoForwardPageQueryDate entry" );

	if(atsEvents == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | atsEvents == NULL");
		return E_POINTER;
	}
	if(filterCriteria == NULL || startTime == NULL)
	{
		LOG4CXX_ERROR(m_logger,  "E_INVALIDARG | filterCriter == NULL || startTime == NULL");
		return E_INVALIDARG;
	}

	IATSEventsAPI::ATSQueryError atsQError;
	IATSEventsAPI::ATSFilterCriteria atsFilterCritera;
	IATSEventsAPI::ATSDateTime atsStartTime;
	STD_ATSEvents stdEventList;

	try {
		m_objConvert.IATSType2ATSType(filterCriteria, atsFilterCritera);
		m_objConvert.IATSType2ATSType(startTime, atsStartTime);

		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(filterCriteria));
		LOG4CXX_INFO(m_logger, "StartTime | "<< m_objConvert.ObjectToString(startTime) << " | Lenght " << length );

		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCritera));
		LOG4CXX_INFO(m_logger, "Converted StartTime | "<< m_objConvert.ObjectToString(atsStartTime) << " | Lenght " << length );

		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoForwardPageQuery");

		atsQError = IATSEventsAPI::DoForwardPageQuery(&atsFilterCritera, atsStartTime, (unsigned int)length, stdEventList);
		*pRetVal = (LONG)atsQError;

		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoForwardPageQuery | Result: " << (int)atsQError);
		}
		else
		{
			LOG4CXX_TRACE(m_logger, "Query Results: \n" << m_objConvert.ObjectToString(stdEventList));
			m_objConvert.ATSType2IATSType(stdEventList, atsEvents);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoForwardPageQuery | Result: "<< (int)atsQError <<" | EventList size: " << stdEventList.size()); 
		}

		if(stdEventList.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(stdEventList);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception error Ocured!");
		return E_FAIL;
	}
	LOG4CXX_INFO(m_logger, "DoForwardPageQueryDate exit");
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoPreviousPageQuery(IATSFilterCriteria* pfilterCriteria, BSTR atsEventID, LONG length, IATSEventList** pATSEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	USES_CONVERSION;
	LOG4CXX_INFO(m_logger, "DoPreviousPageQuery entry");

	if(pATSEvents == NULL)	
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pATSEvents == NULL");
		return E_POINTER;
	}

	if(pfilterCriteria == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_INVALIDARG | pfilterCriteria == NULL");
		return E_INVALIDARG;
	}

	IATSEventsAPI::ATSQueryError atsQError;
	IATSEventsAPI::ATSFilterCriteria atsFilterCritera;
	STD_ATSEvents stdEventList;

	try {
		m_objConvert.IATSType2ATSType(pfilterCriteria, atsFilterCritera);
		
		std::string strEventID;
		CW2A pszConvert(atsEventID);
		strEventID = pszConvert;
		
		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(pfilterCriteria));
		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCritera));

		LOG4CXX_INFO(m_logger, "ATSEventID = "<< strEventID << " | Length = " << length); 
		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoPreviousPageQuery");

		atsQError = IATSEventsAPI::DoPreviousPageQuery(&atsFilterCritera, strEventID, (unsigned int)length, stdEventList);
		*pRetVal = (LONG)atsQError;
		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoPreviousPageQuery | Result: " << (int)atsQError);
		}
		else
		{
			LOG4CXX_TRACE(m_logger, "Query Results: \n" << m_objConvert.ObjectToString(stdEventList));
			m_objConvert.ATSType2IATSType(stdEventList, pATSEvents);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoPreviousPageQuery | Result: "<< (int)atsQError <<" | EventList size: "<< stdEventList.size()); 
		}
	
		if(stdEventList.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(stdEventList);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	}
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception error Ocured!");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "DoPreviousPageQuery exit");
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoPreviousPageQueryDate(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pEndTime, LONG length, IATSEventList** pATSEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	USES_CONVERSION;
	LOG4CXX_INFO(m_logger, "DoPreviousPageQueryDate entry" );

	if(pATSEvents == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pATSEvents == NULL");
		return E_POINTER;
	}

	if(pfilterCriteria == NULL || pEndTime == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_INVALIDARG | pfilterCriteria == NULL || pEndTime == NULL");
		return E_INVALIDARG;
	}

	IATSEventsAPI::ATSQueryError atsQError;
	IATSEventsAPI::ATSFilterCriteria atsFilterCritera;
	IATSEventsAPI::ATSDateTime atsEndTime;
	STD_ATSEvents stdEventList;

	try {
		m_objConvert.IATSType2ATSType(pfilterCriteria, atsFilterCritera);
		m_objConvert.IATSType2ATSType(pEndTime, atsEndTime);
		
		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(pfilterCriteria));
		LOG4CXX_INFO(m_logger, "EndTime: "<< m_objConvert.ObjectToString(pEndTime) <<" | Lenght: " << length );
		
		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCritera));
		LOG4CXX_INFO(m_logger, "Converted EndTime: "<< m_objConvert.ObjectToString(atsEndTime) <<" | Lenght: " << length );

		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoPreviousPageQuery");
		atsQError = IATSEventsAPI::DoPreviousPageQuery(&atsFilterCritera, atsEndTime, (unsigned int)length, stdEventList);
		*pRetVal = (LONG)atsQError;

		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoPreviousPageQuery | Result: " << (int)atsQError);
		}
		else
		{
			LOG4CXX_TRACE(m_logger, "Query Results: \n" << m_objConvert.ObjectToString(stdEventList));
			m_objConvert.ATSType2IATSType(stdEventList, pATSEvents);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoPreviousPageQuery | Result: "<< (int)atsQError <<" | EventList size: " << stdEventList.size()); 
		}	

		if(stdEventList.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(stdEventList);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	}
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception error Ocured!");
		return E_FAIL;
	}
	LOG4CXX_INFO(m_logger, "DoPreviousPageQueryDate exit");
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoStartTimeEndTimeQuery(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pStartTime, IATSDateTime* pEndTime, IATSEventList** pATSEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	USES_CONVERSION;
	LOG4CXX_INFO(m_logger, "DoStartTimeEndTimeQuery entry" );

	if(pATSEvents == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pATSEvents == NULL");
		return E_POINTER;
	}
	if(pfilterCriteria == NULL || pStartTime == NULL || pEndTime == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_INVALIDARG | pfilterCriteria == NULL || pStartTime == NULL || pEndTime == NULL");
		return E_INVALIDARG;
	}

	IATSEventsAPI::ATSQueryError atsQError;
	IATSEventsAPI::ATSFilterCriteria atsFilterCriteria;
	IATSEventsAPI::ATSDateTime atsEndTime;
	IATSEventsAPI::ATSDateTime atsStartTime;
	STD_ATSEvents stdEventList;

	try 
	{
		m_objConvert.IATSType2ATSType(pfilterCriteria, atsFilterCriteria);
		m_objConvert.IATSType2ATSType(pEndTime, atsEndTime);
		m_objConvert.IATSType2ATSType(pStartTime, atsStartTime);

		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(pfilterCriteria));
		LOG4CXX_INFO(m_logger, "StartTime: " << m_objConvert.ObjectToString(pStartTime))
		LOG4CXX_INFO(m_logger,	"EndTime: " << m_objConvert.ObjectToString(pEndTime));
		
		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCriteria));
		LOG4CXX_INFO(m_logger, "Converted StartTime: " << m_objConvert.ObjectToString(atsEndTime))
		LOG4CXX_INFO(m_logger, "Converted EndTime: " << m_objConvert.ObjectToString(atsStartTime));

		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoStartTimeEndTimeQuery");
		atsQError = IATSEventsAPI::DoStartTimeEndTimeQuery(&atsFilterCriteria, atsStartTime, atsEndTime, stdEventList);
		*pRetVal = (LONG)atsQError;
		
		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoStartTimeEndTimeQuery | Result: " << (int)atsQError);
		}
		else
		{
			LOG4CXX_TRACE(m_logger, "Query Results: \n" << m_objConvert.ObjectToString(stdEventList));
			m_objConvert.ATSType2IATSType(stdEventList, pATSEvents);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoStartTimeEndTimeQuery | Result: "<< (int)atsQError <<" | EventList size: " << stdEventList.size()); 
		}	

		if(stdEventList.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(stdEventList);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception Error Ocured!");
		return E_FAIL;
	}
	LOG4CXX_INFO(m_logger, "DoStartTimeEndTimeQuery exit" );
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoStatisticsQuery(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pStartTime, IATSDateTime* pEndTime, LONG* pEventCount, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	LOG4CXX_INFO(m_logger, "DoStatisticsQuery entry" );

	if(pEventCount == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pEventCount == NULL");
		return E_POINTER;
	}
	if(pfilterCriteria == NULL || pStartTime == NULL || pEndTime == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_INVALIDARG | pfilterCriteria == NULL || pStartTime == NULL || pEndTime == NULL");
		return E_INVALIDARG;
	}

	IATSEventsAPI::ATSFilterCriteria atsFilterCriteria;
	IATSEventsAPI::ATSDateTime atsStartTime;
	IATSEventsAPI::ATSDateTime atsEndTime;
	IATSEventsAPI::ATSQueryError atsQError;
	long eventCount;

	try
	{
		m_objConvert.IATSType2ATSType(pfilterCriteria, atsFilterCriteria);
		m_objConvert.IATSType2ATSType(pStartTime, atsStartTime);
		m_objConvert.IATSType2ATSType(pEndTime, atsEndTime);
		LOG4CXX_INFO(m_logger, "Filter Criteria: \n" << m_objConvert.ObjectToString(pfilterCriteria));
		LOG4CXX_INFO(m_logger, "StartTime: " << m_objConvert.ObjectToString(pStartTime))
		LOG4CXX_INFO(m_logger,	"EndTime: " << m_objConvert.ObjectToString(pEndTime));
		
		LOG4CXX_INFO(m_logger, "Converted Filter Criteria: \n" << m_objConvert.ObjectToString(atsFilterCriteria));
		LOG4CXX_INFO(m_logger, "Converted StartTime: " << m_objConvert.ObjectToString(atsStartTime))
		LOG4CXX_INFO(m_logger, "Converted EndTime: " << m_objConvert.ObjectToString(atsEndTime));

		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoStatisticsQuery");
		atsQError = IATSEventsAPI::DoStatisticsQuery(&atsFilterCriteria, atsStartTime, atsEndTime, eventCount);
		*pRetVal = (long)atsQError;

		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoStatisticsQuery | Result: " << (int)atsQError);
		}
		else
		{
			*pEventCount = eventCount;
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoStatisticsQuery | Result: "<< (int)atsQError << " | EventCount: " << eventCount);
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception Error Ocured!");
		return E_FAIL;
	}
	
	LOG4CXX_INFO(m_logger, "DoStatisticsQuery exit");
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoEventInfoQuery(IATSEventSystemList** pEventSystems, IATSStringList** pEventTypes, IATSStringList** pAlarmTypes, LONG* plVal)
{
	// TODO: Add your implementation code here
	LOG4CXX_INFO(m_logger, "DoEventInfoQuery entry");

	if(pEventSystems == NULL || pEventTypes == NULL || pAlarmTypes == NULL || plVal == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pEventSystems == NULL || pEventTypes == NULL || pAlarmTypes == NULL || plVal == NULL");
		return E_POINTER;
	}

	std::list<IATSEventsAPI::ATSEventSystem> atsEventSystems;
	std::list<std::string> atsEventTypes;
	std::list<std::string> atsAlarmTypes;
	IATSEventsAPI::ATSQueryError atsQError;

	try 
	{
		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoEventInfoQuery");
		atsQError = IATSEventsAPI::DoEventInfoQuery(atsEventSystems, atsEventTypes, atsAlarmTypes);
		*plVal = (long)atsQError;

		if(atsQError != ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoEventInfoQuery | Result: " << (int)atsQError);

		}
		else
		{
			m_objConvert.ATSType2IATSType(atsEventSystems, pEventSystems);
			m_objConvert.ATSType2IATSType(atsEventTypes, pEventTypes);
			m_objConvert.ATSType2IATSType(atsAlarmTypes, pAlarmTypes);

			LOG4CXX_TRACE(m_logger, "Query Results EventSystems: " << m_objConvert.ObjectToString(atsEventSystems));
			LOG4CXX_TRACE(m_logger, "Query Results EventTypes: " << m_objConvert.ObjectToString(atsEventTypes));
			LOG4CXX_TRACE(m_logger, "Query Results AlarmTypes: " << m_objConvert.ObjectToString(atsAlarmTypes));

			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoEventInfoQuery | Result: "<< (int)atsQError <<" | EventSystem Size: "<< (int)atsEventSystems.size() <<" | EventType Size: "<< (int)atsEventTypes.size() <<" | AlarmTypes Size: " << (int)atsAlarmTypes.size());
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception Error Occured!");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "DoEventInfoQuery exit"); 
	return S_OK;
}

STDMETHODIMP CATSBridgeUtility::DoAlarmInfoQuery(IATSStringList** pAlarmTypes, LONG* plVal)
{
	// TODO: Add your implementation code here
	LOG4CXX_INFO(m_logger, "DoAlarmInfoQuery entry" );

	if(pAlarmTypes == NULL)
	{
		LOG4CXX_ERROR(m_logger, "E_POINTER | pAlarmTypes == NULL");
		return E_POINTER;
	}

	std::list<std::string> atsAlarmTypes;
	IATSEventsAPI::ATSQueryError atsQError;
	
	try 
	{
		LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::DoAlarmInfoQuery");
		atsQError = IATSEventsAPI::DoAlarmInfoQuery(atsAlarmTypes);
		*plVal = (long)atsQError;

		if(atsQError = ATS_QUERY_OK)
		{
			LOG4CXX_ERROR(m_logger, "- IATSEventsAPI::DoAlarmInfoQuery | Result: " << (int)atsQError);
		}
		else
		{
			LOG4CXX_TRACE(m_logger, "Query Results AlarmTypes: " << m_objConvert.ObjectToString(atsAlarmTypes));
			m_objConvert.ATSType2IATSType(atsAlarmTypes, pAlarmTypes);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::DoAlarmInfoQuery | Result: "<< (int)atsQError <<" | AlarmTypes Size: " << (int)atsAlarmTypes.size());
			
		}
	} 
	catch (...)
	{
		LOG4CXX_ERROR(m_logger, "Exception Error Occured!");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "DoAlarmInfoQuery exit");
	return S_OK;
}

//STDMETHODIMP CATSBridgeUtility::TestInt(IATSIntList * pIntList, IATSIntList ** pIntListRes)
//{
//	// TODO: Add your implementation code here
//	LOG4CXX_INFO(m_logger, "CATSBridgeUtility::TestInt" );
//	TCHAR pszLog[256];
//
//	typedef std::list<short int> STDIntList;
//	STDIntList stdIntList;
//	STDIntList::iterator it;
//
//	HRESULT hr = m_objConvert.IATSType2ATSType(pIntList, stdIntList);
//	if(FAILED(hr))
//	{
//		sprintf(pszLog, "Unable to convert IntList | HRESULT = %ld", hr);
//		LOG4CXX_INFO(m_logger, pszLog);
//		return E_FAIL;
//	}
//
//	int nIndex =1;
//	for(it = stdIntList.begin(); it != stdIntList.end(); it++)
//	{
//		TCHAR pszData[50];
//		sprintf(pszData, _T("Transfered Data %d: %d"), nIndex, (*it));
//		LOG4CXX_INFO(m_logger,  pszData);
//		//MessageBox(NULL, pszData, _T("TEST INT"), MB_OK);
//		nIndex++;
//	}
//
//	m_objConvert.ATSType2IATSType(stdIntList, pIntListRes);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestString(IATSStringList* pStringList, IATSStringList** pStringListRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger,  "CATSBridgeUtility::TestString");
//
//	typedef std::list<std::string> STDString;
//	STDString stdStringList;
//	STDString::iterator it;
//
//	m_objConvert.IATSType2ATSType(pStringList, stdStringList);
//
//	int nIndex =1;
//	for(it = stdStringList.begin(); it != stdStringList.end(); it++)
//	{
//		TCHAR pszData[1028];
//		sprintf(pszData, _T("Converted Data %d: %s"), nIndex, (*it).c_str()); 
//		//MessageBox(NULL, pszData, _T("Test String"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//	}
//
//	m_objConvert.ATSType2IATSType(stdStringList, pStringListRet);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestEvent(IATSEvent* pEvent, IATSEvent** pEventRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger,  "CATSBridgeUtility::TestEvent");
//
//	IATSEventsAPI::ATSEvent atsEvent;
//
//	m_objConvert.IATSType2ATSType(pEvent, atsEvent);
//
//	TCHAR pszData[256];
//	sprintf(pszData, _T("EventID: %s || EventType: %s"), atsEvent.EventID.c_str(), atsEvent.EventType.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//	
//	sprintf(pszData, _T("TimeStamp = Day: %d | Mo: %d | Year: %d"), atsEvent.Timestamp.Day, atsEvent.Timestamp.Month, atsEvent.Timestamp.Year);
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("Asset: %s || Description: %s"), atsEvent.Asset.c_str(), atsEvent.Description.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//	
//	sprintf(pszData, _T("Operator: %s || Location: %s"), atsEvent.Operator.c_str(), atsEvent.Location.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("Value: %s || SystemName: %s"), atsEvent.Value.c_str(), atsEvent.SystemName.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("SubsystemName: %s || AlarmID: %s"), atsEvent.SubsystemName.c_str(), atsEvent.AlarmID.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	m_objConvert.ATSType2IATSType(atsEvent, pEventRet);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestDateTime(IATSDateTime* pDateTime, IATSDateTime** pDateTimeRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger,  "CATSBridgeUtility::TestDateTime");
//
//	IATSEventsAPI::ATSDateTime atsDateTime;
//
//	m_objConvert.IATSType2ATSType(pDateTime, atsDateTime);
//
//	TCHAR pszData[256];
//	sprintf(pszData, _T("TimeStamp = Day: %d | Mo: %d | Year: %d"), atsDateTime.Day, atsDateTime.Month, atsDateTime.Year);
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("TimeStamp = Hour: %d | Minute: %d | Second: %d | NanoSecond: %ld"), atsDateTime.Hour, atsDateTime.Minute, atsDateTime.Second, atsDateTime.Nanosecond);
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	m_objConvert.ATSType2IATSType(atsDateTime, pDateTimeRet);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestEventSystem(IATSEventSystem* pEventSystem, IATSEventSystem** pEventSystemRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger, "CATSBridgeUtility::TestEventSystem");
//
//	IATSEventsAPI::ATSEventSystem atsEventSystem;
//	std::list<std::string>::iterator it;
//
//	m_objConvert.IATSType2ATSType(pEventSystem, atsEventSystem);
//
//	TCHAR pszData[256];
//	sprintf(pszData, _T("EventSystem = Name: %s"), atsEventSystem.Name.c_str());
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//	
//	int nIndex = 1;
//	for(it = atsEventSystem.SubsystemNames.begin(); it != atsEventSystem.SubsystemNames.end(); it++)
//	{
//		sprintf(pszData, _T("SubsystemNames = %d: %s"), nIndex++, (*it).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	m_objConvert.ATSType2IATSType(atsEventSystem, pEventSystemRet);
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestEventList(IATSEventList* pEventList, IATSEventList** pEventListRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger, "CATSBridgeUtility::TestEventList");
//
//	std::list<IATSEventsAPI::ATSEvent*> atsEvents;
//	std::list<IATSEventsAPI::ATSEvent*>::iterator it;
//
//	m_objConvert.IATSType2ATSType(pEventList, atsEvents);
//
//	int nIndex = 1;
//	for(it = atsEvents.begin(); it!= atsEvents.end(); it++)
//	{
//		TCHAR pszData[512];
//		sprintf(pszData, _T("EventList: %d"), nIndex++);
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//		
//		sprintf(pszData, _T("EventID: %s || EventType: %s"), (*it)->EventID.c_str(), (*it)->EventType.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//		
//		sprintf(pszData, _T("TimeStamp = Day: %d | Mo: %d | Year: %d"), (*it)->Timestamp.Day, (*it)->Timestamp.Month, (*it)->Timestamp.Year);
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//
//		sprintf(pszData, _T("Asset: %s || Description: %s"), (*it)->Asset.c_str(), (*it)->Description.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//		
//		sprintf(pszData, _T("Operator: %s || Location: %s"), (*it)->Operator.c_str(), (*it)->Location.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//
//		sprintf(pszData, _T("Value: %s || SystemName: %s"), (*it)->Value.c_str(), (*it)->SystemName.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//
//		sprintf(pszData, _T("SubsystemName: %s || AlarmID: %s"), (*it)->SubsystemName.c_str(), (*it)->AlarmID.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	m_objConvert.ATSType2IATSType(atsEvents, pEventListRet);
//	if(atsEvents.size() != 0)
//		IATSEventsAPI::FreeATSEventList(atsEvents);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestEventSystemList(IATSEventSystemList* pEventSystems, IATSEventSystemList** pEventSystemsRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger, "CATSBridgeUtility::TestEventSystemList");
//
//	std::list<IATSEventsAPI::ATSEventSystem> atsEventSystems;
//	std::list<IATSEventsAPI::ATSEventSystem>::iterator it1;
//	std::list<std::string>::iterator itStr;
//
//	m_objConvert.IATSType2ATSType(pEventSystems, atsEventSystems);
//
//	int nIdx = 1;
//	for(it1 = atsEventSystems.begin(); it1 != atsEventSystems.end(); it1++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("EventSystemList : %d"), nIdx++);
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//
//		sprintf(pszData, _T("EventSystem = Name: %s"), (*it1).Name.c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//		
//		int nIndex = 1;
//		for(itStr = (*it1).SubsystemNames.begin(); itStr != (*it1).SubsystemNames.end(); itStr++)
//		{
//			sprintf(pszData, _T("SubsystemNames = %d: %s"), nIndex++, (*itStr).c_str());
//			//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//			LOG4CXX_INFO(m_logger, pszData);
//			ZeroMemory(pszData, sizeof(pszData));
//		}		
//	}
//
//	m_objConvert.ATSType2IATSType(atsEventSystems, pEventSystemsRet);
//
//	return S_OK;
//}
//
//STDMETHODIMP CATSBridgeUtility::TestFilterCriteria(IATSFilterCriteria* pFilterCriteria, IATSFilterCriteria** pFilterCriteriaRet)
//{
//	// TODO: Add your implementation code here
//	USES_CONVERSION;
//	LOG4CXX_INFO(m_logger, "CATSBridgeUtility::TestFilterCriteria");
//
//	IATSEventsAPI::ATSFilterCriteria atsFilterCriteria;
//	IATSEventsAPI::ATSDateTime atsDateTime;
//	std::list<std::string> strList;
//	std::list<std::string>::iterator itStr;
//	int nIndex;
//
//	m_objConvert.IATSType2ATSType(pFilterCriteria, atsFilterCriteria);
//
//	TCHAR pszData[256];
//	sprintf(pszData, _T("StartTimePeriod = %s | EndTimePeriod = %s"), atsFilterCriteria.EndTimePeriodDefined? _T("TRUE") : _T("FALSE"), atsFilterCriteria.StartTimePeriodDefined? _T("TRUE") : _T("FALSE"));
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger,  pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("StartTimePeriod = Mo: %d | Day: %d | Year: %d"), atsFilterCriteria.StartTimePeriod.Month, atsFilterCriteria.StartTimePeriod.Day, atsFilterCriteria.StartTimePeriod.Year);
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	sprintf(pszData, _T("StartTimePeriod = Mo: %d | Day: %d | Year: %d"), atsFilterCriteria.EndTimePeriod.Month, atsFilterCriteria.EndTimePeriod.Day, atsFilterCriteria.EndTimePeriod.Year);
//	//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//	LOG4CXX_INFO(m_logger, pszData);
//	ZeroMemory(pszData, sizeof(pszData));
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.EventTypeEqualOr.begin(); itStr != atsFilterCriteria.EventTypeEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("EventTypeEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.AssetEqualOr.begin(); itStr != atsFilterCriteria.AssetEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AssetEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
///*
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.DescriptionEqualOr.begin(); itStr != atsFilterCriteria.DescriptionEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, L"DescriptionEqualOr = Data %d: | %s", nIndex++, A2OLE((*itStr).c_str()));
//		MessageBox(NULL, pszData, L"Server Test", MB_OK);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//*/
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.OperatorEqualOr.begin(); itStr != atsFilterCriteria.OperatorEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("OperatorEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.LocationEqualOr.begin(); itStr != atsFilterCriteria.LocationEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("LocationEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.ValueEqualOr.begin(); itStr != atsFilterCriteria.ValueEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("ValueEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.EventTypeEqualOr.begin(); itStr != atsFilterCriteria.EventTypeEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("EventTypeEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.SystemNameEqualOr.begin(); itStr != atsFilterCriteria.SystemNameEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("SystemNameEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.SubsystemNameEqualOr.begin(); itStr != atsFilterCriteria.SubsystemNameEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("SubsystemNameEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	for(itStr = atsFilterCriteria.AlarmIDEqualOr.begin(); itStr != atsFilterCriteria.AlarmIDEqualOr.end(); itStr++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AlarmIDEqualOr = Data %d: | %s"), nIndex++, (*itStr).c_str());
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	std::list<IATSEventsAPI::ATSEventAlarmType>::iterator itInt1;
//	for(itInt1 = atsFilterCriteria.AlarmTypeEqualOr.begin(); itInt1 != atsFilterCriteria.AlarmTypeEqualOr.end(); itInt1++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AlarmTypeEqualOr = Data %d: | %d"), nIndex++, (int)(*itInt1));
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger,  pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	std::list<IATSEventsAPI::ATSEventAlarmMMSType>::iterator itInt2;
//	for(itInt2 = atsFilterCriteria.AlarmMMSTypeEqualOr.begin(); itInt2 != atsFilterCriteria.AlarmMMSTypeEqualOr.end(); itInt2++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AlarmMMSTypeEqualOr = Data %d: | %d"), nIndex++, (int)(*itInt2));
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	std::list<IATSEventsAPI::ATSEventAlarmAvalancheType>::iterator itInt3;
//	for(itInt3 = atsFilterCriteria.AlarmAvalancheTypeEqualOr.begin(); itInt3 != atsFilterCriteria.AlarmAvalancheTypeEqualOr.end(); itInt3++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AlarmAvalancheTypeEqualOr = Data %d: | %d"), nIndex++, (int)(*itInt3));
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	nIndex = 1;
//	std::list<short int>::iterator itInt4;
//	for(itInt4 = atsFilterCriteria.AlarmSeverityEqualOr.begin(); itInt4 != atsFilterCriteria.AlarmSeverityEqualOr.end(); itInt4++)
//	{
//		TCHAR pszData[256];
//		sprintf(pszData, _T("AlarmSeverityEqualOr = Data %d: | %d"), nIndex++, (int)(*itInt4));
//		//MessageBox(NULL, pszData, _T("Server Test"), MB_OK);
//		LOG4CXX_INFO(m_logger, pszData);
//		ZeroMemory(pszData, sizeof(pszData));
//	}
//
//	m_objConvert.ATSType2IATSType(atsFilterCriteria, pFilterCriteriaRet);
//
//	return S_OK;
//}
//