// ATSEventSystemList.cpp : Implementation of CATSEventSystemList

#include "stdafx.h"
#include "ATSEventSystemList.h"


// CATSEventSystemList


STDMETHODIMP CATSEventSystemList::Add(IATSEventSystem* pATSEventSystem)
{
	// TODO: Add your implementation code here
	if(pATSEventSystem == NULL)
		return E_POINTER;

	pATSEventSystem->AddRef();
	m_pEventSystemList.push_back(pATSEventSystem);

	return S_OK;
}

STDMETHODIMP CATSEventSystemList::Clear(void)
{
	// TODO: Add your implementation code here
	for(int nIndex = 0; nIndex < (int)m_pEventSystemList.size(); nIndex++ )
	{
		m_pEventSystemList[nIndex]->Release();
	}

	m_pEventSystemList.clear();

	return S_OK;
}

STDMETHODIMP CATSEventSystemList::Remove(LONG lIndex)
{
	// TODO: Add your implementation code here
	if(lIndex <0 || lIndex > (long)m_pEventSystemList.size() - 1)
		return E_INVALIDARG;

	StdEventSystemList::iterator it = m_pEventSystemList.begin();
	std::advance(it, lIndex);

	(*it)->Release();

	m_pEventSystemList.erase(it);

	return S_OK;
}

STDMETHODIMP CATSEventSystemList::GetSize(LONG* plRetVal)
{
	// TODO: Add your implementation code here
	if(plRetVal == NULL)
		return E_POINTER;
	
	*plRetVal = (long)m_pEventSystemList.size();

	return S_OK;
}

STDMETHODIMP CATSEventSystemList::GetItem(LONG lIndex, IATSEventSystem** pATSEventSystem)
{
	// TODO: Add your implementation code here
	if(lIndex <0 || lIndex > (long)m_pEventSystemList.size() - 1)
		return E_INVALIDARG;

	if(pATSEventSystem == NULL)
		return E_POINTER;

	m_pEventSystemList[lIndex]->AddRef();
	*pATSEventSystem = m_pEventSystemList[lIndex];

	return S_OK;
}

STDMETHODIMP CATSEventSystemList::SetItemAt(LONG lIndex, IATSEventSystem* pATSEventSystem)
{
	// TODO: Add your implementation code here
	if(lIndex <0 || lIndex > (long)m_pEventSystemList.size() - 1)
		return E_INVALIDARG;

	if(pATSEventSystem == NULL)
		return E_POINTER;

	m_pEventSystemList[lIndex]->Release();

	pATSEventSystem->AddRef();
	m_pEventSystemList[lIndex] = pATSEventSystem;

	return S_OK;
}
