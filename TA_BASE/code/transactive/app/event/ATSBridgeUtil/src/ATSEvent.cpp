// ATSEvent.cpp : Implementation of CATSEvent

#include "stdafx.h"
#include "ATSEvent.h"


// CATSEvent


STDMETHODIMP CATSEvent::get_EventID(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(EventID);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_EventID(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(EventID, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_EventType(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(EventType);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_EventType(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(EventType, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::GetTimeStamp(IATSDateTime** pDateTime)
{
	// TODO: Add your implementation code here
	if(pDateTime == NULL)
		return E_POINTER;

	if(!m_pATSTimeStamp)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSDateTime, 
										NULL, 
										CLSCTX_INPROC_SERVER, 
										IID_IATSDateTime, 
										(void**) &m_pATSTimeStamp);

		if(FAILED(hr))
			return hr;
	}
	
	*pDateTime = m_pATSTimeStamp;
	m_pATSTimeStamp->AddRef();
	
	return S_OK;
}

STDMETHODIMP CATSEvent::get_Asset(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(Asset);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_Asset(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(Asset, newVal);
	return S_OK;
}


STDMETHODIMP CATSEvent::get_Description(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(Description);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_Description(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(Description, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_Operator(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(Operator);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_Operator(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(Operator, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_Location(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(Location);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_Location(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(Location, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_Value(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(Value);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_Value(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(Value, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_SystemName(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(SystemName);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_SystemName(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(SystemName, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_SubsystemName(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrRetVal;
	bstrRetVal = GetStringProp(SubsystemName);

	bstrRetVal.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_SubsystemName(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(SubsystemName, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_AlarmID(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrData = GetStringProp(AlarmID);
	
	bstrData.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmID(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(AlarmID, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_AlarmType(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	CComBSTR bstrData = GetStringProp(AlarmType);

	bstrData.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmType(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	SetStringProp(AlarmType, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_AlarmMMSState(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_INVALIDARG;
	
	*pVal = GetNumberProp(AlarmMMSState);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmMMSState(SHORT newVal)
{
	// TODO: Add your implementation code here
	SetNumberProp(AlarmMMSState, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_AlarmAvalancheType(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_INVALIDARG;
	
	*pVal = GetNumberProp(AlarmAvalancheType);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmAvalancheType(SHORT newVal)
{
	// TODO: Add your implementation code here
	SetNumberProp(AlarmAvalancheType, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_AlarmSeverity(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_INVALIDARG;
	
	*pVal = GetNumberProp(AlarmSeverity);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmSeverity(SHORT newVal)
{
	// TODO: Add your implementation code here
	SetNumberProp(AlarmSeverity, newVal);

	return S_OK;
}

STDMETHODIMP CATSEvent::get_OperationalMaintenanceType(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_INVALIDARG;

	*pVal = GetNumberProp(OperationalMaintenanceType);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_OperationalMaintenanceType(SHORT newVal)
{
	// TODO: Add your implementation code here
	SetNumberProp(OperationalMaintenanceType, newVal);

	return S_OK;
}


STDMETHODIMP CATSEvent::get_AlarmState(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_INVALIDARG;
	
	*pVal = GetNumberProp(AlarmState);

	return S_OK;
}

STDMETHODIMP CATSEvent::put_AlarmState(SHORT newVal)
{
	// TODO: Add your implementation code here
	SetNumberProp(AlarmState, newVal);

	return S_OK;
}

CComBSTR CATSEvent::GetStringProp(int nIndex)
{
	if(nIndex <0 || nIndex > MAX_STRING_PROP)
		return NULL;

	return m_bstrStringProp[nIndex];
}

void CATSEvent::SetStringProp(int nIndex, CComBSTR bstrData)
{
	if(nIndex <0 || nIndex > MAX_STRING_PROP)
		return;

	m_bstrStringProp[nIndex] = bstrData;
}

int CATSEvent::GetNumberProp(int nIndex)
{
	if(nIndex <0 || nIndex > MAX_NUMBER_PROP)
		return NULL;

	return m_nNumberProp[nIndex];
}

void CATSEvent::SetNumberProp(int nIndex, int nData)
{
	if(nIndex <0 || nIndex > MAX_NUMBER_PROP)
		return;

	m_nNumberProp[nIndex] = nData;
}
