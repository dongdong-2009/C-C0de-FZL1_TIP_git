// ATSDateTime.cpp : Implementation of CATSDateTime

#include "stdafx.h"
#include "ATSDateTime.h"


// CATSDateTime


STDMETHODIMP CATSDateTime::get_Year(SHORT* pVal)
{
	// TODO: Add your implementation code here
	//CFuncLog funcLog(m_pLog, "CATSDateTime::FinalConstruct");
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Year);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Year(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 9999 ||newVal < 1000)
		return E_INVALIDARG;

	SetProperty(Year, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Month(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Month);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Month(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 12 ||newVal < 1)
		return E_INVALIDARG;

	SetProperty(Month, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Day(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Day);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Day(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 31 ||newVal < 1)
		return E_INVALIDARG;

	SetProperty(Day, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Hour(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Hour);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Hour(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 23 ||newVal < 0)
		return E_INVALIDARG;

	SetProperty(Hour, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Minute(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Minute);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Minute(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 59 ||newVal < 0)
		return E_INVALIDARG;

	SetProperty(Minute, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Second(SHORT* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = GetProperty(Second);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Second(SHORT newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 59 ||newVal < 0)
		return E_INVALIDARG;

	SetProperty(Second, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::get_Nanosecond(LONG* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = m_lNanoSecond;
	//*pVal = GetProperty(Nanosecond);

	return S_OK;
}

STDMETHODIMP CATSDateTime::put_Nanosecond(LONG newVal)
{
	// TODO: Add your implementation code here
	if(newVal > 999999999 ||newVal < 0)
		return E_INVALIDARG;

	m_lNanoSecond = newVal;
//	SetProperty(Nanosecond, newVal);

	return S_OK;
}

STDMETHODIMP CATSDateTime::SetDate(SHORT Year, SHORT Month, SHORT Day, SHORT Hour, SHORT Minute, SHORT Second, LONG NanoSecond)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	hr = put_Year(Year);
	if(FAILED(hr))
		return hr;

	put_Year(Year);
	put_Month(Month);
	put_Day(Day);
	put_Hour(Hour);
	put_Minute(Minute);
	put_Second(Second);
	put_Nanosecond(NanoSecond);

	return S_OK;
}

int CATSDateTime::GetProperty(int nIndex)
{
	if(nIndex < 0 || nIndex > MAX_PROP)
		return -1;
	
	return m_nProperties[nIndex];
}

void CATSDateTime::SetProperty(int nIndex, int nNewVal)
{
	if(nIndex < 0 || nIndex > MAX_PROP)
		return;

	m_nProperties[nIndex] = nNewVal;
}
