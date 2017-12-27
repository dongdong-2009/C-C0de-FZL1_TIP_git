// NewLangDlg.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "NewLangDlg.h"


// NewLangDlg dialog

IMPLEMENT_DYNAMIC(NewLangDlg, CMenuDialog)

NewLangDlg::NewLangDlg(CWnd* pParent, MessageGridDoc* pMessageDoc)
	: CMenuDialog(NewLangDlg::IDD, pParent), m_pMessageDoc(pMessageDoc)
{
}

NewLangDlg::~NewLangDlg()
{
}

void NewLangDlg::DoDataExchange(CDataExchange* pDX)
{
	CMenuDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_NEWLANGDLG, m_comboLanguage);
}


BEGIN_MESSAGE_MAP(NewLangDlg, CMenuDialog)
END_MESSAGE_MAP()


// NewLangDlg message handlers

void NewLangDlg::OnOK()
{
	int iIndex = m_comboLanguage.GetCurSel();
	if(iIndex < 0)
	{
		AfxMessageBox(_T("Please select the language to add before clicking on the Add button"));
		return;
	}

	if(m_pMessageDoc->AddNewLanguage(m_comboLanguage.GetItemData(iIndex)))
		CMenuDialog::OnOK();
}

BOOL NewLangDlg::OnInitDialog()
{
	CMenuDialog::OnInitDialog();

	InitialiseComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void NewLangDlg::InitialiseComboBox()
{
	int iIndex = 0;
	LCID lcidCurrent;
	CString sCurrent;

	for(int i = m_comboLanguage.GetCount(); i >= 0; i--)
		m_comboLanguage.DeleteString(i);

	MessageLibraryApp* pApp = GetApp();

	POSITION pos = pApp->m_mapAvailableLocales.GetStartPosition();
	while(pos)
	{
		pApp->m_mapAvailableLocales.GetNextAssoc(pos, lcidCurrent, sCurrent);
		
		if(CheckIfLanguageExists(lcidCurrent))
			continue;

		iIndex = m_comboLanguage.AddString(sCurrent);
		m_comboLanguage.SetItemData(iIndex, lcidCurrent);
	}
}

bool NewLangDlg::CheckIfLanguageExists(LCID lcidCurrent)
{
	for(int i = 0, iCount = m_pMessageDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize(); 
		i < iCount; i++)
	{
		if(lcidCurrent == m_pMessageDoc->m_messageXMLObj.m_rgSupportedLocales[i])
			return true;
	}
	return false;
}

