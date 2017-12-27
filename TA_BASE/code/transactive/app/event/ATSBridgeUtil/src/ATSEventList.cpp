// ATSEventList.cpp : Implementation of CATSEventList

#include "stdafx.h"
#include "ATSEventList.h"


// CATSEventList


STDMETHODIMP CATSEventList::Add(IATSEvent* pATSEvent)
{
	// TODO: Add your implementation code here
	if(pATSEvent == NULL)
		return E_POINTER;

	pATSEvent->AddRef();
	m_EventList.push_back(pATSEvent);
	return S_OK;
}

STDMETHODIMP CATSEventList::Clear(void)
{
	// TODO: Add your implementation code here
	for(int nIndex = 0; nIndex < (long)m_EventList.size(); nIndex++)
	{
		m_EventList[nIndex]->Release();
	}
	
	m_EventList.clear();

	return S_OK;
}

STDMETHODIMP CATSEventList::Remove(LONG lIndex)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > (long)m_EventList.size() - 1)
		return E_INVALIDARG;

	EventList::iterator it = m_EventList.begin();
	std::advance(it, lIndex);
	(*it)->Release();

	m_EventList.erase(it);
	
	return S_OK;
}

STDMETHODIMP CATSEventList::GetSize(LONG* plRetVal)
{
	// TODO: Add your implementation code here
	if(plRetVal == NULL)
		return E_INVALIDARG;

	*plRetVal = m_EventList.size();

	return S_OK;
}

STDMETHODIMP CATSEventList::GetItem(LONG lIndex, IATSEvent** pATSEvent)
{
	// TODO: Add your implementation code here
	if(pATSEvent == NULL)
		return E_POINTER;

	*pATSEvent = m_EventList[lIndex];
	m_EventList[lIndex]->AddRef();

	return S_OK;
}

STDMETHODIMP CATSEventList::SetItemAt(LONG lIndex, IATSEvent* pATSEvent)
{
	// TODO: Add your implementation code here
	if(lIndex < 0 || lIndex > (long)m_EventList.size() - 1)
		return E_INVALIDARG;

	if(pATSEvent == NULL)
		return E_POINTER;
	
	m_EventList[lIndex]->Release();
	
	pATSEvent->AddRef();
	m_EventList[lIndex] = pATSEvent;

	return S_OK;
}
