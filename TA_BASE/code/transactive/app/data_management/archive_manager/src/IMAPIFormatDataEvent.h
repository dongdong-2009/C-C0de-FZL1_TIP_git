#if !defined(_IMAPIFormatDataEvent_H__INCLUDED_)
#define _IMAPIFormatDataEvent_H__INCLUDED_
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/IMAPIFormatDataEvent.h $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/04/13 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Class to handle the updates from IMAPI write operation.
  */
#include <afx.h>
#include <afxctl.h>
#include <afxmt.h>
#include "imapi2.h"
#include "app/data_management/archive_manager/src/ICallbackBurnStatus.h"

#define WM_IMAPI_UPDATE		WM_APP+842

// CIMAPIFormatDataEvent command target
typedef struct _IMAPI_STATUS {
	
	LONG elapsedTime;		// Elapsed time in seconds
	LONG remainingTime;		// Remaining time in seconds
	LONG totalTime;			// total estimated time in seconds
	
} IMAPI_STATUS;

class CIMAPIFormatDataEvent : public CCmdTarget
{
	DECLARE_DYNAMIC(CIMAPIFormatDataEvent)
private:
	LPTYPEINFO  m_ptinfo;           // ITest type information
	DWORD		m_dwCookie;
	LPUNKNOWN	m_pUnkSink;
	LPUNKNOWN	m_pUnkSrc;

	ICallbackBurnStatus* m_callbackBurnStatus;

public:
	CIMAPIFormatDataEvent();
	virtual ~CIMAPIFormatDataEvent();
	
	/** CIMAPIFormatDataEvent
			*
			*	CreateEventSink
			*
			*	Establishes a link between the CDiscFormatData(IDiscFormat2Data)
			*	and the CIMAPIFormatDataEvent(DDiscFormat2DataEvents) so 
			*	CIMAPIFormatDataEvent can receive Update messages
			*
			*/
	static CIMAPIFormatDataEvent* CreateEventSink();
	
	/** CIMAPIFormatDataEvent
			*
			* ConnectDiscFormatData
			*
			*	Establishes a link between the CDiscFormatData(IDiscFormat2Data)
			*	CIMAPIFormatDataEvent can receive Update messages
			*
			*/
	bool ConnectDiscFormatData(IDiscFormat2Data* pDiscFormatData);
	void SetCallBackBurnStatus(ICallbackBurnStatus* callbackBurnStatus)
	{
		m_callbackBurnStatus = callbackBurnStatus;
	}

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(FormatDataEvents, DDiscFormat2DataEvents)
		
		 //IDispatch Methods
		
		STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
		STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
		STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames,
			UINT cNames, LCID lcid, DISPID FAR* rgdispid);
		STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid,
			WORD wFlags, DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
			EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr);
		
		 //DDiscFormat2DataEvents Methods
		
		/**  CIMAPIFormatDataEvent
			*
			*	Update
			*
			*	Receives update notifications from IDiscFormat2Data
			*
			*/
		STDMETHOD_(HRESULT, Update)(LPDISPATCH, LPDISPATCH);
	END_INTERFACE_PART(FormatDataEvents)

protected:
	DECLARE_MESSAGE_MAP()
};


#endif