/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/OpenFileDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// OpenFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "crutil.h"
#include "OpenFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        /////////////////////////////////////////////////////////////////////////////
        // OpenFileDialog

        IMPLEMENT_DYNAMIC(OpenFileDialog, CFileDialog)

        OpenFileDialog::OpenFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		        DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		        CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
        {
        }


        BEGIN_MESSAGE_MAP(OpenFileDialog, CFileDialog)
	        //{{AFX_MSG_MAP(OpenFileDialog)
	        ON_WM_DESTROY()
	        //}}AFX_MSG_MAP
        END_MESSAGE_MAP()

        void OpenFileDialog::OnDestroy() 
        {
	        CFileDialog::OnDestroy();
	        
	        // TODO: Add your message handler code here
	        
        }

        BOOL OpenFileDialog::OnInitDialog() 
        {
	        CFileDialog::OnInitDialog();
	        
	        // TODO: Add extra initialization here
	        
	        return TRUE;  // return TRUE unless you set the focus to a control
	                      // EXCEPTION: OCX Property Pages should return FALSE
        }

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
