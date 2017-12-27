// ATSStringList.cpp : Implementation of CATSStringList

#include "stdafx.h"
#include "ATSStringList.h"


// CATSStringList


STDMETHODIMP CATSStringList::Add(BSTR bstrVal)
{
	// TODO: Add your implementation code here
	CComBSTR bstrData = bstrVal;
	if(bstrData.Length() == 0)
		return E_INVALIDARG;

	m_StringList.push_back(bstrData);

	return S_OK;
}

STDMETHODIMP CATSStringList::Clear(void)
{
	// TODO: Add your implementation code here
	m_StringList.clear();

	return S_OK;
}

STDMETHODIMP CATSStringList::Remove(LONG lIndex)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > ((long)m_StringList.size() -1))
		return E_INVALIDARG;
	
	StdStringList::iterator it = m_StringList.begin();
	std::advance(it, lIndex);
	m_StringList.erase(it);

	return S_OK;
}

STDMETHODIMP CATSStringList::GetSize(LONG* plRetVal)
{
	// TODO: Add your implementation code here
	*plRetVal = m_StringList.size();
	return S_OK;
}

STDMETHODIMP CATSStringList::GetItem(LONG lIndex, BSTR* pbstrRetVal)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > ((long)m_StringList.size() -1))
		return E_INVALIDARG;

	if(pbstrRetVal == NULL)
		return E_POINTER;
	StdStringList::iterator it = m_StringList.begin();
	std::advance(it, lIndex);

	CComBSTR tempData = *it;

	tempData.CopyTo(pbstrRetVal);

	return S_OK;
}

STDMETHODIMP CATSStringList::SetItemAt(LONG lIndex, BSTR bstrNewVal)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > ((long)m_StringList.size() -1) || SysStringLen(bstrNewVal) == 0)
		return E_INVALIDARG;

	StdStringList::iterator it = m_StringList.begin();
	std::advance(it, lIndex);
	*it = bstrNewVal;

	return S_OK;
}
