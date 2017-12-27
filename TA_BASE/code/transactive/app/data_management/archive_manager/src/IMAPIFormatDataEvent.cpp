/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/IMAPIFormatDataEvent.cpp $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/04/13 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Class to handle the updates from IMAPI write operation.
  */
#include "app/data_management/archive_manager/src/IMAPIFormatDataEvent.h"


// CIMAPIFormatDataEvent

IMPLEMENT_DYNAMIC(CIMAPIFormatDataEvent, CCmdTarget)

BEGIN_INTERFACE_MAP(CIMAPIFormatDataEvent, CCmdTarget)
	INTERFACE_PART(CIMAPIFormatDataEvent, IID_IDispatch, FormatDataEvents)
	INTERFACE_PART(CIMAPIFormatDataEvent, IID_DDiscFormat2DataEvents, FormatDataEvents)
END_INTERFACE_MAP()

CIMAPIFormatDataEvent::CIMAPIFormatDataEvent()
: m_callbackBurnStatus(NULL)
, m_ptinfo(NULL)
, m_dwCookie(0)
, m_pUnkSink(0)
, m_pUnkSrc(0)
{
}

CIMAPIFormatDataEvent::~CIMAPIFormatDataEvent()
{
	if (m_dwCookie && (m_pUnkSrc != NULL) && (m_pUnkSink != NULL))
	{
		AfxConnectionUnadvise(m_pUnkSrc, IID_DDiscFormat2DataEvents, m_pUnkSink,
			TRUE, m_dwCookie);
	}
}


BEGIN_MESSAGE_MAP(CIMAPIFormatDataEvent, CCmdTarget)
END_MESSAGE_MAP()



// CIMAPIFormatDataEvent message handlers

ULONG FAR EXPORT CIMAPIFormatDataEvent::XFormatDataEvents::AddRef()
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		return pThis->ExternalAddRef();
}
ULONG FAR EXPORT CIMAPIFormatDataEvent::XFormatDataEvents::Release()
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		return pThis->ExternalRelease();
}
STDMETHODIMP CIMAPIFormatDataEvent::XFormatDataEvents::QueryInterface(REFIID riid,
																	 LPVOID FAR* ppvObj)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}
STDMETHODIMP
CIMAPIFormatDataEvent::XFormatDataEvents::GetTypeInfoCount(UINT FAR* pctinfo)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		*pctinfo = 1;
	return NOERROR;
}
STDMETHODIMP CIMAPIFormatDataEvent::XFormatDataEvents::GetTypeInfo(
	UINT itinfo,
	LCID lcid,
	ITypeInfo FAR* FAR* pptinfo)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		*pptinfo = NULL;

	if(itinfo != 0)
		return ResultFromScode(DISP_E_BADINDEX);
	pThis->m_ptinfo->AddRef();
	*pptinfo = pThis->m_ptinfo;
	return NOERROR;
}
STDMETHODIMP CIMAPIFormatDataEvent::XFormatDataEvents::GetIDsOfNames(
	REFIID riid,
	OLECHAR FAR* FAR* rgszNames,
	UINT cNames,
	LCID lcid,
	DISPID FAR* rgdispid)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)
		return DispGetIDsOfNames(pThis->m_ptinfo, rgszNames, cNames, rgdispid);
}
STDMETHODIMP CIMAPIFormatDataEvent::XFormatDataEvents::Invoke(
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS FAR* pdispparams,
	VARIANT FAR* pvarResult,
	EXCEPINFO FAR* pexcepinfo,
	UINT FAR* puArgErr)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)

		return DispInvoke(&pThis->m_xFormatDataEvents, pThis->m_ptinfo,
		dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}



CIMAPIFormatDataEvent* CIMAPIFormatDataEvent::CreateEventSink()
{
	// Create the event sink
	CIMAPIFormatDataEvent* pIMAPIFormatDataEvent = new CIMAPIFormatDataEvent();

	pIMAPIFormatDataEvent->EnableAutomation();
	pIMAPIFormatDataEvent->ExternalAddRef();

	return pIMAPIFormatDataEvent;
}


bool CIMAPIFormatDataEvent::ConnectDiscFormatData(IDiscFormat2Data* pDiscFormatData)
{
	m_pUnkSink = GetIDispatch(FALSE);
	m_pUnkSrc = pDiscFormatData;

	LPTYPELIB ptlib = NULL;
	HRESULT hr = LoadRegTypeLib(LIBID_IMAPILib2, 
		IMAPILib2_MajorVersion, IMAPILib2_MinorVersion, 
		LOCALE_SYSTEM_DEFAULT, &ptlib);
	if (FAILED(hr))
	{
		return false;
	}
	hr = ptlib->GetTypeInfoOfGuid(IID_DDiscFormat2DataEvents, &m_ptinfo);
	ptlib->Release();
	if (FAILED(hr))
	{
		return false;
	}

	BOOL bRet = AfxConnectionAdvise(m_pUnkSrc, IID_DDiscFormat2DataEvents, m_pUnkSink,
		TRUE, &m_dwCookie);
	if (bRet)
	{
		return true;
	}

	return false;
}



STDMETHODIMP_(HRESULT) CIMAPIFormatDataEvent::XFormatDataEvents::Update(IDispatch* objectDispatch, IDispatch* progressDispatch)
{
	METHOD_PROLOGUE(CIMAPIFormatDataEvent, FormatDataEvents)

	IDiscFormat2DataEventArgs* progress = NULL;
	HRESULT hr = progressDispatch->QueryInterface(IID_PPV_ARGS(&progress));

	IDiscFormat2Data* discFormatData = NULL;
	hr = objectDispatch->QueryInterface(IID_PPV_ARGS(&discFormatData));

	IMAPI_FORMAT2_DATA_WRITE_ACTION currentAction = IMAPI_FORMAT2_DATA_WRITE_ACTION_VALIDATING_MEDIA;
	hr = progress->get_CurrentAction(&currentAction);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		return S_OK;
	}

	IMAPI_STATUS imapiStatus = {0};
	
	//set BurnStatus
	BurnStatus burnStatus = BurnStatus::None;
	switch(currentAction)
	{
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_VALIDATING_MEDIA:
		burnStatus = BurnStatus::ValidatingMedia;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_FORMATTING_MEDIA:
		burnStatus = BurnStatus::FormattingMedia;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_INITIALIZING_HARDWARE:
		burnStatus = BurnStatus::InitialisingHardware;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_CALIBRATING_POWER:
		burnStatus = BurnStatus::CalibratingPower;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA:
		burnStatus = BurnStatus::Writing;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_FINALIZATION:	
		burnStatus = BurnStatus::WriteFinalization;
		break;
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_COMPLETED	:
		burnStatus = BurnStatus::WriteCompleted;
		break;
	default:
		burnStatus = BurnStatus::None;
	}

	if ((currentAction == IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA) ||
		(currentAction == IMAPI_FORMAT2_DATA_WRITE_ACTION_FINALIZATION))
	{
		progress->get_ElapsedTime(&imapiStatus.elapsedTime);
		progress->get_RemainingTime(&imapiStatus.remainingTime);
		progress->get_TotalTime(&imapiStatus.totalTime);

	
	}
	
	if(pThis->m_callbackBurnStatus == NULL)
	{

	}
	pThis->m_callbackBurnStatus->BurnStatusCallBack(imapiStatus.totalTime,imapiStatus.remainingTime,burnStatus);

	return S_OK;
}
