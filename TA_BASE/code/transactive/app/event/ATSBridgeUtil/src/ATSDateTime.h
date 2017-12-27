// ATSDateTime.h : Declaration of the CATSDateTime

#pragma once
#include "resource.h"       // main symbols

#include "ATSBridgeUtil_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

enum Prop {
	Year = 0,
	Month,
	Day,
	Hour,
	Minute,
	Second,
	MAX_PROP
};

// CATSDateTime

class ATL_NO_VTABLE CATSDateTime :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSDateTime, &CLSID_ATSDateTime>,
	public IDispatchImpl<IATSDateTime, &IID_IATSDateTime, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSDateTime()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSDATETIME)


BEGIN_COM_MAP(CATSDateTime)
	COM_INTERFACE_ENTRY(IATSDateTime)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		
		// Initialize variables
		for (int nIndex = 0; nIndex < MAX_PROP; nIndex++)
		{
			m_nProperties[nIndex] = 0;
		}
		m_lNanoSecond = 0;
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(get_Year)(SHORT* pVal);
	STDMETHOD(put_Year)(SHORT newVal);
	STDMETHOD(get_Month)(SHORT* pVal);
	STDMETHOD(put_Month)(SHORT newVal);
	STDMETHOD(get_Day)(SHORT* pVal);
	STDMETHOD(put_Day)(SHORT newVal);
	STDMETHOD(get_Hour)(SHORT* pVal);
	STDMETHOD(put_Hour)(SHORT newVal);
	STDMETHOD(get_Minute)(SHORT* pVal);
	STDMETHOD(put_Minute)(SHORT newVal);
	STDMETHOD(get_Second)(SHORT* pVal);
	STDMETHOD(put_Second)(SHORT newVal);
	STDMETHOD(get_Nanosecond)(LONG* pVal);
	STDMETHOD(put_Nanosecond)(LONG newVal);
	STDMETHOD(SetDate)(SHORT Year, SHORT Month, SHORT Day, SHORT Hour, SHORT Minute, SHORT Second, LONG NanoSecond);

private:
	int GetProperty(int nIndex);
	void SetProperty(int nIndex, int nNewVal);
	
	int m_nProperties[MAX_PROP];
	LONG m_lNanoSecond;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSDateTime), CATSDateTime)
