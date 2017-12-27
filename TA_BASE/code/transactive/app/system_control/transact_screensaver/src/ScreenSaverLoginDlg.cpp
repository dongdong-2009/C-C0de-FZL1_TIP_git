/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/ScreenSaverLoginDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ScreenSaverLoginDlg.cpp : implementation file
//

#include "stdafx.h"

#include "app/system_control/transact_screensaver/src/saver.h"
#include "app/system_control/transact_screensaver/src/ScreenSaverLoginDlg.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CScreenSaverLoginDlg dialog

    static const CString AVI_FILE = "TransActiveIntro.avi";

    CScreenSaverLoginDlg::CScreenSaverLoginDlg(CWnd* pParent /*=NULL*/)
	    : CDialog(CScreenSaverLoginDlg::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(CScreenSaverLoginDlg)
	    m_location = _T("");
	    m_operator = _T("");
	    m_profile = _T("");
	    m_version = _T("");
	    m_password = _T("");
	    //}}AFX_DATA_INIT
    }


    void CScreenSaverLoginDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CScreenSaverLoginDlg)
	    DDX_Control(pDX, IDC_PASSWORD_EDIT_AB, m_passwordEdit);
	    DDX_Control(pDX, IDC_LOGIN_BUTTON_AB, m_loginBtn);
	    DDX_Control(pDX, IDC_ANIMATE_AB, m_introMovie);
	    DDX_Text(pDX, IDC_LOCATION_EDIT, m_location);
	    DDX_Text(pDX, IDC_NAME_EDIT, m_operator);
	    DDX_Text(pDX, IDC_PROFILE_EDIT, m_profile);
	    DDX_Text(pDX, IDC_VERSION, m_version);
	    DDX_Text(pDX, IDC_PASSWORD_EDIT_AB, m_password);
	    DDV_MaxChars(pDX, m_password, 30);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CScreenSaverLoginDlg, CDialog)
	    //{{AFX_MSG_MAP(CScreenSaverLoginDlg)
	    ON_BN_CLICKED(IDC_LOGIN_BUTTON_AB, OnLoginOK)
	    ON_BN_CLICKED(IDC_CANCEL, OnLoginCancel)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CScreenSaverLoginDlg message handlers

    BOOL CScreenSaverLoginDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    char szAppPath[MAX_PATH] = "";
        ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	    CString aviPath = szAppPath;
        aviPath = aviPath.Left(aviPath.ReverseFind('\\'));

        // Append the AVI name
        aviPath += "\\";
        aviPath += AVI_FILE;

        // Finally open and play the movie
        if( !m_introMovie.Open(aviPath) )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AVI specified could not be found: %s", aviPath );		
        }
        else
        {
	        m_introMovie.Play(0, -1, 1);
        }

	    UpdateData(FALSE);

	    if( isDialogInitialized() )
	    {
		    m_passwordEdit.EnableWindow(TRUE);
		    m_loginBtn.EnableWindow(TRUE);			
	    }

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CScreenSaverLoginDlg::setLocation(const CString &location)
    {
	    m_location = location;
    }


    void CScreenSaverLoginDlg::setOperator(const CString &operatorName)
    {
	    m_operator = operatorName;
    }


    void CScreenSaverLoginDlg::setProfile(const CString &profile)
    {
	    m_profile = profile;
    }


    bool CScreenSaverLoginDlg::isDialogInitialized()
    {
	    return (!m_profile.IsEmpty() && !m_location.IsEmpty() && !m_operator.IsEmpty());
    }


    CString CScreenSaverLoginDlg::getPassword()
    {	
	    return m_password;
    }


    void CScreenSaverLoginDlg::OnLoginOK() 
    {
	    UpdateData();	
	    m_introMovie.Stop();
        m_introMovie.Close();

	    CDialog::OnOK();	
    }


    void CScreenSaverLoginDlg::OnLoginCancel() 
    {
	    m_introMovie.Stop();
        m_introMovie.Close();
	    CDialog::OnCancel();
    }
}
