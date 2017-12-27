/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/PasswordDlg.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Dialog box which allows the current operator to change password
  */

#include "app\system_control\control_station\src\stdafx.h"
#include "app\system_control\control_station\src\controlstation.h"
#include "app\system_control\control_station\src\PasswordDlg.h"
#include "app\system_control\control_station\src\ControlStation.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"

#include <vector>
#include <sstream>

#include "bus\generic_gui\src\HelpLauncher.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\SessionException.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\utilities\src\DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_App::TA_ControlStation::APPLICATION_NAME;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::SessionException;
using TA_Base_App::TA_ControlStation::MAX_PASSWORD_LENGTH;

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent),
      m_oldPassword(""),
      m_newPassword1(""),
      m_newPassword2(""),
      m_useExistingData(false)  //TD726 - need to set this to false so that the old password will get focus the first time
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_PASSWORD_EDIT, m_oldPasswordEdit);
	DDX_Control(pDX, IDC_NAME_COMBO, m_nameCombo);
	DDX_Control(pDX, IDC_NEW_PASSWORD_EDIT, m_newPasswordEdit);
	DDX_Control(pDX, IDOK, m_applyCtrl);
	DDX_Control(pDX, IDC_CONFIRM_PASSWORD_EDIT, m_confirmPasswordEdit);
	DDX_Control(pDX, IDC_ANIMATE_AB, m_introMovie);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_EN_CHANGE(IDC_NEW_PASSWORD_EDIT, OnChangeNewPasswordEdit)
	ON_EN_CHANGE(IDC_PASSWORD_EDIT, OnChangePasswordEdit)
	ON_EN_CHANGE(IDC_CONFIRM_PASSWORD_EDIT, OnChangeConfirmPasswordEdit)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
    ON_MESSAGE(WM_FORCE_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnChangePasswordEdit() 
{
    m_oldPasswordEdit.GetWindowText(m_oldPassword);
    m_newPasswordEdit.EnableWindow(!m_oldPassword.IsEmpty());

    // clear the passwords
    m_newPasswordEdit.SetWindowText("");
    m_confirmPasswordEdit.SetWindowText("");

    m_confirmPasswordEdit.EnableWindow(FALSE);
    m_applyCtrl.EnableWindow(FALSE);
}


void CPasswordDlg::OnChangeNewPasswordEdit() 
{
    m_newPasswordEdit.GetWindowText(m_newPassword1);
    m_confirmPasswordEdit.EnableWindow(!m_newPassword1.IsEmpty());
    m_confirmPasswordEdit.SetWindowText("");
    m_applyCtrl.EnableWindow(FALSE);
}


void CPasswordDlg::OnChangeConfirmPasswordEdit() 
{
    m_confirmPasswordEdit.GetWindowText(m_newPassword2);
    m_applyCtrl.EnableWindow(!m_newPassword2.IsEmpty());
}


BOOL CPasswordDlg::OnInitDialog() 
{
    BeginWaitCursor(); // display the hourglass cursor

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

    // Add "About..." menu item to system menu.
    // Remove the "Close" menu item from the system menu

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

	populateNameComboBox();

	// Set the name to the current operator
	//TD15552++
	m_nameCombo.SetCurSel(m_nameCombo.FindStringExact(-1,
						  SessionManager::getInstance().getSessionDetails().getOperatorName().c_str())); //TD15528
	//++TD15552

    // Make the Window always on top.
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

    // limit our password length
    m_oldPasswordEdit.SetLimitText(MAX_PASSWORD_LENGTH); 
    m_newPasswordEdit.SetLimitText(MAX_PASSWORD_LENGTH); 
    m_confirmPasswordEdit.SetLimitText(MAX_PASSWORD_LENGTH); 

    m_newPassword1 = "";
    m_newPassword2 = "";
    m_newPasswordEdit.SetWindowText("");
    m_confirmPasswordEdit.SetWindowText("");

    if (m_useExistingData)
    {
        m_oldPasswordEdit.SetWindowText(m_oldPassword);
        m_newPasswordEdit.SetFocus(); // if we already got the old password, then set focus to the new password
        m_useExistingData = false;
        EndWaitCursor(); // remove the hourglass cursor
        return FALSE;
    }
    EndWaitCursor(); // remove the hourglass cursor
    return TRUE;
}


void CPasswordDlg::OnOK() 
{
    m_oldPasswordEdit.GetWindowText(m_oldPassword);
    m_newPasswordEdit.GetWindowText(m_newPassword1);
    m_confirmPasswordEdit.GetWindowText(m_newPassword2);
	m_introMovie.Stop();
	m_introMovie.Close();
	CDialog::OnOK();
}


void CPasswordDlg::getSelectedDetails(
	TA_Base_Core::IOperator*& selectedOperator, CString& oldPassword, 
	CString& newPassword1, CString& newPassword2) const
{
	oldPassword = m_oldPassword;
	newPassword1 = m_newPassword1;
    newPassword2 = m_newPassword2;
}


void CPasswordDlg::populateNameComboBox()
{
    m_nameCombo.ResetContent();

    SessionDetails& details = SessionManager::getInstance().getSessionDetails();

    SessionDetails::OperatorVector& operators = details.getListOfOperators();

    SessionDetails::OperatorVector::iterator iter = operators.begin();
    while ( iter != operators.end() )
    {
        CString /*description*/ name(""); //TD15528
        try
        {
            /*description*/ name = (*iter)->getName().c_str(); //TD15528
        }
        catch ( DataException& )
        {
        }
        catch ( DatabaseException& )
        {
        }

        // Add each operator description to the combo box and store the IOperator* as the
        // associated data.
        int index = m_nameCombo.InsertString(0,/*description*/ name); //TD15528
        m_nameCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter) );

        ++iter;
    }
}


void CPasswordDlg::OnHelpButton() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}


void CPasswordDlg::OnSysCommand(UINT nID, LPARAM lParam) 
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

LRESULT CPasswordDlg::OnCancel( WPARAM, LPARAM )
{
	CDialog::OnCancel();
	return S_OK;
}
