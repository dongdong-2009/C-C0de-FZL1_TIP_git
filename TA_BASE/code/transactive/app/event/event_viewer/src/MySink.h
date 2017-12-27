#pragma once
#include "StdAfx.h"
/*#include "afxwin.h"*/
//#import "../../../../../../bin/win32_nd/ATSBridgeUtil.dll" named_guids
#ifdef _DEBUG
#import "../../../../../../sdk/win32_nd/ATSBridgeUtil.tlb" named_guids
#else
#import "../../../../../../sdk/win32_nr/ATSBridgeUtil.tlb" named_guids
#endif

/*#include <windows.h>*/
using namespace ATSBridgeUtilLib;

class CMySink :
	public CCmdTarget
{


	DECLARE_DYNAMIC(CMySink)

public:
	CMySink();
	virtual ~CMySink();

	virtual void OnFinalRelease();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	HRESULT OnEventUpdate(long Error, IATSEventList* pATSEvents);
};
