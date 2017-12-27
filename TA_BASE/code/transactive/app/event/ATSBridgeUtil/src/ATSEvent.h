// ATSEvent.h : Declaration of the CATSEvent

#pragma once
#include "resource.h"       // main symbols

#include "ATSBridgeUtil_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

enum StringProp {
	EventID = 0,
	EventType,
	Asset,
	Description,
	Operator,
	Location,
	Value,
	SystemName,
	SubsystemName,
	AlarmID,
	AlarmType,
	MAX_STRING_PROP
};

enum NumberProp {
	OperationalMaintenanceType = 0,
	AlarmMMSState,
	AlarmAvalancheType,
	AlarmSeverity,
	AlarmState,
	MAX_NUMBER_PROP
};

// CATSEvent

class ATL_NO_VTABLE CATSEvent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSEvent, &CLSID_ATSEvent>,
	public IDispatchImpl<IATSEvent, &IID_IATSEvent, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSEvent()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSEVENT)


BEGIN_COM_MAP(CATSEvent)
	COM_INTERFACE_ENTRY(IATSEvent)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		USES_CONVERSION;
		// initialize string properties
		for(int nIndex = 0; nIndex < MAX_STRING_PROP; nIndex++ )
		{
			m_bstrStringProp[nIndex] = A2OLE("");
		}

		// initialize number properties
		for(int nIndex = 0; nIndex < MAX_NUMBER_PROP; nIndex++ )
		{
			m_nNumberProp[nIndex] = 0;
		}

		m_pATSTimeStamp = NULL;

		return S_OK;
	}

	void FinalRelease()
	{
		if(m_pATSTimeStamp)
			m_pATSTimeStamp->Release();
	}

public:

	STDMETHOD(get_EventID)(BSTR* pVal);
	STDMETHOD(put_EventID)(BSTR newVal);
	STDMETHOD(get_EventType)(BSTR* pVal);
	STDMETHOD(put_EventType)(BSTR newVal);
	STDMETHOD(GetTimeStamp)(IATSDateTime** pDateTime);
	STDMETHOD(get_Asset)(BSTR* pVal);
	STDMETHOD(put_Asset)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR* pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_Operator)(BSTR* pVal);
	STDMETHOD(put_Operator)(BSTR newVal);
	STDMETHOD(get_Location)(BSTR* pVal);
	STDMETHOD(put_Location)(BSTR newVal);
	STDMETHOD(get_Value)(BSTR* pVal);
	STDMETHOD(put_Value)(BSTR newVal);
	STDMETHOD(get_SystemName)(BSTR* pVal);
	STDMETHOD(put_SystemName)(BSTR newVal);
	STDMETHOD(get_SubsystemName)(BSTR* pVal);
	STDMETHOD(put_SubsystemName)(BSTR newVal);
	STDMETHOD(get_AlarmID)(BSTR* pVal);
	STDMETHOD(put_AlarmID)(BSTR newVal);
	STDMETHOD(get_AlarmType)(BSTR* pVal);
	STDMETHOD(put_AlarmType)(BSTR newVal);
	STDMETHOD(get_AlarmMMSState)(SHORT* pVal);
	STDMETHOD(put_AlarmMMSState)(SHORT newVal);
	STDMETHOD(get_AlarmAvalancheType)(SHORT* pVal);
	STDMETHOD(put_AlarmAvalancheType)(SHORT newVal);
	STDMETHOD(get_AlarmSeverity)(SHORT* pVal);
	STDMETHOD(put_AlarmSeverity)(SHORT newVal);
	STDMETHOD(get_OperationalMaintenanceType)(SHORT* pVal);
	STDMETHOD(put_OperationalMaintenanceType)(SHORT newVal);
	STDMETHOD(get_AlarmState)(SHORT* pVal);
	STDMETHOD(put_AlarmState)(SHORT newVal);

private:
	// functions
	CComBSTR GetStringProp(int nIndex);
	void SetStringProp(int nIndex, CComBSTR bstrData);
	int GetNumberProp(int nIndex);
	void SetNumberProp(int nIndex, int nData);

	// variables
	CComBSTR m_bstrStringProp[MAX_STRING_PROP];
	int	m_nNumberProp[MAX_NUMBER_PROP];
	IATSDateTime* m_pATSTimeStamp;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSEvent), CATSEvent)
