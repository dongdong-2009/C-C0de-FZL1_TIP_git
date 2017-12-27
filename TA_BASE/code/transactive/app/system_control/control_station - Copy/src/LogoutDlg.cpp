/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/LogoutDlg.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is an extension of CDialog and is used to present user with
  * options of logging out, exitting control station and restarting workstation.
  * This class does not perform the above actions itself - that needs to be
  * done by whichever class that called it.
  */

#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\controlstation.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\LogoutDlg.h"
#include "bus\generic_gui\src\HelpLauncher.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "core\utilities\src\DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

/////////////////////////////////////////////////////////////////////////////
// LogoutDlg dialog


LogoutDlg::LogoutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LogoutDlg::IDD, pParent),
      m_terminateCode(TA_Base_App::TA_ControlStation::LOGOUT) // by default, just assume it's logout
{
	//{{AFX_DATA_INIT(LogoutDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void LogoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LogoutDlg)
	DDX_Control(pDX, IDC_HELP_BUTTON, m_helpCtrl);
	DDX_Control(pDX, IDCANCEL, m_cancelCtrl);
	DDX_Control(pDX, IDC_LOGOUT_BUTTON, m_logoutCtrl);
	DDX_Control(pDX, IDC_RESTART_BUTTON, m_restartCtrl);
	DDX_Control(pDX, IDC_EXIT_BUTTON, m_exitCtrl);
	DDX_Control(pDX, IDC_ANIMATE_AB, m_introMovie);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LogoutDlg, CDialog)
	//{{AFX_MSG_MAP(LogoutDlg)
	ON_BN_CLICKED(IDC_LOGOUT_BUTTON, OnLogoutButton)
	ON_BN_CLICKED(IDC_EXIT_BUTTON, OnExitButton)
	ON_BN_CLICKED(IDC_RESTART_BUTTON, OnRestartButton)
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LogoutDlg message handlers

void LogoutDlg::OnLogoutButton() 
{
    m_terminateCode = TA_Base_App::TA_ControlStation::LOGOUT;
    CDialog::OnOK();
}


void LogoutDlg::OnExitButton() 
{
    m_terminateCode = TA_Base_App::TA_ControlStation::EXIT;
    CDialog::OnOK();
}


void LogoutDlg::OnRestartButton() 
{
    m_terminateCode = TA_Base_App::TA_ControlStation::RESTART;
    CDialog::OnOK();
}


void LogoutDlg::OnHelpButton() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}


BOOL LogoutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Play the AVI intro movie.

    // Get the application's path
    char    szAppPath[MAX_PATH] = "";
    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

    // Extract directory
    CString aviPath = szAppPath;
    aviPath = aviPath.Left(aviPath.ReverseFind('\\'));

    // Append the AVI name
    aviPath += "\\";
    aviPath += TA_Base_App::TA_ControlStation::AVI_FILE.c_str();

    // Finally open and play the movie
    if( !m_introMovie.Open(aviPath) )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "AVI specified could not be found: %s", aviPath );
    }
    else
    {
	    m_introMovie.Play(0, -1, -1);
    }

    // Make the Window always on top.
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

    // Add "About..." menu item to system menu.
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
	    TA_VERIFY(strAboutMenu.LoadString(IDS_ABOUTBOX)!=0, "Unable to load IDS_ABOUTBOX");
	    if (!strAboutMenu.IsEmpty())
	    {
		    pSysMenu->AppendMenu(MF_SEPARATOR);
		    pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	    }
    }

    activateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Copied from ProfileLoginDlg so that both dialogs work consistently
void LogoutDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
    // In WM_SYSCOMMAND messages, the four low-order bits of the nID parameter 
    // are used internally by Windows CE. When an application tests the value of 
    // nID, it must combine the value 0xFFF0 with the nID value by using the 
    // bitwise-AND operator to obtain the correct result.
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void LogoutDlg::activateControls()
{
    int ctrlCount = 0;

    TA_Base_Core::ControlStation& controlStationEntity = SessionManager::getInstance().getControlStationEntity();

    bool shouldShowLogout(true);
    bool shouldShowExit(true);
    bool shouldShowRestart(true);

    try
    {
        shouldShowLogout = controlStationEntity.shouldShowLogoutButton();
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not determine if logout button should be shown so it will be shown");
    }

    try
    {
        shouldShowExit = controlStationEntity.shouldShowExitButton();
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not determine if exit button should be shown so it will be shown");
    }

    
    try
    {
        shouldShowRestart = controlStationEntity.shouldShowRestartButton();
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not determine if restart button should be shown so it will be shown");
    }

    if( shouldShowLogout )
    {
        m_logoutCtrl.ShowWindow(SW_SHOW);
        m_logoutCtrl.EnableWindow(true);
        ctrlCount++;
    }

    if( shouldShowExit )
    {
        m_exitCtrl.ShowWindow(SW_SHOW);
        if (SessionManager::getInstance().isExitAccessControlled())
        {
            TA_Base_App::TA_ControlStation::EPermission canExit = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_EXIT);
            bool enableExit = (canExit == TA_Base_App::TA_ControlStation::PERMITTED);
	        m_exitCtrl.EnableWindow(enableExit);
        }
        else
        {
	        m_exitCtrl.EnableWindow(true);
        }
        ctrlCount++;
    }

    if ( shouldShowRestart )
    {
        m_restartCtrl.ShowWindow(SW_SHOW);
        if (SessionManager::getInstance().isRestartAccessControlled())
        {
            TA_Base_App::TA_ControlStation::EPermission canRestart = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_RESTART);
            bool enableRestart = (canRestart == TA_Base_App::TA_ControlStation::PERMITTED);
            m_restartCtrl.EnableWindow(enableRestart);
        }
        else
        {
            m_restartCtrl.EnableWindow(true);
        }
        ctrlCount++;
    }

    if (ctrlCount == 3)
    {
        // if we're showing all the buttons, then don't worry about repositioning them
        // just need to show them as they are
        return;
    }

    // We need to arrange the buttons to make the dialog looks good
    ctrlCount += 2; // cancel and help buttons are always there
    // Get size of the main window
    CRect windowRect;
    GetWindowRect(&windowRect);

    // Get the size of each button - should be all the same, so just use m_help
    CRect buttonRect;
    m_helpCtrl.GetWindowRect(&buttonRect);

    int offset = windowRect.right - buttonRect.right;
    int unitWidth = 0;
    int gap = 0;
    int x, y, width, height;
    width = buttonRect.Width();
    height = buttonRect.Height();
    y = buttonRect.top - windowRect.top - GetSystemMetrics(SM_CYSIZE) - GetSystemMetrics(SM_CYSIZEFRAME);
    x = offset;

    if (ctrlCount > 2)  // if there's only cancel and help buttons, then don't worry about the spaces in between
    {
        int spaceInBetween = windowRect.Width() - 2 * (buttonRect.Width() + offset);
        gap = (spaceInBetween - (ctrlCount-2)*width)/(ctrlCount-1);

        unitWidth = width + gap;
    }

    int pos = 0;

    if (shouldShowLogout)
    {
        // don't need to move the logout button 'cos it's already at the right position
        pos++;
    }
    if (shouldShowExit)
    {
        if (pos != 0) // got a logout button before this, so recalculate position
        {
            // new x position = logout button's x + logout button's width + 1 gap 
            x += unitWidth;
        }
        m_exitCtrl.MoveWindow(x, y, width, height);
        pos++;
    }

    if (shouldShowRestart)
    {
        if (pos != 0)  // already got logout and/or exit button, recalculate position
        {
            x += unitWidth;
        }
        m_restartCtrl.MoveWindow(x, y, width, height);
        pos++;
    }

    if (pos != 0)  // already got logout and/or exit button, recalculate position
    {
        x += unitWidth;
    }
    m_cancelCtrl.MoveWindow(x, y, width, height);
    pos++;
}
