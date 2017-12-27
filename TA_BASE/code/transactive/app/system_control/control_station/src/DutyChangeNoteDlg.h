#if !defined(AFX_DUTYCHANGENOTEDLG_H__EF4EBA02_4B42_4190_A991_4EA0B1683FE6__INCLUDED_)
#define AFX_DUTYCHANGENOTEDLG_H__EF4EBA02_4B42_4190_A991_4EA0B1683FE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DutyChangeNoteDlg.h : header file
// For TD19075

/////////////////////////////////////////////////////////////////////////////
// DutyChangeNoteDlg dialog

class DutyChangeNoteDlg : public CDialog
{
// Construction
public:
	DutyChangeNoteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DutyChangeNoteDlg)
	enum { IDD = IDD_DIALOG_DUTY_CHANGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DutyChangeNoteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DutyChangeNoteDlg)
	afx_msg void OnDetail();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUTYCHANGENOTEDLG_H__EF4EBA02_4B42_4190_A991_4EA0B1683FE6__INCLUDED_)
