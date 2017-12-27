// security_simulatorDlg.h : header file
//

#if !defined(AFX_SECURITY_SIMULATORDLG_H__702074BB_C70E_4B38_8D9C_E5C789D9E006__INCLUDED_)
#define AFX_SECURITY_SIMULATORDLG_H__702074BB_C70E_4B38_8D9C_E5C789D9E006__INCLUDED_

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "SessionHelper.h"
#include "DutyHelper.h"
#include "RightsHelper.h"
#include "CommonInterface.h"
#include "RightsCheckingThread.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorDlg dialog

class CSecurity_simulatorDlg : public CDialog, public IResultObserver
{
// Construction
public:
	CSecurity_simulatorDlg(CWnd* pParent = NULL);	// standard constructor

	virtual ~CSecurity_simulatorDlg();

	virtual void notify(bool result, std::string& resultString);

// Dialog Data
	//{{AFX_DATA(CSecurity_simulatorDlg)
	enum { IDD = IDD_SECURITY_SIMULATOR_DIALOG };
	CButton	m_btnAutoCheckDuty;
	CComboBox	m_locationCMB;
	CButton	m_btnDutyCheck;
	CEdit	m_result;
	CButton	m_btnRmOverride;
	CButton	m_btnOverride;
	CButton	m_btnLogout;
	CButton	m_btnLogin;
	long	m_loginLocation;
	CString	m_loginPassword;
	long	m_loginUserId;
	long	m_loginProfile;
	long	m_loginWorkstation;
	long	m_ORLocation;
	CString	m_ORPassword;
	long	m_ORProfile;
	long	m_ORUserId;
	long	m_ORWorkstation;
	long	m_actionKey;
	long	m_resourceKey;
	long	m_locationKey;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurity_simulatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void showSessionResult(const std::string& result);

	std::vector< std::string >			m_configedAgentLocation;

	TA_Base_Bus::AuthenticationLibrary	m_authLib;
	SessionHelper *						m_sessionHelper;
	DutyHelper *						m_dutyHelper;
	RightsHelper *						m_rightsHelper;
	RightsCheckingThread *				m_rightsCheckThread;
	std::string							m_loginSession;


protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSecurity_simulatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnLogin();
	afx_msg void OnBtnLogout();
	afx_msg void OnBtnOverride();
	afx_msg void OnBtnRmOverride();
	afx_msg void OnBtnDutycheck();
	afx_msg void OnChkAutocheckduty();
	afx_msg void OnBtnTakeduty();
	afx_msg void OnBtnLogsession();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITY_SIMULATORDLG_H__702074BB_C70E_4B38_8D9C_E5C789D9E006__INCLUDED_)
