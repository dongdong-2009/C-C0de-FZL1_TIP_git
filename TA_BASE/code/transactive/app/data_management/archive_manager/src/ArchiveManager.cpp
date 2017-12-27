/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/ArchiveManager.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * User interface for Archive Operations
  *
  */

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/ArchiveManager.h"
#include "app/data_management/archive_manager/src/ArchiveManagerDlg.h"
#include "app/data_management/archive_manager/src/ArchiveManagerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // CArchiveManagerApp
    
    BEGIN_MESSAGE_MAP(CArchiveManagerApp, TransActiveWinApp)
        //{{AFX_MSG_MAP(CArchiveManagerApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
    END_MESSAGE_MAP()
    
    /////////////////////////////////////////////////////////////////////////////
    // CArchiveManagerApp construction
    
    CArchiveManagerApp::CArchiveManagerApp() : TransActiveWinApp(new ArchiveManagerGUI(), "Archive Manager")
    {
        // Place all significant initialization in InitInstance
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // The one and only CArchiveManagerApp object
    
    CArchiveManagerApp theApp;
    
    
} // TA_Base_App
