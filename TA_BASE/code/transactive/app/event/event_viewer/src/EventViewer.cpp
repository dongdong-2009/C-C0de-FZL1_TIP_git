/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventViewer.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * EventViewer is the TransactiveWinApp derived main application class. This is the class that bootstraps the application.
 */

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/EventViewerDlg.h"
#include "app/event/event_viewer/src/EventViewerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const std::string EventViewerApp::RPARAM_ALARMID = "AlarmId";

/////////////////////////////////////////////////////////////////////////////
// EventViewerApp

BEGIN_MESSAGE_MAP(EventViewerApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(EventViewerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EventViewerApp construction

EventViewerApp theApp;

EventViewerApp::EventViewerApp()
: TransActiveWinApp( new EventViewerGUI(), "Event Viewer")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

BOOL EventViewerApp::InitInstance()
{
	BOOL res = TransActiveWinApp::InitInstance();
	return res;
}
