/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/saver.cpp $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Defines the class behaviors for the application.
  *
  */

#include "stdafx.h"

#include "app/system_control/transact_screensaver/src/Saver.h"
#include "app/system_control/transact_screensaver/src/saverwnd.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveMessage;

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CSaverApp
    BEGIN_MESSAGE_MAP(CSaverApp, CWinApp)
	    //{{AFX_MSG_MAP(CSaverApp)
		    // NOTE - the ClassWizard will add and remove mapping macros here.
		    //    DO NOT EDIT what you see in these blocks of generated code!
	    //}}AFX_MSG
	    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    END_MESSAGE_MAP()


    CSaverApp::CSaverApp()
    {
	    // TODO: add construction code here,
	    // Place all significant initialization in InitInstance
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only CSaverApp object
    CSaverApp theApp;

    //*****************************************************************************
    /////////////////////////////////////////////////////////////////////////////
    // CSaverApp initialization
    //-----------------------------------------------------------------------------
    BOOL CSaverApp::InitInstance()
    {
	    // Standard initialization
	    // If you are not using these features and wish to reduce the size
	    //  of your final executable, you should remove from the following
	    //  the specific initialization routines you do not need.

    #ifdef _AFXDLL
	    this->Enable3dControls();		// Call this when using MFC in a shared DLL
    #else
	    this->Enable3dControlsStatic();	// Call this when linking to MFC statically
    #endif

	    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	    if (FAILED(hr))
	    {
		    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to initialize CoInitializeEx");
	    }

	    //this->SetRegistryKey(IDS_COMPANYNAME);

	    HWND hWnd = NULL;
 
	    SCRMODE ScrMode = this->GetScrMode(hWnd);

	    if((ScrMode == SMCONFIG) || (ScrMode == SMPASSWORD))
	    {
		    DoConfig(hWnd);
	    }
	    else
	    {
		    CSaverWnd *pWnd = new CSaverWnd;
		    if(pWnd->Create(hWnd))
		    {
			    m_pMainWnd = pWnd;
			    return TRUE;
		    }
	    }
	    return FALSE;
    }
    
    
    CSaverApp::SCRMODE CSaverApp::GetScrMode(HWND &hWnd) const
    {
	    // parse the command line to determine what to do
	    // return value is SMNONE, SMSAVER, SMPREVIEW, SMCONFIG, or SMPASSWORD
	    // hWnd is the parent window handle
	    SCRMODE ScrMode = SMNONE;
	    hWnd = NULL;

	    TCHAR *pc = ::GetCommandLine();
	    ASSERT(pc != NULL);

	    if (*pc == _T('\"'))
	    {
		    pc++;
		    while (*pc != _T('\0') && *pc != _T('\"'))
		    {
			    pc++;
		    }
	    }
	    else
	    {
		    while (*pc != _T('\0') && *pc != _T(' '))
		    {
			    pc++;
		    }
	    }

	    if (*pc != _T('\0'))
	    {
		    pc++;
	    }

	    while (*pc == _T(' '))
	    {
		    pc++;
	    }

	    if (*pc == _T('\0'))
	    {
		    ScrMode = SMCONFIG;
		    hWnd = NULL;
	    }
	    else
	    {
		    if (*pc == _T('-') || *pc == _T('/'))
		    {
			    pc++;
		    }

		    if (*pc == _T('s') || *pc == _T('S'))
		    {
			    ScrMode = SMSAVER;
		    }
		    else
		    {
			    if (*pc == _T('p') || *pc == _T('P') ||
				    *pc == _T('l') || *pc == _T('L'))
			    {
				    pc++;
				    while (*pc == _T(' ') || *pc == _T(':'))
				    {
					    pc++;
				    }

				    ScrMode = SMPREVIEW;
				    hWnd = reinterpret_cast<HWND>(::_ttoi(pc));
			    }
			    else
			    {
				    if (*pc == _T('c') || *pc == _T('C'))
				    {
					    pc++;
					    while (*pc == _T(' ') || *pc == _T(':'))
					    {
						    pc++;
					    }
					    
					    ScrMode = SMCONFIG;
					    hWnd = (*pc == _T('\0'))
						       ? ::GetForegroundWindow()
						       : reinterpret_cast<HWND>(::_ttoi(pc));
				    }
				    else
				    {
					    if (*pc == _T('a') || *pc == _T('A'))
					    {
						    pc++;
						    while (*pc == _T(' ') || *pc == _T(':'))
						    {
							    pc++;
						    }
						    
						    ScrMode = SMPASSWORD;
						    hWnd = reinterpret_cast<HWND>(::_ttoi(pc));
					    }
				    }
			    }
		    }
	    }

	    return ScrMode;
    }

    
    void CSaverApp::DoConfig(HWND hWndParent) const
    {
	    CString action = "This screen saver has no options that you can set.";
	    TransActiveMessage userMsg;		
	    userMsg << action ;
	    userMsg.showMsgBox(IDS_UI_050063);				

    }


    int CSaverApp::ExitInstance() 
    {
        CoUninitialize();
	    return CWinApp::ExitInstance();
    }
}
