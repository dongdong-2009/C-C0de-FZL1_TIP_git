// ATSEventList.h : Declaration of the CATSEventList

#pragma once
#include "resource.h"       // main symbols
#include <vector>
#include "ATSBridgeUtil_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef std::vector<IATSEvent*> EventList;

// CATSEventList

class ATL_NO_VTABLE CATSEventList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSEventList, &CLSID_ATSEventList>,
	public IDispatchImpl<IATSEventList, &IID_IATSEventList, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSEventList()
	{
		m_EventList.clear();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSEVENTLIST)


BEGIN_COM_MAP(CATSEventList)
	COM_INTERFACE_ENTRY(IATSEventList)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		Clear();
	}

public:

	STDMETHOD(Add)(IATSEvent* pATSEvent);
	STDMETHOD(Clear)(void);
	STDMETHOD(Remove)(LONG lIndex);
	STDMETHOD(GetSize)(LONG* plRetVal);
	STDMETHOD(GetItem)(LONG lIndex, IATSEvent** pATSEvent);
	STDMETHOD(SetItemAt)(LONG lIndex, IATSEvent* pATSEvent);
private:
	EventList m_EventList;

};

OBJECT_ENTRY_AUTO(__uuidof(ATSEventList), CATSEventList)
