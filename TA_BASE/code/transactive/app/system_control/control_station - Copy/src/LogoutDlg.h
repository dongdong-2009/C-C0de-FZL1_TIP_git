/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/LogoutDlg.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is an extension of CDialog and is used to present user with
  * options of logging out, exitting control station and restarting workstation
  */

#if !defined(AFX_LOGOUTDIALOG_H__40BB5E10_2A97_47A9_A1B1_D201BD1D64DE__INCLUDED_)
#define AFX_LOGOUTDIALOG_H__40BB5E10_2A97_47A9_A1B1_D201BD1D64DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// LogoutDlg dialog

class LogoutDlg : public CDialog
{
// Construction
public:
	LogoutDlg(CWnd* pParent = NULL);   // standard constructor


    /** 
      * getTerminateCode
      *
      * Returns the way that user wants to terminate the session
      * - could be logout, restart or exit
      *
      * @return The way that user wants to terminate the session
      */
    TA_Base_App::TA_ControlStation::ETerminateMode getTerminateCode() const
    {
        return m_terminateCode;
    }

    
// Dialog Data
	//{{AFX_DATA(LogoutDlg)
	enum { IDD = IDD_ARIALBOLD_LOGOUT_DIALOG };
	CButton	m_helpCtrl;
	CButton	m_cancelCtrl;
	CButton	m_logoutCtrl;
	CButton	m_restartCtrl;
	CButton	m_exitCtrl;
	CAnimateCtrl	m_introMovie;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LogoutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(LogoutDlg)
	afx_msg void OnLogoutButton();
	afx_msg void OnExitButton();
	afx_msg void OnRestartButton();
	afx_msg void OnHelpButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void activateControls();

    // How the user wants to terminate
    TA_Base_App::TA_ControlStation::ETerminateMode m_terminateCode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGOUTDIALOG_H__40BB5E10_2A97_47A9_A1B1_D201BD1D64DE__INCLUDED_)
