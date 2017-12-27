// ATSEventSystem.h : Declaration of the CATSEventSystem

#pragma once
#include "resource.h"       // main symbols

#include "ATSBridgeUtil_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CATSEventSystem

class ATL_NO_VTABLE CATSEventSystem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSEventSystem, &CLSID_ATSEventSystem>,
	public IDispatchImpl<IATSEventSystem, &IID_IATSEventSystem, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSEventSystem()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSEVENTSYSTEM)


BEGIN_COM_MAP(CATSEventSystem)
	COM_INTERFACE_ENTRY(IATSEventSystem)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_bstrName = L"";
		m_pSubSystemList = NULL;
		return S_OK;
	}

	void FinalRelease()
	{
		if(m_pSubSystemList)
			m_pSubSystemList->Release();
	}

public:

	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(GetSubSystemList)(IATSStringList** pSubSystemList);
private:
	CComBSTR m_bstrName;
	IATSStringList* m_pSubSystemList;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSEventSystem), CATSEventSystem)
