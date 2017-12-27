/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */
#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogArial

IMPLEMENT_DYNAMIC(CFileDialogArial, CFileDialog)

CFileDialogArial::CFileDialogArial(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, FALSE)
{
    // Create the font to use
    m_font.CreatePointFont(100,"Arial Bold",NULL); // 10pt Arial Bold
}


BEGIN_MESSAGE_MAP(CFileDialogArial, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogArial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFileDialogArial::OnInitDialog() 
{
    // Tell each control in the file dialog to change to Arial Bold 10pt.
    CWnd *wndDlg = GetParent();
    wndDlg->SendMessageToDescendants(WM_SETFONT, (WPARAM)m_font.m_hObject);

    // Do the usual initialisation
	CFileDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
