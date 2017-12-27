/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class extends a list control. It is a list control where each of the icons
  * is a button. Each of those buttons is linked to an application that can be
  * launched by the user.
  * 
  * This is the class for the Control Station launch bar dialogue. It basically sets
  * up all the controls and then delegates commands when the buttons and controls are
  * used.
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/ControlStation.h"
#include "app/system_control/control_station/src/ControlStationBar.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"
#include "app/system_control/control_station/src/ScreenPositioning.h"
#include "app/system_control/control_station/src/IGraphWorxManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "bus/resources/resource.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus\application_types\src\AppTypes.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "bus\user_settings\src\SettingsMgr.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GraphWorxException.h"
#include "core\exceptions\src\UserSettingsException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/ScadaDisplay.h"
#include "core\configuration_updates\src\OnlineUpdateListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_App::TA_ControlStation::APPLICATION_NAME;

// This indicates how much the button colour should change when the mouse is moved over
// them (ie they become hot).
const int ControlStationBar::BUTTON_COLOUR_OFFSET_WHEN_HOT = 30;

// This is the text to be used for the Operator Override tool button.
const CString ControlStationBar::APPLY_OVERRIDE_BUTTON_TEXT   = "Override";
const CString ControlStationBar::APPLY_OVERRIDE_TOOLTIP_TEXT  = "Apply Operator Override";
const CString ControlStationBar::REMOVE_OVERRIDE_BUTTON_TEXT  = "Remove";
const CString ControlStationBar::REMOVE_OVERRIDE_TOOLTIP_TEXT = "Remove Operator Override";

// These are the formats of the text that will describe who is logged in. If the format is
// changed and the placeholders (ie % items) are changed in any way then the place where this
// constant is used must be changed as well.
const CString ControlStationBar::LOGIN_DETAILS_WHEN_OVERRIDE_FORMAT = "%s / %s\n%s / %s";
const CString ControlStationBar::LOGIN_DETAILS_WHEN_NO_OVERRIDE_FORMAT = "%s\n%s";

// Format of the text to save into the USER_SETTINGS table.
// Shortcut names are in the form of "Shortcut00", "Shortcut01", ..., "Shortcut19", "Shortcut<MAX_SHORTCUTS-1>"
// We just need 2 digits for the number as having 100 shortcuts is more than enough
// Shortcut values are in the form of "My new shortcut name\display number"
const CString ControlStationBar::SHORTCUT_SETTING_NAME_FORMAT = "%s%.2d";
const CString ControlStationBar::SHORTCUT_SETTING_VALUE_FORMAT = "%s\\%d"; 


// This is the id of the clock timer that updates the time and date in the launch bar
const UINT CLOCK_TIMER = 123;


ControlStationBar::ControlStationBar(CWnd* pParent /*=NULL*/)
	: m_deviceCombo(this), m_isTaskBarIconShowing(false), CDialog(ControlStationBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(ControlStationBar)
	//}}AFX_DATA_INIT
}


void ControlStationBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ControlStationBar)
	DDX_Control(pDX, IDC_FAVOURITES_COMBO, m_favouritesComboBox);
	DDX_Control(pDX, IDC_DATE_TIME_DETAILS, m_dateTimeDetails);
	DDX_Control(pDX, IDC_DISPLAY_BUTTON5, m_displayButton5);
	DDX_Control(pDX, IDC_DISPLAY_BUTTON4, m_displayButton4);
	DDX_Control(pDX, IDC_DISPLAY_BUTTON3, m_displayButton3);
	DDX_Control(pDX, IDC_DISPLAY_BUTTON2, m_displayButton2);
	DDX_Control(pDX, IDC_DISPLAY_BUTTON1, m_displayButton1);
	DDX_Control(pDX, IDC_DEVICE_COMBO, m_deviceCombo);
	DDX_Control(pDX, IDC_USER_DETAILS, m_userDetails);
	DDX_Control(pDX, IDC_VIEW_CAMERA_BUTTON, m_launchCameraView);
	DDX_Control(pDX, IDC_DISPLAY_SCHEMATIC_BUTTON, m_launchSchematicDisplay);
	DDX_Control(pDX, IDC_LOGOUT_BUTTON, m_logoutButton);
	DDX_Control(pDX, IDC_HELP_BUTTON, m_helpButton);
	DDX_Control(pDX, IDC_OVERRIDE_BUTTON, m_operatorOverrideButton);
	DDX_Control(pDX, IDC_PASSWORD_BUTTON, m_changePasswordButton);
	DDX_Control(pDX, IDC_OPERATOR_BUTTON, m_changeOperatorButton);
	DDX_Control(pDX, IDC_PROFILE_BUTTON, m_changeProfileButton);
	DDX_Control(pDX, IDC_LIST_APPLICATIONS, m_launchList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ControlStationBar, CDialog)
	//{{AFX_MSG_MAP(ControlStationBar)
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
	ON_BN_CLICKED(IDC_LOGOUT_BUTTON, OnLogoutButton)
	ON_BN_CLICKED(IDC_OVERRIDE_BUTTON, OnOverrideButton)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DISPLAY_SCHEMATIC_BUTTON, OnDisplaySchematicButton)
	ON_BN_CLICKED(IDC_VIEW_CAMERA_BUTTON, OnViewCameraButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_DISPLAY_BUTTON1, OnDisplayButton1)
	ON_BN_CLICKED(IDC_DISPLAY_BUTTON2, OnDisplayButton2)
	ON_BN_CLICKED(IDC_DISPLAY_BUTTON3, OnDisplayButton3)
	ON_BN_CLICKED(IDC_DISPLAY_BUTTON4, OnDisplayButton4)
	ON_BN_CLICKED(IDC_DISPLAY_BUTTON5, OnDisplayButton5)
	ON_BN_CLICKED(IDC_OPERATOR_BUTTON, OnOperatorButton)
	ON_BN_CLICKED(IDC_PROFILE_BUTTON, OnProfileButton)
	ON_BN_CLICKED(IDC_PASSWORD_BUTTON, OnPasswordButton)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EDIT_SHORTCUT_BUTTON, OnEditShortcutButton)
	ON_BN_CLICKED(IDC_VIEW_SHORTCUT_BUTTON, OnViewShortcutButton)
	ON_LBN_DBLCLK(IDC_SHORTCUT_LIST, OnDblclkShortcutList)
	ON_CBN_EDITCHANGE(IDC_FAVOURITES_COMBO, OnEditchangeFavouritesCombo)
	ON_BN_CLICKED(IDC_PASSWORD_BUTTON, OnPasswordButton)
	ON_CBN_SELCHANGE(IDC_FAVOURITES_COMBO, OnSelchangeFavouritesCombo)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_NOTIFYICON, OnNotifyIcon)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ControlStationBar message handlers

BOOL ControlStationBar::OnInitDialog() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnInitDialog");

	CDialog::OnInitDialog();

    calculateDialogLayout();

    initialiseViewDisplayButtons();

    initialiseToolButtons();

    initialiseQuickLaunchButtons();

    initialiseShortcutButtons();

    m_deviceCombo.loadEntities();

    // Register interest for online updates of all scada displays so that our internal
    // display list that is used for displaying shortcuts is always up to date
    TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::SCADA_DISPLAY, *this);
    
    // Set the timer to go off every 1000 milliseconds to update the clock
    SetTimer(CLOCK_TIMER, 1000, 0);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnInitDialog");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void ControlStationBar::OnClose() 
{
	// We do not want this dialog to be closed so we ensure that
    // close is never called by commenting it out.
	//CDialog::OnClose();
}


void ControlStationBar::OnCancel() 
{
    // We do not want this dialog to be hidden or closed when
    // ESC is pressed so we comment out the OnCancel code.
	//CDialog::OnCancel();
}


void ControlStationBar::OnOK() 
{
    // We do not want this dialog to be hidden or closed when
    // Enter is pressed so we comment out the OnOK code.
	//CDialog::OnOK();
}


BOOL ControlStationBar::DestroyWindow() 
{
	// stop the clock timer
	KillTimer(CLOCK_TIMER);

    unloadShortcuts();
    m_editShortcutDlg.cleanUp();

    unsigned int i;
    for (i = 0; i < m_shortcuts.size(); i++)
    {
        delete m_shortcuts[i];
        m_shortcuts[i] = NULL;
    }
    m_shortcuts.clear();

    if (m_settingsMgr != NULL)
    {
        delete m_settingsMgr;
        m_settingsMgr = NULL;
    }

	return CDialog::DestroyWindow();
}


LRESULT ControlStationBar::OnNotifyIcon(WPARAM wParam, LPARAM lParam) 
{ 
    // This means the icon in the task bar has been clicked. We
    // want to ensure the Control Station comes to the front

    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnNotifyIcon");

    UINT uID; 
    UINT uMouseMsg; 
 
    uID = (UINT) wParam; 
    uMouseMsg = (UINT) lParam; 
 
    if (uMouseMsg == WM_LBUTTONDOWN)
    { 
        SetForegroundWindow();
    } 

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnNotifyIcon");
    return NULL; 
} 


void ControlStationBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
    // The task bar icon should not be showing when this bar is hidden.
    // This adds and deletes it depending on what is showing.
	if (bShow)
    {
        addTaskBarIcon();
    }
    else
    {
        deleteTaskBarIcon();
    }
}


void ControlStationBar::OnHelpButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnHelpButton");

	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnHelpButton");
}


void ControlStationBar::OnLogoutButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnLogoutButton");

    ControlStationRootDialog::getInstance().PostMessage(WM_LOGOUT);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnLogoutButton");
}


void ControlStationBar::OnOverrideButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnOverrideButton");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_OVERRIDE_DIALOG);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnOverrideButton");
}


void ControlStationBar::OnOperatorButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnOperatorButton");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_CHANGE_OPERATOR_DIALOG);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnOperatorButton");
}


void ControlStationBar::OnProfileButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnProfileButton");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_CHANGE_PROFILE_DIALOG);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnProfileButton");
}


void ControlStationBar::OnPasswordButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnPasswordButton");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_CHANGE_PASSWORD_DIALOG);

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnPasswordButton");
}


void ControlStationBar::OnDisplaySchematicButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplaySchematicButton");

    m_deviceCombo.viewSchematicDisplayPressed();

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplaySchematicButton");
}


void ControlStationBar::OnViewCameraButton() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnViewCameraButton");

    m_deviceCombo.cameraViewPressed();

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnViewCameraButton");
}


void ControlStationBar::OnDisplayButton1() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplayButton1");

    // Monitor number linked to this button was stored as associated data
    launchDisplay( m_displayButton1.GetAssociatedData() );

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplayButton1");
}


void ControlStationBar::OnDisplayButton2() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplayButton2");

    // Monitor number linked to this button was stored as associated data
    launchDisplay( m_displayButton2.GetAssociatedData() );

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplayButton2");
}


void ControlStationBar::OnDisplayButton3() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplayButton3");

    // Monitor number linked to this button was stored as associated data
    launchDisplay( m_displayButton3.GetAssociatedData() );

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplayButton3");
}


void ControlStationBar::OnDisplayButton4() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplayButton4");

    // Monitor number linked to this button was stored as associated data
    launchDisplay( m_displayButton4.GetAssociatedData() );

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplayButton4");
}


void ControlStationBar::OnDisplayButton5() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnDisplayButton5");

    // Monitor number linked to this button was stored as associated data
    launchDisplay( m_displayButton5.GetAssociatedData() );

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnDisplayButton5");
}


void ControlStationBar::OnTimer(UINT nIDEvent) 
{
	// if it's the clock timer
	if (CLOCK_TIMER == nIDEvent)
	{
		// Display the date and time in the Launch Bar
        m_dateTimeDetails.SetWindowText(COleDateTime::GetCurrentTime().Format());
	}
	
	CDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// ControlStationBar - Other Methods

void ControlStationBar::calculateDialogLayout()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateDialogLayout");

    // Calculate the height the Control Station Launch Bar can be.
    RECT rect = SessionManager::getInstance().getScreenPositioning().getLaunchBarRect();

    if ( (rect.right == 0) && (rect.bottom == 0) )
    {
        // We are not meant to be displaying a launch bar so clean up and get out of here
        deleteTaskBarIcon();
        EndDialog(IDOK);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateDialogLayout");
        return;
    }

    // Now position the launch bar
    MoveWindow(&rect);

    LOG( SourceInfo, DebugUtil::FunctionExit, "calculateDialogLayout");
}


void ControlStationBar::initialiseViewDisplayButtons()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialiseViewDisplayButtons");

    // Determine how many monitors we have and show/hide and position the buttons as required
    unsigned long numberOfMonitors = SessionManager::getInstance().getScreenPositioning().getNumberOfMonitors();

    switch (numberOfMonitors)
    {
    case 1:
        // 1 monitor so just show centre button.
        m_displayButton1.ShowWindow(SW_HIDE);
        m_displayButton2.ShowWindow(SW_HIDE);
        m_displayButton4.ShowWindow(SW_HIDE);
        m_displayButton5.ShowWindow(SW_HIDE);

        // Assign icon and colour to the button
        m_displayButton3.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton3.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton3.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton3.SetTooltipText("Load display on centre monitor");
        m_displayButton3.SetAssociatedData(1); // Set the screen number this button represents
	    m_displayButton3.EnableWindow(TRUE);
        
        break;

    case 2:
        // 2 monitors so just show monitors 2 and 4
        m_displayButton1.ShowWindow(SW_HIDE);
        m_displayButton3.ShowWindow(SW_HIDE);
        m_displayButton5.ShowWindow(SW_HIDE);

        // Assign icons and colour to the buttons
        m_displayButton2.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton2.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton2.SetTooltipText("Load display on centre monitor");
        m_displayButton2.SetAssociatedData(1); // Set the screen number this button represents
	    m_displayButton2.EnableWindow(TRUE);

        m_displayButton4.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton4.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton4.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton4.SetTooltipText("Load display on centre monitor");
        m_displayButton4.SetAssociatedData(2); // Set the screen number this button represents
	    m_displayButton4.EnableWindow(TRUE);
        break;

    case 3:
        // 3 monitors so show the centre 3 (1,3 and 5)
        m_displayButton2.ShowWindow(SW_HIDE);
        m_displayButton4.ShowWindow(SW_HIDE);

        // Assign icons and colour to the buttons
        m_displayButton1.SetIcon(IDI_MONITOR_LEFT,(int)BTNST_AUTO_GRAY);
	    m_displayButton1.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton1.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton1.SetTooltipText("Load display on centre monitor");
        m_displayButton1.SetAssociatedData(1); // Set the screen number this button represents
	    m_displayButton1.EnableWindow(TRUE);

        m_displayButton3.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton3.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton3.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton3.SetTooltipText("Load display on centre monitor");
        m_displayButton3.SetAssociatedData(2); // Set the screen number this button represents
	    m_displayButton3.EnableWindow(TRUE);

        m_displayButton5.SetIcon(IDI_MONITOR_RIGHT,(int)BTNST_AUTO_GRAY);
	    m_displayButton5.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton5.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton5.SetTooltipText("Load display on centre monitor");
        m_displayButton5.SetAssociatedData(3); // Set the screen number this button represents
	    m_displayButton5.EnableWindow(TRUE);
        break;

    default:
        // 4 monitors (or more) so hide the centre monitor
        m_displayButton3.ShowWindow(SW_HIDE);

        // Assign icons and colour to the buttons
        m_displayButton1.SetIcon(IDI_MONITOR_LEFT,(int)BTNST_AUTO_GRAY);
	    m_displayButton1.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton1.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton1.SetTooltipText("Load display on centre monitor");
        m_displayButton1.SetAssociatedData(1); // Set the screen number this button represents
	    m_displayButton1.EnableWindow(TRUE);

        m_displayButton2.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton2.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton2.SetTooltipText("Load display on centre monitor");
        m_displayButton2.SetAssociatedData(2); // Set the screen number this button represents
	    m_displayButton2.EnableWindow(TRUE);

        m_displayButton4.SetIcon(IDI_MONITOR_CENTER,(int)BTNST_AUTO_GRAY);
	    m_displayButton4.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton4.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton4.SetTooltipText("Load display on centre monitor");
        m_displayButton4.SetAssociatedData(3); // Set the screen number this button represents
	    m_displayButton4.EnableWindow(TRUE);

        m_displayButton5.SetIcon(IDI_MONITOR_RIGHT,(int)BTNST_AUTO_GRAY);
	    m_displayButton5.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	    m_displayButton5.SetAlign(CButtonST::ST_ALIGN_VERT);
	    m_displayButton5.SetTooltipText("Load display on centre monitor");
        m_displayButton5.SetAssociatedData(4); // Set the screen number this button represents
	    m_displayButton5.EnableWindow(TRUE);
        break;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "initialiseViewDisplayButtons");
}

    
void ControlStationBar::initialiseToolButtons()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialiseToolButtons");

    // Assign icons and colours to the buttons.
    m_helpButton.SetIcon(IDI_HELP,(int)BTNST_AUTO_GRAY);
	m_helpButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_helpButton.SetAlign(CButtonST::ST_ALIGN_VERT);
	m_helpButton.SetTooltipText("Launch Help");
	m_helpButton.EnableWindow(TRUE);

	m_logoutButton.SetIcon(IDI_LOGOUT,(int)BTNST_AUTO_GRAY);
	m_logoutButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_logoutButton.SetAlign(CButtonST::ST_ALIGN_VERT);
	m_logoutButton.SetTooltipText("Logout");
	m_logoutButton.EnableWindow(TRUE);

	m_operatorOverrideButton.SetIcon(IDI_OPERATOR_OVERRIDE,(int)BTNST_AUTO_GRAY);
	m_operatorOverrideButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_operatorOverrideButton.SetAlign(CButtonST::ST_ALIGN_VERT);
    m_operatorOverrideButton.SetTooltipText("Apply Operator Override");

	m_changePasswordButton.SetIcon(IDI_OPERATOR_PASSWORD,(int)BTNST_AUTO_GRAY);
	m_changePasswordButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_changePasswordButton.SetAlign(CButtonST::ST_ALIGN_VERT);
    m_changePasswordButton.SetTooltipText("Change Password");

	m_changeOperatorButton.SetIcon(IDI_OPERATOR_SHIFT_CHANGE,(int)BTNST_AUTO_GRAY);
	m_changeOperatorButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_changeOperatorButton.SetAlign(CButtonST::ST_ALIGN_VERT);
    m_changeOperatorButton.SetTooltipText("Change Operator");

	m_changeProfileButton.SetIcon(IDI_OPERATOR_PROFILE_CHANGE,(int)BTNST_AUTO_GRAY);
	m_changeProfileButton.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_changeProfileButton.SetAlign(CButtonST::ST_ALIGN_VERT);
    m_changeProfileButton.SetTooltipText("Change Profile");

    refreshQuickToolsButtons();

    LOG( SourceInfo, DebugUtil::FunctionExit, "initialiseToolButtons");
}


void ControlStationBar::initialiseQuickLaunchButtons()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialiseQuickLaunchButtons");

    // Assign icons and colours to the buttons.
	m_launchCameraView.SetIcon(IDI_VIEW_CAMERA,(int)BTNST_AUTO_GRAY);
	m_launchCameraView.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_launchCameraView.SetAlign(CButtonST::ST_ALIGN_VERT);
	m_launchCameraView.SetTooltipText("Camera View");
	m_launchCameraView.EnableWindow(FALSE);

	m_launchSchematicDisplay.SetIcon(IDI_VIEW_SCHEMATIC,(int)BTNST_AUTO_GRAY);
	m_launchSchematicDisplay.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, BUTTON_COLOUR_OFFSET_WHEN_HOT);
	m_launchSchematicDisplay.SetAlign(CButtonST::ST_ALIGN_VERT);
	m_launchSchematicDisplay.SetTooltipText("Display Schematic");
	m_launchSchematicDisplay.EnableWindow(FALSE);

    LOG( SourceInfo, DebugUtil::FunctionExit, "initialiseQuickLaunchButtons");
}


void ControlStationBar::initialiseShortcutButtons()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialiseShortcutButtons");

    // Shortcut buttons have been deleted, but still want to initialise the scada displays, so this method stays

    // get the list of displays here too since we're going to use this to initialise the shortcut list
    m_allDisplays = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getAllScadaDisplays();

    m_settingsMgr = NULL;

    LOG( SourceInfo, DebugUtil::FunctionExit, "initialiseShortcutButtons");
}


void ControlStationBar::addTaskBarIcon() 
{ 
    LOG( SourceInfo, DebugUtil::FunctionEntry, "addTaskBarIcon");

    if (m_isTaskBarIconShowing)
    {
        // Already showing so just return
        LOG( SourceInfo, DebugUtil::FunctionExit, "addTaskBarIcon");
        return;
    }
    
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    HICON hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_hWnd; 
    tnid.uID = IDR_MAINFRAME; 
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnid.uCallbackMessage = WM_NOTIFYICON; 
    tnid.hIcon = hicon; 
    lstrcpyn(tnid.szTip, APPLICATION_NAME, sizeof(tnid.szTip)); 
 
    res = Shell_NotifyIcon(NIM_ADD, &tnid); 
 
    if (hicon)
    {
        DestroyIcon(hicon); 
    }
 
    m_isTaskBarIconShowing = true;

    LOG( SourceInfo, DebugUtil::FunctionExit, "addTaskBarIcon");
} 


void ControlStationBar::deleteTaskBarIcon() 
{ 
    LOG( SourceInfo, DebugUtil::FunctionEntry, "deleteTaskBarIcon");

    if (!m_isTaskBarIconShowing)
    {
        // Already not showing so just return
        LOG( SourceInfo, DebugUtil::FunctionExit, "deleteTaskBarIcon");
        return;
    }

    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_hWnd; 
    tnid.uID = IDR_MAINFRAME; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 

    m_isTaskBarIconShowing = false;

    LOG( SourceInfo, DebugUtil::FunctionExit, "deleteTaskBarIcon");
} 


void ControlStationBar::launchDisplay(unsigned long screenNumber)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "launchDisplay");

    if (screenNumber == 0)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020044);
        LOG( SourceInfo, DebugUtil::FunctionExit, "launchDisplay");
        return;
    }

    try
    {
        IGraphWorxManager* graphworx = SessionManager::getInstance().getGraphWorxManager();
        if (graphworx == NULL)        
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020044);
            LOG( SourceInfo, DebugUtil::FunctionExit, "launchDisplay");
            return;
        }

        graphworx->copyGraphWorxDisplay(screenNumber);
    }
    catch ( const TA_Base_Core::GraphWorxException& )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020044);
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "launchDisplay");
}


void ControlStationBar::refreshQuickToolsButtons()
{
    bool enable = (SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_OVERRIDE) == TA_Base_App::TA_ControlStation::PERMITTED);
    m_operatorOverrideButton.EnableWindow(enable);

    enable = (SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_CHANGE_PASSWORD) == TA_Base_App::TA_ControlStation::PERMITTED);
	m_changePasswordButton.EnableWindow(enable);

    enable = (SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_CHANGE_PROFILE) == TA_Base_App::TA_ControlStation::PERMITTED);
	m_changeProfileButton.EnableWindow(enable);
}


void ControlStationBar::OnEditShortcutButton() 
{
    // if user cancelled, then just return
    m_editShortcutDlg.initialise(m_shortcuts, m_allDisplays, m_favouritesComboBox.GetCurSel());
    if (m_editShortcutDlg.DoModal() == IDCANCEL)
    {
        return;
    }

    synchroniseShortcuts();
    refreshShortcutListBox();
}


void ControlStationBar::OnViewShortcutButton() 
{
    int index = m_favouritesComboBox.GetCurSel();
    if (index == CB_ERR)
    {
        return;
    }

    // if for some reason the display path is empty, which should be, we'll just return.
    if (m_allDisplays[index]->getPath().empty())
    {
        return;
    }

    RECT rect = SessionManager::getInstance().getScreenPositioning().getAppRectNextToLaunchBar();
    DisplayInformation* info = new struct DisplayInformation;
    std::string path;
    try
    {
        path = m_allDisplays[index]->getPath();
    }
    catch (...)
    {
    }

    if ((info == NULL) || path.empty())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Unable to create new DisplayInformation or path is empty");
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020064);
        return;
    }

    info->display = CString(path.c_str());
    info->asset = "";  // TODO: might want to consider having asset as a configurable item in the EditShortcutDialog
    info->position = (LPARAM)rect.left;
    ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
}


void ControlStationBar::OnDblclkShortcutList() 
{
    // just the same as clicking the 'shortcut' button
    OnViewShortcutButton();	
}


void ControlStationBar::loginDetailsChanged()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loginDetailsChanged");

    SessionDetails& session = SessionManager::getInstance().getSessionDetails();
    if (session.getSessionId().empty())
    {
        // no active session, so hide this window
        ShowWindow(SW_HIDE);
        unloadShortcuts();
        return;
    }

    // This will be set up to contain the current login details. This will be set in the
    // m_userDetails control.
    CString userDetailString("");


    bool overrideChange = false;

    CString previousText;
    m_operatorOverrideButton.GetWindowText(previousText);
    if( session.isOperatorOverridden() )
    {
        if (previousText.Compare(APPLY_OVERRIDE_BUTTON_TEXT) == TA_Base_App::TA_ControlStation::STRINGS_EQUIVALENT)
        {
            overrideChange = true;
        }
        userDetailString.Format(LOGIN_DETAILS_WHEN_OVERRIDE_FORMAT,
                                session.getOperatorName().c_str(),
                                session.getOverrideOperatorName().c_str(),
                                session.getProfileAndLocationDisplayString().c_str(),
                                session.getOverrideProfileAndLocationDisplayString().c_str());

        // Ensure the operator override button is indicating the correct action as the same
        // button is used to both add and remove an operator override
        m_operatorOverrideButton.SetWindowText(REMOVE_OVERRIDE_BUTTON_TEXT);
	    m_operatorOverrideButton.SetTooltipText(REMOVE_OVERRIDE_TOOLTIP_TEXT);
        m_userDetails.SetWindowText( userDetailString );
    }
    else
    {
        if (previousText.Compare(REMOVE_OVERRIDE_BUTTON_TEXT) == TA_Base_App::TA_ControlStation::STRINGS_EQUIVALENT)
        {
            overrideChange = true;
        }
        userDetailString.Format(LOGIN_DETAILS_WHEN_NO_OVERRIDE_FORMAT,
                                session.getOperatorName().c_str(),
                                session.getProfileAndLocationDisplayString().c_str() );

        // Ensure the operator override button is indicating the correct action as the same
        // button is used to both add and remove an operator override
        m_operatorOverrideButton.SetWindowText(APPLY_OVERRIDE_BUTTON_TEXT);
	    m_operatorOverrideButton.SetTooltipText(APPLY_OVERRIDE_TOOLTIP_TEXT);
        m_userDetails.SetWindowText( userDetailString );
    }

    // if it is change profile, change operator or new login, then we need save the changes made to 
    // the shortcuts in last session and load the shortcuts for the new session
    if (!overrideChange)
    {
        unloadShortcuts();
        loadShortcuts();
    }

    // Tell the list of launch applications to refresh it's list (as the available applications
    // may now have changed.
    m_launchList.reset();

    // refresh the quick tools buttons
    refreshQuickToolsButtons();

    LOG( SourceInfo, DebugUtil::FunctionExit, "loginDetailsChanged");
}


void ControlStationBar::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

    if (updateEvent.getType() != TA_Base_Core::SCADA_DISPLAY)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Not scada display update, ignore");
        LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
        return;
    }

    unsigned long key = updateEvent.getKey();

    switch(updateEvent.getModifications())
    {
    case TA_Base_Core::Update:
        {
            ScadaDisplayVector::iterator iter = m_allDisplays.begin();
            ScadaDisplayVector::iterator end = m_allDisplays.end();

            while (iter != end)
            {
                if ((*iter)->getKey() == key)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "scada display %d has been updated", key);
                    (*iter)->invalidate();
                    break;
                }
                ++iter;
            }
        }
        break;
    case TA_Base_Core::Delete:
        {
            ScadaDisplayVector::iterator iter = m_allDisplays.begin();
            ScadaDisplayVector::iterator end = m_allDisplays.end();
            while (iter != end)
            {
                if ((*iter)->getKey() == key)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Deleting scada display %d from the scada display list", key);
                    delete (*iter);
                    *iter = NULL;
                    m_allDisplays.erase(iter);
                    break;
                }
                ++iter;
            }
        }
        break;
    case TA_Base_Core::Create:
        {
            bool found = false;
            // see if it is already there first
            ScadaDisplayVector::iterator iter = m_allDisplays.begin();
            ScadaDisplayVector::iterator end = m_allDisplays.end();

            while (iter != end)
            {
                if ((*iter)->getKey() == key)
                {
                    found = true;
                    break;
                }
                ++iter;
            }

            if (found)
            {
                // no need to add again 'cos it's already there
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "scada display %d has already been added no need to add again", key);
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Adding scada display %d", key);
                m_allDisplays.push_back(TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getScadaDisplay(key));
            }
        }
        break;
    default:
        break;
    }
    if (m_editShortcutDlg.m_hWnd != NULL)
    {
        m_editShortcutDlg.PostMessage(WM_SCADA_DISPLAY_CHANGE);
    }
}


void ControlStationBar::unloadShortcuts()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "unloadShortcuts");
    
    if (m_settingsMgr == NULL)
    {
        return;
    }

    try
    {
        saveShortcuts();
    }
    catch (TA_Base_Core::UserSettingsException&)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_020054);
    }
    catch (...) 
    {
        // We'll get some kind of exception if a display has been deleted causing the operator's shortcut 
        // to point to an invalid display
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_020009);
        if (IDYES == selectedButton)
        {
            if (IDOK == m_editShortcutDlg.DoModal())
            {
                try
                {
                    synchroniseShortcuts();
                    saveShortcuts();
                }
                catch (...)     
                {
                    // we'll just give the operator 1 chance to update.  If there's more changes to the 
                    // scada displays, then that too bad.
                    userMsg.showMsgBox(IDS_UE_020054);
                }
            }
        }
    }

    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
        delete m_shortcuts[i];
        m_shortcuts[i] = NULL;
    }
    m_shortcuts.clear();
    m_oldShortcuts.clear();

    // delete the settings manager since we're done with the settings
    delete m_settingsMgr;
    m_settingsMgr = NULL;
}


void ControlStationBar::saveShortcuts()
{
    unsigned int newSize = m_shortcuts.size();
    unsigned int oldSize = m_oldShortcuts.size();

    // no need to save if there's nothing to save
    if ((oldSize == 0) && (newSize == 0))
    {
        return;
    }

    unsigned int i;
    CString name, value;
    unsigned int numCommon = oldSize;   // the number of items that are common in new and old lists

    // if we have added new items, then we need to add them to the database
    if (newSize > oldSize)
    {
        for (i = oldSize; i < newSize; i++)
        {
            name.Format(SHORTCUT_SETTING_NAME_FORMAT, TA_Base_App::TA_ControlStation::SHORTCUT_NAME, i);
            value.Format(SHORTCUT_SETTING_VALUE_FORMAT, m_shortcuts[i]->name.c_str(), m_shortcuts[i]->display->getKey());
            m_settingsMgr->saveSetting(std::string(name), std::string(value));
        }
        numCommon = oldSize;
    }
    // We'll just delete any old shortcuts that are no longer needed
    else if (newSize < oldSize)
    {
        for (i = newSize; i < oldSize; i++)
        {
            name.Format(SHORTCUT_SETTING_NAME_FORMAT, TA_Base_App::TA_ControlStation::SHORTCUT_NAME, i);
            m_settingsMgr->deleteSetting(std::string(name));
        }
        numCommon = newSize;
    }

    // if there's any changes made to the shortcuts, then we need to update the values
    for (i = 0; i < numCommon; i++)
    {
        value.Format(SHORTCUT_SETTING_VALUE_FORMAT, m_shortcuts[i]->name.c_str(), m_shortcuts[i]->display->getKey());

        // if the setting has changed, then save the changes
        if (m_oldShortcuts[i].compare(value) != TA_Base_App::TA_ControlStation::STRINGS_EQUIVALENT)
        {
            name.Format(SHORTCUT_SETTING_NAME_FORMAT, TA_Base_App::TA_ControlStation::SHORTCUT_NAME, i);
            m_settingsMgr->saveSetting(std::string(name), std::string(value));
        }
    }
}


void ControlStationBar::loadShortcuts()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadShortcuts");
    TA_ASSERT(m_settingsMgr == NULL, "The settings manager is not NULL");

    m_settingsMgr = new SettingsMgr(SessionManager::getInstance().getSessionDetails().getSessionId(),CONTROL_STATION);

    // save the shortcut details in 2 separate lists - the m_oldShortcuts will remain unchanged
    // during the session; m_shortcuts will contain any changes that are made to the shortcut list.
    m_oldShortcuts = m_settingsMgr->getSettingsLike(TA_Base_App::TA_ControlStation::SHORTCUT_NAME);
    int numDisplays = m_allDisplays.size();
    std::string name;
    TA_Base_Core::IScadaDisplay* display = NULL;
    long displayIndex; 

    for (unsigned int i = 0; i < m_oldShortcuts.size(); i++)
    {
        std::string::size_type index = m_oldShortcuts[i].find("\\"); // find the \ delimiter

        // if we can find the \ delimiter
        if (index != std::string::npos)
        {
            // see if the string after the delimiter is a number
            displayIndex = atol(m_oldShortcuts[i].substr(index+1, m_oldShortcuts[i].size()).c_str());

            // find the corresponding display first.  If we can't find it, then there's no
            // point showing the shortcut
            bool found;
            for (int j = 0; j < numDisplays; j++)
            {
                if ( m_allDisplays[j]->getKey() == static_cast<unsigned long>(displayIndex) )
                {
                    display = m_allDisplays[j];
                    found = true;
                    break;
                }
            }
            // if the display is valid, then we can
            if (found)
            {
                name = m_oldShortcuts[i].substr(0, index);

                EditShortcutDialog::ShortcutInformation* item = new struct EditShortcutDialog::ShortcutInformation;
                item->name = name;
                item->display = display;
                m_shortcuts.push_back(item);
            }
        }
    }

    refreshShortcutListBox();

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadShortcuts");
}


void ControlStationBar::refreshShortcutListBox()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "refreshShortcutListBox");
    m_favouritesComboBox.ResetContent();
    int index;
    for (unsigned int i = 0; i < m_shortcuts.size(); i++)
    {
        index = m_favouritesComboBox.AddString(m_shortcuts[i]->name.c_str());
        m_favouritesComboBox.SetItemData(index, reinterpret_cast<DWORD>(m_shortcuts[i]));
    }

    // Once all the "favourites" are added, add the "Organise Favourites..." to the front of the list
    m_favouritesComboBox.InsertString(0,"----------------------");
    m_favouritesComboBox.InsertString(0,"Organise Favourites...");
    LOG( SourceInfo, DebugUtil::FunctionExit, "refreshShortcutListBox");
}


void ControlStationBar::synchroniseShortcuts()
{
    // otherwise, need to synchronise the list
	unsigned int i = 0;
    for ( i = 0; i < m_shortcuts.size(); i++)
    {
        delete m_shortcuts[i];
        m_shortcuts[i] = NULL;
    }
    m_shortcuts.clear();

    EditShortcutDialog::ShortcutVector& shortcuts = m_editShortcutDlg.getShortcuts();
    for (i = 0; i < shortcuts.size(); i++)
    {
        EditShortcutDialog::ShortcutInformation* item = new struct EditShortcutDialog::ShortcutInformation;
        item->name = shortcuts[i]->name;
        item->display = shortcuts[i]->display;
        m_shortcuts.push_back(item);
    }
}



void ControlStationBar::OnEditchangeFavouritesCombo() 
{
}

void ControlStationBar::OnSelchangeFavouritesCombo() 
{
    FUNCTION_ENTRY("OnSelchangeFavouritesCombo");
    // If the "Configure" item was selected, open the dialog.
    int index = m_favouritesComboBox.GetCurSel();
    if (0 == index)
    {
        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Selected configuration from favourites list. Will now load favourites edit dialog.");
        m_editShortcutDlg.initialise(m_shortcuts, m_allDisplays, -1);
        if (m_editShortcutDlg.DoModal() == IDCANCEL)
        {
            return;
        }

        synchroniseShortcuts();
        refreshShortcutListBox();
    }
    else
    {
        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Selected favourite with index %d. Will now attempt to load favourite.",
            index);
        if (index == CB_ERR)
        {
            return;
        }

        // if for some reason the display path is empty, which should be, we'll just return.
        if (m_allDisplays[index]->getPath().empty())
        {
            return;
        }

        RECT rect = SessionManager::getInstance().getScreenPositioning().getAppRectNextToLaunchBar();
        DisplayInformation* info = new struct DisplayInformation;
        std::string path;
        try
        {
            path = m_allDisplays[index]->getPath();
        }
        catch (...)
        {
        }

        if ((info == NULL) || path.empty())
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                "Unable to create new DisplayInformation or path is empty");
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020064);
            return;
        }

        info->display = CString(path.c_str());
        info->asset = "";  // TODO: might want to consider having asset as a configurable item in the EditShortcutDialog
        info->position = (LPARAM)rect.left;
        ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
    }	
    FUNCTION_EXIT;
}
