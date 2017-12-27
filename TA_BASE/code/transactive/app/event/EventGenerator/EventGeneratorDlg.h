// EventGeneratorDlg.h : header file
//

#pragma once

#include "app/event/EventGenerator/ThreadSubmitMgr.h"
#include "afxwin.h"

// CEventGeneratorDlg dialog
class CEventGeneratorDlg : public CDialog
{
// Construction
public:
	CEventGeneratorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EVENTGENERATOR_DIALOG };
	afx_msg void OnBnClickedBtnAutoSubmit();

	void startAutoSubmit();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg void OnBnClickedBtnExit();
	
	afx_msg void OnBnClickedBtnClearAll();
	afx_msg LRESULT OnUpdateCounters(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateErrorCount(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CString m_strEventSize;
	CString m_strThreads;
	CString m_strSubmitInterval;
	CString m_strSubmitLocation;
	CString m_strBatchSize;
	CString m_strErrorCount;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	
	void stopAutoSubmit();

private: 
	bool m_bStartAutoSubmit;
	ThreadSubmitMgr* m_pSubmitMgr;
	unsigned long m_nAutoSubmitTime;
public:
	CComboBox m_cboEventType;
	CString m_strPrefix;
};
