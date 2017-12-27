// OverRideExclusiveDuty.cpp : implementation file
// added for TD19409

#include "stdafx.h"
#include "controlstation.h"
#include "OverRideExclusiveDuty.h"

#include "app/system_control/control_station/src/SessionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OverRideExclusiveDuty dialog


OverRideExclusiveDuty::OverRideExclusiveDuty(CString subsystemsAtLocations,CWnd* pParent /*=NULL*/)
	: CDialog(OverRideExclusiveDuty::IDD, pParent), m_detail(subsystemsAtLocations)
{
	//{{AFX_DATA_INIT(OverRideExclusiveDuty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/*
ExclusiveDutyDialog::ExclusiveDutyDialog(CString subsystemsAtLocations, CWnd* pParent)
	: CDialog(ExclusiveDutyDialog::IDD, pParent), m_detail(subsystemsAtLocations)
*/


void OverRideExclusiveDuty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OverRideExclusiveDuty)
	DDX_Control(pDX, IDC_EXCLUSIVE_DUTY_TEXT, m_textCtrl);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OverRideExclusiveDuty, CDialog)
	//{{AFX_MSG_MAP(OverRideExclusiveDuty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OverRideExclusiveDuty message handlers

void OverRideExclusiveDuty::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void OverRideExclusiveDuty::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL OverRideExclusiveDuty::OnInitDialog()
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
