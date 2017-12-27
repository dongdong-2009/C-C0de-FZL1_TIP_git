// ATSEventSystem.cpp : Implementation of CATSEventSystem

#include "stdafx.h"
#include "ATSEventSystem.h"


// CATSEventSystem


STDMETHODIMP CATSEventSystem::get_Name(BSTR* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;
	
	m_bstrName.CopyTo(pVal);
	
	return S_OK;
}

STDMETHODIMP CATSEventSystem::put_Name(BSTR newVal)
{
	// TODO: Add your implementation code here
	if(SysStringLen(newVal) == 0)
		return E_INVALIDARG;

	m_bstrName = newVal;

	return S_OK;
}

STDMETHODIMP CATSEventSystem::GetSubSystemList(IATSStringList** pSubSystemList)
{
	// TODO: Add your implementation code here
	if(pSubSystemList == NULL)
		return E_POINTER;

	if(!m_pSubSystemList)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSStringList, 
										NULL, 
										CLSCTX_INPROC_SERVER, 
										IID_IATSStringList, 
										(void**) &m_pSubSystemList);

		if(FAILED(hr))
			return hr;
	}

	*pSubSystemList = m_pSubSystemList;
	m_pSubSystemList->AddRef();
	return S_OK;
}
