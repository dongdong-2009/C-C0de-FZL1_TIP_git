// MessageExtractorDlg.h : header file
//

#pragma once

#include "MsgExtractor.h"

// MessageExtractorDlg dialog
class MessageExtractorDlg : public CMenuDialog
{
// Construction
public:
	MessageExtractorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MESSAGEEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	MsgExtractor m_msgExtractor;

// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedBtnSourceBrowse();
	afx_msg void OnBnClickedOk();
};
