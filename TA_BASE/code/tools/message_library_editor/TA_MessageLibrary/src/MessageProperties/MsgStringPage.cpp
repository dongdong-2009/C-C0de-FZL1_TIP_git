// MsgStringPage.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "MsgStringPage.h"
#include "MessagePropertySheet.h"


// MsgStringPage dialog

IMPLEMENT_DYNAMIC(MsgStringPage, MsgPropertyPage)

MsgStringPage::MsgStringPage(LCID lcidLanguage)
	: MsgPropertyPage(MsgStringPage::IDD), m_lcidLanguage(lcidLanguage)
{
	CString sLocale;
	GetApp()->m_mapAvailableLocales.Lookup(m_lcidLanguage, sLocale);
	
	m_sTitle = _T("Message ");
	m_sTitle += sLocale;
}

MsgStringPage::~MsgStringPage()
{
}

void MsgStringPage::DoDataExchange(CDataExchange* pDX)
{
	CString sTempMsg;

	MsgPropertyPage::DoDataExchange(pDX);

	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_ED_MSGSTRINGPAGE_STRING, sTempMsg);
		pSheet->m_pMessage->m_mapMessages.SetAt(m_lcidLanguage, sTempMsg);
	}
	else
	{
		pSheet->m_pMessage->m_mapMessages.Lookup(m_lcidLanguage, sTempMsg);
		DDX_Text(pDX, IDC_ED_MSGSTRINGPAGE_STRING, sTempMsg);
	}
}


BEGIN_MESSAGE_MAP(MsgStringPage, MsgPropertyPage)
	
END_MESSAGE_MAP()


// MsgStringPage message handlers

BOOL MsgStringPage::OnInitDialog()
{
	MsgPropertyPage::OnInitDialog();

	CEdit* pEdit = reinterpret_cast<CEdit*>(GetDlgItem(IDC_ED_MSGSTRINGPAGE_STRING));

	AddSzControl(*pEdit, mdResize, mdResize);

	// Disable the australian message in external mode
	if(!GetApp()->m_bInternalMode && m_lcidLanguage == 3081)
		pEdit->SetReadOnly();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

