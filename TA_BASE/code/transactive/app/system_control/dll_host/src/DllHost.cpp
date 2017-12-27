
// DllHost.cpp : Defines the class behaviors for the application.
//

#include "app/system_control/dll_host/src/stdafx.h"
#include "app/system_control/dll_host/src/DllHost.h"
#include "app/system_control/dll_host/src/DllHostGUI.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"


// CDllHostApp

BEGIN_MESSAGE_MAP(CDllHostApp, TransActiveWinApp)
END_MESSAGE_MAP()


// CDllHostApp construction

CDllHostApp::CDllHostApp()
    : TransActiveWinApp( new TA_Base_App::DllHostGUI(), "Control Station Dll Host" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDllHostApp object

CDllHostApp theApp;


// CDllHostApp initialization

BOOL CDllHostApp::InitInstance()
{
    return TransActiveWinApp::InitInstance();
}
