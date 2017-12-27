/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/ScreenSaverLoginDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SCREENSAVERLOGINDLG_H__84888621_7F05_40F3_B49F_95CFE4A1B042__INCLUDED_)
#define AFX_SCREENSAVERLOGINDLG_H__84888621_7F05_40F3_B49F_95CFE4A1B042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenSaverLoginDlg.h : header file
//

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CScreenSaverLoginDlg dialog

    class CScreenSaverLoginDlg : public CDialog
    {
    // Construction
    public:
	    CString getPassword();
	    bool isDialogInitialized();
	    void setProfile(const CString &profile);
	    void setOperator(const CString &operatorName);
	    void setLocation(const CString &location);
	    CScreenSaverLoginDlg(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
	    //{{AFX_DATA(CScreenSaverLoginDlg)
	    enum { IDD = IDD_LOGIN_DLG };
	    CEdit	m_passwordEdit;
	    CButton	m_loginBtn;
	    CAnimateCtrl	m_introMovie;
	    CString	m_location;
	    CString	m_operator;
	    CString	m_profile;
	    CString	m_version;
	    CString	m_password;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CScreenSaverLoginDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CScreenSaverLoginDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnLoginOK();
	    afx_msg void OnLoginCancel();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_SCREENSAVERLOGINDLG_H__84888621_7F05_40F3_B49F_95CFE4A1B042__INCLUDED_)
