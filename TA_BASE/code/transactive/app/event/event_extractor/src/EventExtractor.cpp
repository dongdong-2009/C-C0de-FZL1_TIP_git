/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_extractor/src/EventExtractor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// EventExtractor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EventExtractor.h"
#include "EventExtractorDlg.h"

#include <core/utilities/src/RunParams.h>
#include "bus/generic_gui/src/HelpLauncher.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EventExtractor

BEGIN_MESSAGE_MAP(EventExtractor, CWinApp)
	//{{AFX_MSG_MAP(EventExtractor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EventExtractor construction

EventExtractor::EventExtractor()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only EventExtractor object

EventExtractor theApp;

/////////////////////////////////////////////////////////////////////////////
// EventExtractor initialization

BOOL EventExtractor::InitInstance()
{


	TA_Base_Core::RunParams::getInstance().parseCmdLine(GetCommandLine());

	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_VERSION))
	{
		// Show the version number of the application
		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	EventExtractorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
