#if !defined(AFX_CONFIGDLG_H__B973C670_D70A_4CEB_B02E_DEB41F6D7707__INCLUDED_)
#define AFX_CONFIGDLG_H__B973C670_D70A_4CEB_B02E_DEB41F6D7707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// configdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(UINT severity, UINT location, CString asset, CString description, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG_DIALOG };
	int	m_severityValue;
	int	m_locationValue;
	CString	m_descValue;
	CString	m_assetValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCancel();
	afx_msg void OnChk1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__B973C670_D70A_4CEB_B02E_DEB41F6D7707__INCLUDED_)
