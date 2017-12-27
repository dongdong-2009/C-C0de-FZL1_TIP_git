#pragma once

#include "..\MessageGrid\MessageGridDoc.h"
// NewLangDlg dialog

class NewLangDlg : public CMenuDialog
{
	DECLARE_DYNAMIC(NewLangDlg)

public:
	NewLangDlg();
	NewLangDlg(CWnd* pParent, MessageGridDoc* pMessageDoc);   // standard constructor
	virtual ~NewLangDlg();

	CComboBox m_comboLanguage;
	void InitialiseComboBox();
	bool CheckIfLanguageExists(LCID lcidCurrent);

	MessageGridDoc* m_pMessageDoc;

// Dialog Data
	enum { IDD = IDD_NEWLANGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};
