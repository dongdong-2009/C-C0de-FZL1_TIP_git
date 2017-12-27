// AtsAlarmGeneratorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "atslistctrl.h"
#include "ThreadWorkerMgr.h"
#include "RequestHandler.h"
#include "IUpdateCounters.h"

// CAtsAlarmGeneratorDlg dialog
class CAtsAlarmGeneratorDlg : public CDialog, public IUpdateCounters
{
// Construction
public:
	CAtsAlarmGeneratorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ATSALARMGENERATOR_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnSubmit();
	afx_msg void OnBnClickedBtnGenAlarmid();
	afx_msg void OnCbnSelchangeCboSystem();
	afx_msg void OnBnClickedBtnClearAts();
	afx_msg void OnBnClickedBtnAutoSubmit();
	afx_msg void OnBnClickedChkAckClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnAck();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnNormalize();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnAutoSubmitFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutoAckCloseFinished(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnGenRandom();
	
	virtual void UpdateCounters(ULONG ulSubmitted, ULONG ulUnCloseCount);

	CComboBox m_cboSystem;
	CComboBox m_cboSubSystem;
	CComboBox m_cboSeverity;
	CComboBox m_cboLocationID;
	CComboBox m_cboAlarmType;
	CEdit m_edtAlarmID;
	CString m_strAlarmID;
	CString m_strSystemName;
	CString m_strSubSystemName;
	CString m_strDescription;
	CString m_strAlarmValue;
	CString m_strAssetName;
	CString m_strAlarmType;
	CString m_strLocation;

	CString m_strSeverity;
	CATSListCtrl m_listATSAlarms;
	bool m_bStartAutoSubmit;
	BOOL m_bChkAckClose;
	CEdit m_edtAckClose;
	CString m_strAlarmCount;
	CString m_strThreads;
	CString m_strSubmitInterval;
	CString m_strAckCloseInterval;
	CString m_strUnSubmitted;
	CString m_strUnclose;
	CComboBox m_cboMMSState;
	CString m_strMMSState;
	CComboBox m_cboAlarmState;
	CString m_strAlarmState;
	
private:
	void Initialize(void);
	void ClearManualFields(void);
	void ClearAutoFields(void);
	std::string _formatState(I_ALARM_PROVIDER::AlarmState state);
	void startAutoSubmit();
	void stopAutoSubmit();
	void updateManualButtons(BOOL bEnable);

	CThreadWorkerMgr * m_pThreadWorkerMgr;
	CRequestHandler * m_pRequestHandler;
	long m_nAutoAckTime;
	I_ALARM_PROVIDER::IISCSAlarm *m_pInterfaceATS;
	
};
