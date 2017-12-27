/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ExclusiveDutyDialog.cpp $
  * @author:  Azenith Arcilla
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists the exclusive duties of the current session
  *
  */

// ExclusiveDutyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "controlstation.h"
#include "ExclusiveDutyDialog.h"

#include "app/system_control/control_station/src/SessionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ExclusiveDutyDialog dialog


//ExclusiveDutyDialog::ExclusiveDutyDialog(CWnd* pParent /*=NULL*/)
ExclusiveDutyDialog::ExclusiveDutyDialog(CString subsystemsAtLocations, CWnd* pParent)
	: CDialog(ExclusiveDutyDialog::IDD, pParent), m_detail(subsystemsAtLocations)
{
	//{{AFX_DATA_INIT(ExclusiveDutyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ExclusiveDutyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ExclusiveDutyDialog)
	DDX_Control(pDX, IDC_EXCLUSIVE_DUTY_TEXT, m_textCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ExclusiveDutyDialog, CDialog)
	//{{AFX_MSG_MAP(ExclusiveDutyDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ExclusiveDutyDialog message handlers

void ExclusiveDutyDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void ExclusiveDutyDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


BOOL ExclusiveDutyDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

    // disable the 'x' button for now
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
    }

    // format the content
    m_textCtrl.SetWindowText(m_detail);
    m_textCtrl.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));

    // if we can get the current cursor position, then centre the dialog on that screen
    // otherwise, just centre it on the middle screen.
    CPoint pt;
    RECT screen, rect, newRect;
    if (!GetCursorPos(&pt))
    {
        // Make the Window always on top.
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);
        return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    screen = SessionManager::getInstance().getScreenPositioning().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
            TA_Base_Bus::TA_GenericGui::AREA_SCREEN,
            pt.x);

    // get our window rect
    GetWindowRect(&rect);

    int width = (rect.right - rect.left);
    newRect.left = screen.left + ((screen.right - screen.left) - width)/2;
    newRect.right = newRect.left + width;

    int height = (rect.bottom - rect.top);
    newRect.top = screen.top + ((screen.bottom - screen.top) - height)/2;
    newRect.bottom = newRect.top + height;

    // Make the Window always on top.
    SetWindowPos(&wndTopMost, newRect.left, newRect.top, width, height, SWP_NOSIZE);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
