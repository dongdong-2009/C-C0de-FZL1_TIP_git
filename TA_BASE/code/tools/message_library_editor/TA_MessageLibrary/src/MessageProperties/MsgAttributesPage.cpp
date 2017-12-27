// MsgAttributesPage.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "MsgAttributesPage.h"
#include "MessagePropertySheet.h"


// MsgAttributesPage dialog

IMPLEMENT_DYNAMIC(MsgAttributesPage, MsgPropertyPage)

MsgAttributesPage::MsgAttributesPage()
	: MsgPropertyPage(MsgAttributesPage::IDD)
{
}

MsgAttributesPage::~MsgAttributesPage()
{
}

void MsgAttributesPage::DoDataExchange(CDataExchange* pDX)
{
    TRACE(_T("DoDataExchange called\n"));

	MsgPropertyPage::DoDataExchange(pDX);

	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();

	DDX_Text(pDX, IDC_ED_MSGATTRIBUTESPAGE_ID, pSheet->m_pMessage->m_ulID);
	DDX_Text(pDX, IDC_ED_MSGATTRIBUTESPAGE_HELPID, pSheet->m_pMessage->m_ulHelpID);
    
    

	if(pDX->m_bSaveAndValidate)
	{
        // validate the ResourceID string
        // it shouldn't be empty and
        // all its characters should be alphanumeric or '-' or '_'
        pDX->PrepareEditCtrl(IDC_ED_MSGATTRIBUTESPAGE_RES_ID);
        CString strResID;
        GetDlgItem(IDC_ED_MSGATTRIBUTESPAGE_RES_ID)->GetWindowText(strResID);
        if (-1 == strResID.FindOneOf(_T("~`@#$%^&*()+=\\|}{[]'\":;?/.,<>"))
            && ! strResID.IsEmpty())
        {
            DDX_Text(pDX, IDC_ED_MSGATTRIBUTESPAGE_RES_ID, pSheet->m_pMessage->m_ResID);
        }
        else
        {
            pDX->Fail();
        }
	
        int iTemp = 0;
		DDX_Check(pDX, IDC_CHK_MSGATTRIBUTESPAGE_OBSOLETE, iTemp);
		pSheet->m_pMessage->m_bObsolete = (iTemp == 1);

		DDX_Check(pDX, IDC_CHK_MSGATTRIBUTESPAGE_AUDIT_EVENT, iTemp);
		pSheet->m_pMessage->m_bAuditEvent = (iTemp == 1);

		DDX_Radio(pDX, IDC_RADIO_MSGATTRIBUTESPAGE_INFO, iTemp);
		pSheet->m_pMessage->m_usMessageType = iTemp + 1;
	}
	else
	{
		int iTemp = pSheet->m_pMessage->m_usMessageType - 1;
		DDX_Radio(pDX, IDC_RADIO_MSGATTRIBUTESPAGE_INFO, iTemp);

		iTemp = (pSheet->m_pMessage->m_bObsolete) ? 1 : 0;
		DDX_Check(pDX, IDC_CHK_MSGATTRIBUTESPAGE_OBSOLETE, iTemp);

		iTemp = (pSheet->m_pMessage->m_bAuditEvent) ? 1 : 0;
		DDX_Check(pDX, IDC_CHK_MSGATTRIBUTESPAGE_AUDIT_EVENT, iTemp);

        DDX_Text(pDX, IDC_ED_MSGATTRIBUTESPAGE_RES_ID, pSheet->m_pMessage->m_ResID);
	}
}


BEGIN_MESSAGE_MAP(MsgAttributesPage, MsgPropertyPage)
	ON_EN_CHANGE(IDC_ED_MSGATTRIBUTESPAGE_HELPID, OnChangeHelpID)
END_MESSAGE_MAP()


// MsgAttributesPage message handlers

BOOL MsgAttributesPage::OnInitDialog()
{
	MsgPropertyPage::OnInitDialog();

	
	if(!GetApp()->m_bInternalMode)
		DisableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MsgAttributesPage::OnChangeHelpID()
{
	CString sText;
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	GetDlgItem(IDC_ED_MSGATTRIBUTESPAGE_HELPID)->GetWindowText(sText);

	if(sText != _T(""))
	{
		ULONG ulHelpID = _ttol(sText);
		if(ulHelpID > 0)
			pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_HELP;
	}
}

void MsgAttributesPage::DisableControls()
{
	GetDlgItem(IDC_ED_MSGATTRIBUTESPAGE_HELPID)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGATTRIBUTESPAGE_INFO)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGATTRIBUTESPAGE_WARN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGATTRIBUTESPAGE_ERROR)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_MSGATTRIBUTESPAGE_OBSOLETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_MSGATTRIBUTESPAGE_AUDIT_EVENT)->EnableWindow(FALSE);
    GetDlgItem(IDC_ED_MSGATTRIBUTESPAGE_RES_ID)->EnableWindow(FALSE);
}