// ATSIntList.h : Declaration of the CATSIntList

#pragma once
#include "resource.h"       // main symbols
#include <vector>
#include "ATSBridgeUtil_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef std::vector<int> StdIntList;

// CATSIntList

class ATL_NO_VTABLE CATSIntList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSIntList, &CLSID_ATSIntList>,
	public IDispatchImpl<IATSIntList, &IID_IATSIntList, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSIntList()
	{
		m_IntList.clear();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSINTLIST)


BEGIN_COM_MAP(CATSIntList)
	COM_INTERFACE_ENTRY(IATSIntList)
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

	STDMETHOD(Add)(LONG lVal);
	STDMETHOD(Clear)(void);
	STDMETHOD(Remove)(LONG lIndex);
	STDMETHOD(GetSize)(LONG* plRetVal);
	STDMETHOD(GetItem)(LONG lIndex, LONG* plRetVal);
	STDMETHOD(SetItemAt)(LONG lIndex, LONG lnewVal);
private:
	StdIntList m_IntList;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSIntList), CATSIntList)
