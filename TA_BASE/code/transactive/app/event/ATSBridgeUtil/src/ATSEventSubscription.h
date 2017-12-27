// ATSEventSubscription.h : Declaration of the CATSEventSubscription

#pragma once
#include "resource.h"       // main symbols

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h"
	#include "log4cxx/helpers/exception.h"
	using namespace log4cxx;  
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

#include "IATSEventsAPI.h"
#include "ATSBridgeUtil_i.h"
#include "_IATSEventSubscriptionEvents_CP.h"	
#include "CallbackHandler.h"

#include <list>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CATSEventSubscription

class ATL_NO_VTABLE CATSEventSubscription :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSEventSubscription, &CLSID_ATSEventSubscription>,
	public IConnectionPointContainerImpl<CATSEventSubscription>,
	public CProxy_IATSEventSubscriptionEvents<CATSEventSubscription>,
	public IDispatchImpl<IATSEventSubscription, &IID_IATSEventSubscription, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSEventSubscription()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSEVENTSUBSCRIPTION)


BEGIN_COM_MAP(CATSEventSubscription)
	COM_INTERFACE_ENTRY(IATSEventSubscription)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CATSEventSubscription)
	CONNECTION_POINT_ENTRY(__uuidof(_IATSEventSubscriptionEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease()
	{
		
	}

public:
	STDMETHOD(startSubscription)(IATSFilterCriteria* pfilterCriteria, LONG backloglength, IATSEventList** pATSEvents, LONG* pRetVal);
	STDMETHOD(stopSubscription)(LONG* plVal);

	static void SubscriptionCallback(const IATSEventsAPI::ATSSubscriptionError error, std::list<IATSEventsAPI::ATSEvent*>& atsEvents);
	static DWORD WINAPI ThreadFunc(LPVOID args);
private:
	IATSEventsAPI::ATSEventSubscription * m_pATSEventSubscription;
	//static CATSEventSubscription *pThis;
	static LoggerPtr m_logger;
	static CallBackHandler * m_pCallbackHandler;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSEventSubscription), CATSEventSubscription)
