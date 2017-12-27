// ATSBridgeUtility.h : Declaration of the CATSBridgeUtility

#pragma once
#include "resource.h"       // main symbols
#include "Convert.h"
#include "ATSBridgeUtil_i.h"

#ifdef ENABLE_LOG4CXX
	#include <log4cxx/logger.h>  
	#include "log4cxx/helpers/exception.h"
	using namespace log4cxx;  
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CATSBridgeUtility

class ATL_NO_VTABLE CATSBridgeUtility :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSBridgeUtility, &CLSID_ATSBridgeUtility>,
	public IDispatchImpl<IATSBridgeUtility, &IID_IATSBridgeUtility, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSBridgeUtility()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSBRIDGEUTILITY)


BEGIN_COM_MAP(CATSBridgeUtility)
	COM_INTERFACE_ENTRY(IATSBridgeUtility)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(DoForwardPageQuery)(IATSFilterCriteria* filterCriteria, BSTR atsEventID, LONG length, IATSEventList** pAtsEvents, LONG* pRetVal);
	STDMETHOD(DoForwardPageQueryDate)(IATSFilterCriteria* filterCriteria, IATSDateTime* startTime, LONG length, IATSEventList** atsEvents, LONG* pRetVal);
	STDMETHOD(DoPreviousPageQuery)(IATSFilterCriteria* pfilterCriteria, BSTR atsEventID, LONG length, IATSEventList** pATSEvents, LONG* pRetVal);
	STDMETHOD(DoPreviousPageQueryDate)(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pEndTime, LONG length, IATSEventList** pATSEvents, LONG* pRetVal);
	STDMETHOD(DoStartTimeEndTimeQuery)(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pStartTime, IATSDateTime* pEndTime, IATSEventList** pATSEvents, LONG* pRetVal);
	STDMETHOD(DoStatisticsQuery)(IATSFilterCriteria* pfilterCriteria, IATSDateTime* pStartTime, IATSDateTime* pEndTime, LONG* pEventCount, LONG* pRetVal);
	STDMETHOD(DoEventInfoQuery)(IATSEventSystemList** pEventSystems, IATSStringList** pEventTypes, IATSStringList** pAlarmTypes, LONG* plVal);
	STDMETHOD(DoAlarmInfoQuery)(IATSStringList** pAlarmTypes, LONG* plVal);

private:
	CConvert m_objConvert;
	static LoggerPtr m_logger; // (Logger::getLogger("CATSBridgeUtility"));
	void PopulateTestData(std::list<IATSEventsAPI::ATSEvent*>& atsEvents);
//public:
//	STDMETHOD(TestInt)(IATSIntList * pIntList, IATSIntList ** pIntListRes);
//	STDMETHOD(TestString)(IATSStringList* pStringList, IATSStringList** pStringListRet);
//	STDMETHOD(TestEvent)(IATSEvent* pEvent, IATSEvent** pEventRet);
//	STDMETHOD(TestDateTime)(IATSDateTime* pDateTime, IATSDateTime** pDateTimeRet);
//	STDMETHOD(TestEventSystem)(IATSEventSystem* pEventSystem, IATSEventSystem** pEventSystemRet);
//	STDMETHOD(TestEventList)(IATSEventList* pEventList, IATSEventList** pEventListRet);
//	STDMETHOD(TestEventSystemList)(IATSEventSystemList* pEventSystems, IATSEventSystemList** pEventSystemsRet);
//	STDMETHOD(TestFilterCriteria)(IATSFilterCriteria* pFilterCriteria, IATSFilterCriteria** pFilterCriteriaRet);

};

OBJECT_ENTRY_AUTO(__uuidof(ATSBridgeUtility), CATSBridgeUtility)
