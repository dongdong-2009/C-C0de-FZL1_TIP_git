// ATSFilterCriteria.h : Declaration of the CATSFilterCriteria

#pragma once
#include "resource.h"       // main symbols

#include "ATSBridgeUtil_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

enum StringListProp {
	EventTypeEqualOr = 0,
	AssetEqualOr,
	DescriptionEqualOr,
	OperatorEqualOr,
	LocationEqualOr,
	ValueEqualOr,
	SystemNameEqualOr,
	SubsystemNameEqualOr,
	AlarmIDEqualOr,
	AlarmTypeEqualOr,
	MAX_STRINGLIST_PROP
};

enum IntListProp {
	OperationalMaintenanceTypeEqualOr = 0 + MAX_STRINGLIST_PROP,
	AlarmStateEqualOr,
	AlarmAvalancheTypeEqualOr,
	AlarmSeverityEqualOr,
	MAX_INTLIST_PROP
};


// CATSFilterCriteria

class ATL_NO_VTABLE CATSFilterCriteria :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSFilterCriteria, &CLSID_ATSFilterCriteria>,
	public IDispatchImpl<IATSFilterCriteria, &IID_IATSFilterCriteria, &LIBID_ATSBridgeUtilLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSFilterCriteria()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSFILTERCRITERIA)


BEGIN_COM_MAP(CATSFilterCriteria)
	COM_INTERFACE_ENTRY(IATSFilterCriteria)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		// Initialize IStringList properties
		for(int nIndex = 0; nIndex < MAX_STRINGLIST_PROP; nIndex++)
		{
			m_pStringListProp[nIndex] = NULL;
		}

		// Initialize IIntList properties
		for(int nIndex = 0; nIndex < MAX_INTLIST_PROP; nIndex++)
		{
			m_pIntListProp[nIndex] = NULL;
		}
		m_pStartTimePeriod = NULL;
		m_pEndTimePeriod = NULL;

		m_bStartTimePeriodDefined = FALSE;
		m_bEndTimePeriodDefined = FALSE;
		
		return S_OK;
	}

	void FinalRelease()
	{
		// Destroy all IStringList properties
		for(int nIndex = 0; nIndex < MAX_STRINGLIST_PROP; nIndex++)
		{
			if(m_pStringListProp[nIndex])
				m_pStringListProp[nIndex]->Release();
		}

		// Destroy all IIntList properties
		for(int nIndex = 0; nIndex < MAX_INTLIST_PROP; nIndex++)
		{
			if(m_pIntListProp[nIndex])
				m_pIntListProp[nIndex]->Release();
		}

		if(m_pStartTimePeriod)
			m_pStartTimePeriod->Release();

		if(m_pEndTimePeriod)
			m_pEndTimePeriod->Release();
	}

public:

	STDMETHOD(get_StartTimePeriodDefined)(VARIANT_BOOL* pVal);
	STDMETHOD(put_StartTimePeriodDefined)(VARIANT_BOOL newVal);
	STDMETHOD(GetStartTimePeriod)(IATSDateTime** pDateTime);
	STDMETHOD(get_EndTimePeriodDefined)(VARIANT_BOOL* pVal);
	STDMETHOD(put_EndTimePeriodDefined)(VARIANT_BOOL newVal);
	STDMETHOD(GetEndTimePeriod)(IATSDateTime** pDateTime);
	STDMETHOD(GetEventTypeEqualOr)(IATSStringList** pStringList);
	STDMETHOD(GetAssetEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetDescriptionEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetOperatorEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetLocationEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetValueEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetSystemNameEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetSubsystemNameEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetAlarmIDEqualOr)(IATSStringList** pATSStringList);
	STDMETHOD(GetOperationalMaintenanceTypeEqualOr)(IATSIntList** pATSIntList);
	STDMETHOD(GetAlarmStateEqualOr)(IATSIntList** pATSIntList);
	STDMETHOD(GetAlarmAvalancheTypeEqualOr)(IATSIntList** pATSIntList);
	STDMETHOD(GetAlarmSeverityEqualOr)(IATSIntList** pATSIntList);
	STDMETHOD(GetAlarmTypeEqualOr)(IATSStringList** pStringList);

private:
	HRESULT GetStringListProp(int nIndex, IATSStringList** pProp);
//	HRESULT SetStringListProp(int nIndex, IATSStringList* pProp);

	HRESULT GetIntListProp(int nIndex, IATSIntList** pProp);
//	HRESULT SetIntListProp(int nIndex, IATSIntList* pProp);

	BOOL m_bStartTimePeriodDefined;
	BOOL m_bEndTimePeriodDefined;
	IATSDateTime* m_pStartTimePeriod;
	IATSDateTime* m_pEndTimePeriod;

	IATSStringList* m_pStringListProp[MAX_STRINGLIST_PROP];
	IATSIntList* m_pIntListProp[MAX_INTLIST_PROP];
};

OBJECT_ENTRY_AUTO(__uuidof(ATSFilterCriteria), CATSFilterCriteria)
