
// DllHost.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "app/system_control/dll_host/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"


// CDllHostApp:
// See DllHost.cpp for the implementation of this class
//

class CDllHostApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CDllHostApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDllHostApp theApp;
