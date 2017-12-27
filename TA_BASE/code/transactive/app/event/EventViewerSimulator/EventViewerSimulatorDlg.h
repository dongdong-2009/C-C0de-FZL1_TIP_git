// EventViewerSimulatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "app/event/EventViewerSimulator/EventListCtrl.h"

// CEventViewerSimulatorDlg dialog
class CEventViewerSimulatorDlg : public CDialog
{
// Construction
public:
	CEventViewerSimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EVENTVIEWERSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUpdateCounters(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CEventListCtrl m_objListCtrl;
	CString m_txtRetrieveInterval;
	CString m_txtEventCount;
	afx_msg void OnBnClickedBtnStart();
	CString m_AgentLocation;
	bool m_bStartAgentQuery;
	unsigned long m_nQueryTime;
	int m_bIsLogNeeded;
};
