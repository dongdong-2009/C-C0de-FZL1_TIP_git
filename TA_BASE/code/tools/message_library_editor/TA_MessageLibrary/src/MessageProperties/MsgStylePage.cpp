// MsgStylePage.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "MsgStylePage.h"
#include "MessagePropertySheet.h"


// MsgStylePage dialog

IMPLEMENT_DYNAMIC(MsgStylePage, MsgPropertyPage)

MsgStylePage::MsgStylePage()
	: MsgPropertyPage(MsgStylePage::IDD)
{
}

MsgStylePage::~MsgStylePage()
{
}

void MsgStylePage::DoDataExchange(CDataExchange* pDX)
{
	int iButton = 0, iDefaultButton = 0, iIcon = 0, iChkHelp = 0;
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();

	MsgPropertyPage::DoDataExchange(pDX);

	if(pDX->m_bSaveAndValidate)
	{
		pSheet->m_pMessage->m_ulMsgBoxStyle = 0;

		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_OK, iButton);
		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_DEF1, iDefaultButton);
		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_NOICON, iIcon);
		DDX_Check(pDX, IDC_CHK_MSGSTYLEPAGE_HELPBTN, iChkHelp);

		switch(iButton)
		{
			case 0: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_OK; break;
			case 1: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_OKCANCEL; break;
			case 2: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_YESNO; break;
			case 3: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_YESNOCANCEL; break;
			case 4: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_RETRYCANCEL; break;
			case 5: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_ABORTRETRYIGNORE; break;
//			case 6: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_CANCELTRYCONTINUE; break;
		}

		switch(iDefaultButton)
		{
			case 0: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_DEFBUTTON1; break;
			case 1: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_DEFBUTTON2; break;
			case 2: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_DEFBUTTON3; break;
			case 3: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_DEFBUTTON4; break;
		}

		switch(iIcon)
		{
			case 0: break;
			case 1: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_ICONERROR; break;
			case 2: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_ICONWARNING; break;
			case 3: pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_ICONINFORMATION; break;
		}

		if(iChkHelp)
			pSheet->m_pMessage->m_ulMsgBoxStyle |= MB_HELP;
	}
	else
	{
		switch (pSheet->m_pMessage->m_ulMsgBoxStyle & MB_TYPEMASK)
        {
        case MB_OKCANCEL: iButton = 1; break;
        case MB_YESNO: iButton = 2; break;
        case MB_YESNOCANCEL: iButton = 3; break;
        case MB_RETRYCANCEL: iButton = 4; break;
        case MB_ABORTRETRYIGNORE: iButton = 5; break;
        default: iButton = 0; break;
        };
  
        switch (pSheet->m_pMessage->m_ulMsgBoxStyle & MB_DEFMASK)
        {
        case MB_DEFBUTTON2: iDefaultButton = 1; break;
        case MB_DEFBUTTON3: iDefaultButton = 2; break;
        case MB_DEFBUTTON4: iDefaultButton = 3; break;
        default: iDefaultButton = 0; break;
        };

		switch (pSheet->m_pMessage->m_ulMsgBoxStyle & MB_ICONMASK)
        {
        case MB_ICONERROR: iIcon = 1; break;
        case MB_ICONWARNING: iIcon = 2; break;
        case MB_ICONINFORMATION: iIcon = 3; break;
        default: iIcon = 0; break;
        };

		if(MB_HELP == (MB_MISCMASK & pSheet->m_pMessage->m_ulMsgBoxStyle & MB_HELP))
        {
			iChkHelp = 1;
        }
        else
        {
            iChkHelp = 0;
        }

		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_OK, iButton);
		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_DEF1, iDefaultButton);
		DDX_Radio(pDX, IDC_RADIO_MSGSTYLEPAGE_NOICON, iIcon);
		DDX_Check(pDX, IDC_CHK_MSGSTYLEPAGE_HELPBTN, iChkHelp);
	}
}


BEGIN_MESSAGE_MAP(MsgStylePage, MsgPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_OK, OnChgMsgBtnOK)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_OKCANCEL, OnChgMsgBtnOKCANCEL)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_YESNO, OnChgMsgBtnYESNO)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_YESNOCANCEL, OnChgMsgBtnYESNOCANCEL)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_RETRYCANCEL, OnChgMsgBtnRETRYCANCEL)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_ABORTRETRYIGNORE, OnChgMsgBtnABORTRETRYCANCEL)
	ON_BN_CLICKED(IDC_RADIO_MSGSTYLEPAGE_CANCELTRYCONT, OnChgMsgBtnABORTTRYCANCEL)
	ON_BN_CLICKED(IDC_CHK_MSGSTYLEPAGE_HELPBTN, OnChgHelpBtn)
END_MESSAGE_MAP()

// MsgStylePage message handlers

BOOL MsgStylePage::OnInitDialog()
{
	MsgPropertyPage::OnInitDialog();

	SetButtons();

	if(!GetApp()->m_bInternalMode)
		DisableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MsgStylePage::ValidateDefaultButton(int iBtnCount)
{
	CButton* pButton = NULL;
	
	if(IsDlgButtonChecked(IDC_CHK_MSGSTYLEPAGE_HELPBTN) == 1)
		iBtnCount++;

	switch(iBtnCount)
	{
		case 1:
		case 2:	
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF2));
				if(pButton->IsWindowEnabled() && iBtnCount != 2)
					pButton->EnableWindow(FALSE);
			}
		case 3:	
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF3));
				if(pButton->IsWindowEnabled() && iBtnCount != 3)
					pButton->EnableWindow(FALSE);
			}
		case 4:	
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF4));
				if(pButton->IsWindowEnabled() && iBtnCount != 4)
					pButton->EnableWindow(FALSE);
			}
	}

	switch(iBtnCount)
	{
		case 4:
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF4));
				if(pButton->GetCheck() == 1)
					break;
				
				if(!pButton->IsWindowEnabled())
					pButton->EnableWindow(TRUE);
			}
		case 3:
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF3));
				if(pButton->GetCheck() == 1)
					break;

				if(!pButton->IsWindowEnabled())
					pButton->EnableWindow(TRUE);
			}
		case 2:
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF2));
				if(pButton->GetCheck() == 1)
					break;

				if(!pButton->IsWindowEnabled())
					pButton->EnableWindow(TRUE);
			}
		case 1:
			{
				pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF1));
				if(pButton->GetCheck() == 0)
					CheckRadioButton(IDC_RADIO_MSGSTYLEPAGE_DEF1, IDC_RADIO_MSGSTYLEPAGE_DEF4, IDC_RADIO_MSGSTYLEPAGE_DEF1);
				
			}break;
	}

}

void MsgStylePage::SetButtons()
{
	int iBtnCount = 1;

	if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_OK) == 1)
		iBtnCount = 1;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_OKCANCEL) == 1)
		iBtnCount = 2;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_YESNO) == 1)
		iBtnCount = 2;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_RETRYCANCEL) == 1)
		iBtnCount = 2;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_YESNOCANCEL) == 1)
		iBtnCount = 3;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_ABORTRETRYIGNORE) == 1)
		iBtnCount = 3;
	else if(IsDlgButtonChecked(IDC_RADIO_MSGSTYLEPAGE_CANCELTRYCONT) == 1)
		iBtnCount = 3;

	ValidateDefaultButton(iBtnCount);
}

void MsgStylePage::DisableControls()
{
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_OKCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_YESNO)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_YESNOCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_RETRYCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_ABORTRETRYIGNORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_CANCELTRYCONT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF1)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF2)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF3)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_DEF4)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_MSGSTYLEPAGE_HELPBTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_NOICON)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_ERROR)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_WARNING)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_MSGSTYLEPAGE_INFO)->EnableWindow(FALSE);
}