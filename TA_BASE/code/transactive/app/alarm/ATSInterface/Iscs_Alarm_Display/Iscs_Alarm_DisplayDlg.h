// Iscs_Alarm_DisplayDlg.h : header file
//

#if !defined(AFX_ISCS_ALARM_DISPLAYDLG_H__CDF445B0_B862_4D9D_B6A8_5DA9678568DF__INCLUDED_)
#define AFX_ISCS_ALARM_DISPLAYDLG_H__CDF445B0_B862_4D9D_B6A8_5DA9678568DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CIscs_Alarm_DisplayDlg dialog

class CIscs_Alarm_DisplayDlg : public CDialog
{
// Construction
public:
	unsigned int DoThread();
	CIscs_Alarm_DisplayDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIscs_Alarm_DisplayDlg)
	enum { IDD = IDD_ISCS_ALARM_DISPLAY_DIALOG };
	CAlarmListCtrl	m_AlarmList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIscs_Alarm_DisplayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ChangeInfo();
	HICON m_hIcon;
	I_ALARM_PROVIDER::IISCSAlarm *m_pIscsAlarm;

	// Generated message map functions
	//{{AFX_MSG(CIscs_Alarm_DisplayDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonMms();
	afx_msg void OnButtonAck();
	afx_msg void OnButtonProfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISCS_ALARM_DISPLAYDLG_H__CDF445B0_B862_4D9D_B6A8_5DA9678568DF__INCLUDED_)
