// ATSFilterCriteria.cpp : Implementation of CATSFilterCriteria

#include "stdafx.h"
#include "ATSFilterCriteria.h"
//#include "log4cxx/logger.h"
//#include "log4cxx/helpers/exception.h"
//
//using namespace log4cxx;
//using namespace log4cxx::helpers;
//

// CATSFilterCriteria

//LoggerPtr pLogger(Logger::getLogger("CATSFilterCriteria"));

STDMETHODIMP CATSFilterCriteria::get_StartTimePeriodDefined(VARIANT_BOOL* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;

	*pVal = m_bStartTimePeriodDefined?VARIANT_TRUE:VARIANT_FALSE;
	//LOG4CXX_INFO(pLogger, std::string((*pVal)?"TRUE":"FALSE"));

	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::put_StartTimePeriodDefined(VARIANT_BOOL newVal)
{
	// TODO: Add your implementation code here
	m_bStartTimePeriodDefined = (newVal == VARIANT_TRUE)? TRUE:FALSE;

	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::GetStartTimePeriod(IATSDateTime** pDateTime)
{
	// TODO: Add your implementation code here
	if(pDateTime == NULL)
		return E_POINTER;

	if(!m_pStartTimePeriod)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSDateTime, NULL, CLSCTX_INPROC_SERVER, IID_IATSDateTime, (void**)&m_pStartTimePeriod);
		if(FAILED(hr))
			return hr;
	}

	m_pStartTimePeriod->AddRef();
	*pDateTime = m_pStartTimePeriod;

	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::get_EndTimePeriodDefined(VARIANT_BOOL* pVal)
{
	// TODO: Add your implementation code here
	if(pVal == NULL)
		return E_POINTER;
	
	*pVal = m_bEndTimePeriodDefined?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::put_EndTimePeriodDefined(VARIANT_BOOL newVal)
{
	// TODO: Add your implementation code here

	m_bEndTimePeriodDefined = (newVal == VARIANT_TRUE)?TRUE:FALSE;

	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::GetEndTimePeriod(IATSDateTime** pDateTime)
{
	// TODO: Add your implementation code here
	if(pDateTime == NULL)
		return E_POINTER;

	if(!m_pEndTimePeriod)
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSDateTime, NULL, CLSCTX_INPROC_SERVER, IID_IATSDateTime, (void**)&m_pEndTimePeriod);
		if(FAILED(hr))
			return hr;
	}

	m_pEndTimePeriod->AddRef();
	*pDateTime = m_pEndTimePeriod;
	return S_OK;
}

STDMETHODIMP CATSFilterCriteria::GetEventTypeEqualOr(IATSStringList** pStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(EventTypeEqualOr, pStringList);
}

STDMETHODIMP CATSFilterCriteria::GetAssetEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(AssetEqualOr, pATSStringList);
}

STDMETHODIMP CATSFilterCriteria::GetDescriptionEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(DescriptionEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetOperatorEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(OperatorEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetLocationEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(LocationEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetValueEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(ValueEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetSystemNameEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(SystemNameEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetSubsystemNameEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(SubsystemNameEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetAlarmIDEqualOr(IATSStringList** pATSStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(AlarmIDEqualOr, pATSStringList);

}

STDMETHODIMP CATSFilterCriteria::GetOperationalMaintenanceTypeEqualOr(IATSIntList** pATSIntList)
{
	// TODO: Add your implementation code here
	return GetIntListProp(OperationalMaintenanceTypeEqualOr, pATSIntList);

}

STDMETHODIMP CATSFilterCriteria::GetAlarmStateEqualOr(IATSIntList** pATSIntList)
{
	// TODO: Add your implementation code here
	return GetIntListProp(AlarmStateEqualOr, pATSIntList);

}

STDMETHODIMP CATSFilterCriteria::GetAlarmAvalancheTypeEqualOr(IATSIntList** pATSIntList)
{
	// TODO: Add your implementation code here
	return GetIntListProp(AlarmAvalancheTypeEqualOr, pATSIntList);

}

STDMETHODIMP CATSFilterCriteria::GetAlarmSeverityEqualOr(IATSIntList** pATSIntList)
{
	// TODO: Add your implementation code here
	return GetIntListProp(AlarmSeverityEqualOr, pATSIntList);

}

STDMETHODIMP CATSFilterCriteria::GetAlarmTypeEqualOr(IATSStringList** pStringList)
{
	// TODO: Add your implementation code here
	return GetStringListProp(AlarmTypeEqualOr, pStringList);

}
HRESULT CATSFilterCriteria::GetStringListProp(int nIndex, IATSStringList** pProp)
{
	if(pProp == NULL)
		return E_POINTER;

	if(nIndex < 0 || nIndex > MAX_STRINGLIST_PROP)
		return E_INVALIDARG;

	if(!m_pStringListProp[nIndex])
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSStringList, NULL, CLSCTX_INPROC_SERVER, IID_IATSStringList, (void**)&m_pStringListProp[nIndex]);
		if(FAILED(hr))
			return hr;
	}

	m_pStringListProp[nIndex]->AddRef();
	*pProp =  m_pStringListProp[nIndex];
	
	return S_OK;
}
/*
HRESULT CATSFilterCriteria::SetStringListProp(int nIndex, IATSStringList* pProp)
{
	return S_OK;
}
*/
HRESULT CATSFilterCriteria::GetIntListProp(int nIndex, IATSIntList** pProp)
{
	if(pProp == NULL)
		return E_POINTER;

	if(nIndex < 0 || nIndex > MAX_INTLIST_PROP)
		return E_INVALIDARG;

	if(!m_pIntListProp[nIndex])
	{
		HRESULT hr = CoCreateInstance(CLSID_ATSIntList, NULL, CLSCTX_INPROC_SERVER, IID_IATSIntList, (void**)&m_pIntListProp[nIndex]);
		if(FAILED(hr))
			return hr;
	}

	m_pIntListProp[nIndex]->AddRef();
	*pProp =  m_pIntListProp[nIndex];
	return S_OK;
}
/*
HRESULT CATSFilterCriteria::SetIntListProp(int nIndex, IATSIntList* pProp)
{

	return S_OK;
}
*/