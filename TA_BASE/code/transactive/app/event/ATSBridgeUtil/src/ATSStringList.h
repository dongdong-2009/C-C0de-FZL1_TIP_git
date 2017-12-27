// ATSStringList.h : Declaration of the CATSStringList

#pragma once
#include "resource.h"       // main symbols
#include <vector>
#include "ATSBridgeUtil_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef std::vector<CComBSTR> StdStringList;

// CATSStringList

class ATL_NO_VTABLE CATSStringList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSStringList, &CLSID_ATSStringList>,
	public IDispatchImpl<IATSStringList, &IID_IATSStringList, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSStringList()
	{
		m_StringList.clear();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSSTRINGLIST)


BEGIN_COM_MAP(CATSStringList)
	COM_INTERFACE_ENTRY(IATSStringList)
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

	STDMETHOD(Add)(BSTR bstrVal);
	STDMETHOD(Clear)(void);
	STDMETHOD(Remove)(LONG lIndex);
	STDMETHOD(GetSize)(LONG* plRetVal);
	STDMETHOD(GetItem)(LONG lIndex, BSTR* pbstrRetVal);
	STDMETHOD(SetItemAt)(LONG lIndex, BSTR bstrNewVal);

private:
	StdStringList m_StringList;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSStringList), CATSStringList)
