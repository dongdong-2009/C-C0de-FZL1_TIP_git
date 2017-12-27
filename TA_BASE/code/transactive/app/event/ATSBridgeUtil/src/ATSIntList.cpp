// ATSIntList.cpp : Implementation of CATSIntList

#include "stdafx.h"
#include "ATSIntList.h"


// CATSIntList


STDMETHODIMP CATSIntList::Add(LONG lVal)
{
	// TODO: Add your implementation code here
	m_IntList.push_back(lVal);

	return S_OK;
}

STDMETHODIMP CATSIntList::Clear(void)
{
	// TODO: Add your implementation code here
	m_IntList.clear();

	return S_OK;
}

STDMETHODIMP CATSIntList::Remove(LONG lIndex)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > (long)m_IntList.size() - 1)
		return E_INVALIDARG;

	StdIntList::iterator it = m_IntList.begin();
	std::advance(it, lIndex);
	m_IntList.erase(it);

	return S_OK;
}

STDMETHODIMP CATSIntList::GetSize(LONG* plRetVal)
{
	// TODO: Add your implementation code here
	if(plRetVal == NULL)
		return E_POINTER;

	*plRetVal = (long)m_IntList.size();
	
	return S_OK;
}

STDMETHODIMP CATSIntList::GetItem(LONG lIndex, LONG* plRetVal)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > (long)m_IntList.size() - 1)
		return E_INVALIDARG;

	if(plRetVal == NULL)
		return E_POINTER;

	*plRetVal = m_IntList[lIndex];

	return S_OK;
}

STDMETHODIMP CATSIntList::SetItemAt(LONG lIndex, LONG lnewVal)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > (long)m_IntList.size() - 1)
		return E_INVALIDARG;
	
	m_IntList[lIndex] = lnewVal;

	return S_OK;
}
