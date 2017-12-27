#pragma once

#include "MsgExtractor.h"

// ExportToRCDlg dialog

class ExportToRCDlg : public CMenuDialog
{
	DECLARE_DYNAMIC(ExportToRCDlg)

public:
	ExportToRCDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportToRCDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTTORCDLG };

	MsgExtractor m_msgExtractor;

	CComboBox m_comboLanguage;

	bool InitialiseComboBox();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnExporttorcdlgTarget();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
